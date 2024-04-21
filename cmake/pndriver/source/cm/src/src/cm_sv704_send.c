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
/*  F i l e               &F: cm_sv704_send.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, R1 Bridge                                                     */
/*                                                                           */
/*  - Send Message                                                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7704
#define CM_MODULE_ID       7704

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_BOOL
cm_sv_r1b_send_next(
	CM_SV_DEVICE_PTR_TYPE dev
); /* forward */

/*===========================================================================*/
/*=== R1-BRIDGE ALLOC PREFIX ================================================*/
/*===========================================================================*/

CM_SV_R1BMSG_PREFIX_PTR_TYPE
cm_sv_r1b_prefix_alloc(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 max_body_size /* special case: CM_SV_R1B_PREFIX_ALLOC_MAX */
) {
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix;
	LSA_UINT32 alloc_size = sizeof(*prefix);

	if( max_body_size == CM_SV_R1B_PREFIX_ALLOC_MAX ) {

		alloc_size += dev->R1_bridge_max;
	}
	else if( CM_SV_R1BMSG_SIZE_HEADER + max_body_size <= dev->R1_bridge_max ) {

		alloc_size += CM_SV_R1BMSG_SIZE_HEADER + max_body_size;
	}
	else {

		return LSA_NULL;
	}

	/***/

	prefix = (CM_SV_R1BMSG_PREFIX_PTR_TYPE)cm_sv_upper_mem_alloc(dev->channel, alloc_size);

	if( cm_is_not_null(prefix) ) {

		prefix->message_hdr = (CM_COMMON_MEM_U8_PTR_TYPE)(prefix + 1);
		prefix->message_max = alloc_size - sizeof(*prefix) - CM_SV_R1BMSG_SIZE_HEADER;
	}

	return prefix;
}

/*===========================================================================*/
/*=== R1-BRIDGE FREE PREFIX =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_prefix_free(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	cm_sv_upper_mem_free(channel, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE GET MESSAGE BODY FROM PREFIX ================================*/
/*===========================================================================*/

CM_COMMON_MEM_U8_PTR_TYPE
cm_sv_r1b_get_message_body_from_prefix(
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix,
	LSA_UINT32 * message_max /* out-param (LSA_NULL: size will be checked in post function) */
) {
	if( cm_is_not_null(message_max) ) {

		*message_max = prefix->message_max;
	}

	return prefix->message_hdr + CM_SV_R1BMSG_SIZE_HEADER;
}

/*===========================================================================*/
/*=== R1-BRIDGE SEND PROVIDE ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_r1bridge_send_provide(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_R1BRIDGE_SEND_PTR_TYPE param = rb->args.sv.r1bridge_send;
	CM_SV_DEVICE_PTR_TYPE dev;

	dev = cm_sv_dev_get_added(channel, param->device_nr);

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) is not added"
			, param->device_nr
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else if( ! dev->R1_enabled || CM_SV_DEV_IS_ACTIVE(dev) || cm_is_not_null(dev->r1b.send.resource) ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) not R1_enabled OR device active OR already provided"
			, dev->device_nr
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else {

		dev->r1b.send.resource = rb;

		if( ! CmListIsEmpty(&dev->r1b.send.queue) ) {
			CM_FATAL(); /* a bug */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE SEND CANCEL =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_send_res_cancel(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( cm_is_not_null(dev->r1b.send.resource) ) {

		CM_UPPER_RQB_PTR_TYPE rb = dev->r1b.send.resource;
		dev->r1b.send.resource = LSA_NULL;

		CM_ASSERT(CM_RQB_GET_OPCODE(rb) == CM_OPC_SV_R1BRIDGE_SEND_PROVIDE);

		cm_callback(dev->channel, CM_OK_CANCELLED, &rb);
	}

	if( ! CmListIsEmpty(&dev->r1b.send.queue) ) {
		CM_FATAL(); /* unreachable */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE SEND RESPONSE ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_r1bridge_send_rsp(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_R1BRIDGE_SEND_PTR_TYPE param = rb->args.sv.r1bridge_send;
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = (CM_SV_R1BMSG_PREFIX_PTR_TYPE)param->message_raw;
	CM_AR_GRAPH_PTR_TYPE argr;
	CM_SV_DEVICE_PTR_TYPE dev;

	if( cm_is_null(prefix) || (LSA_VOID_PTR_TYPE)(prefix + 1) != param->message_data ) {

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}

	/***/

	if( param->session_key == 0 ) { /* see cm_sv_r1b_post1() */

		argr = LSA_NULL;

		dev = cm_sv_dev_lookup(channel, param->device_nr);

		if( cm_is_null(dev) ) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) could not cm_sv_dev_lookup"
				, param->device_nr
				);

			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
		}

		if( prefix->session_key != 0 || prefix->alarm_prio != 0 ) {
			CM_FATAL(); /* a bug */
		}
	}
	else { /* see cm_sv_r1b_post2() and cm_sv_r1b_post3() */

		argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, 0); //R1 VV: maybe CM_SV_AR_CNFL_EXP_USER_REQ could be used here (stronger check).

		if( cm_is_null(argr) ) {

			CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
				, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
				, param->session_key
				);

			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
		}

		if( prefix->alarm_prio > 1 || cm_is_null(argr->cm.sv.arset) || argr->cm.sv.r1_flags == 0 ) {
			CM_FATAL(); /* a bug */
		}

		dev = argr->cm.sv.dev;
	}

	/***/

	CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u) r1bridge-sent: numb(%u) size(%u)"
		, dev->device_nr, prefix->message_numb, prefix->message_size
		);

	if( cm_is_null(argr) ) {

		switch(prefix->message_numb) {

		case CM_SV_R1BRIDGE_MSG_CONNECT_DONE:
			cm_sv_r1b_posted_connect_done_error(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_UPDATE_BEGIN:
			cm_sv_r1b_posted_update_begin(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_UPDATE_DIAG:
			cm_sv_r1b_posted_update_diag(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_UPDATE_ALARMS:
			cm_sv_r1b_posted_update_alarms(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_UPDATE_END:
			cm_sv_r1b_posted_update_end(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_DIAG_CHANGED:
			cm_sv_r1b_posted_diag_changed(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_DIAG_REPLACE_PDEV:
			cm_sv_r1b_posted_diag_replace_pdev(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ:
		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE:
		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN:
		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_END:
			cm_sv_r1b_pdev_A_posted(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ_DONE:
		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE_DONE:
		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN_DONE:
		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_END_DONE:
			cm_sv_r1b_pdev_B_posted(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_SET:
			cm_sv_r1b_posted_pdev_alarm_mirror_set(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CLR:
			cm_sv_r1b_posted_pdev_alarm_mirror_clr(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CANCEL:
			cm_sv_r1b_posted_pdev_alarm_mirror_cancel(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS:
			cm_sv_r1b_stat_A_posted(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS_DONE:
			cm_sv_r1b_stat_B_posted(dev, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_RESET_TO_FACTORY:
			cm_sv_r1b_posted_reset_to_factory(dev, prefix);
			break;

		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}
	else { /* note: argr and prio are validated above */

		switch(prefix->message_numb) {

		case CM_SV_R1BRIDGE_MSG_CONNECT:
			cm_sv_r1b_posted_connect(argr, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_CONNECT_DONE:
			cm_sv_r1b_posted_connect_done(argr, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT:
			cm_sv_r1b_posted_sub_state(argr, prefix); /* connect */
			break;

		case CM_SV_R1BRIDGE_MSG_DISCONNECT:
			cm_sv_r1b_posted_disconnect(argr, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_DISCONNECT_DONE:
			cm_sv_r1b_posted_disconnect_done(argr, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT:
			cm_sv_r1b_posted_update_connect(argr, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE:
			cm_sv_r1b_posted_update_sub_state(argr, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_ALARM_BEGIN:
			cm_sv_r1b_posted_alarm_begin(argr, prefix->alarm_prio, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_ALARM_END:
			cm_sv_r1b_posted_alarm_end(argr, prefix->alarm_prio, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PLUG_GROUP:
			cm_sv_r1b_posted_alarm_begin(argr, prefix->alarm_prio, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_ALARM_END_PLUG_GROUP:
			cm_sv_r1b_posted_alarm_end(argr, prefix->alarm_prio, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PDEV_GROUP:
			cm_sv_r1b_posted_alarm_begin(argr, prefix->alarm_prio, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_ALARM_END_PDEV_GROUP:
			cm_sv_r1b_posted_alarm_end(argr, prefix->alarm_prio, prefix);
			break;

		case CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END:
			cm_sv_r1b_posted_sub_state(argr, prefix); /* prm-begin-end */
			break;

		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}

	/***/

	if( cm_is_not_null(dev->r1b.send.resource) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_RQB_SET_OPCODE(rb, CM_OPC_SV_R1BRIDGE_SEND_PROVIDE);

		dev->r1b.send.resource = rb;

		/***/

		if( cm_sv_r1b_send_next(dev) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) r1bridge-send.rsp: all sent"
				, dev->device_nr
				);

			cm_sv_r1b_join_broken(dev);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE SEND NEXT ===================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_r1b_send_next( /* returns LSA_TRUE if idle */
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( cm_is_null(dev->r1b.send.resource) ) {

		return LSA_FALSE; /* not idle, waiting for send.rsp */
	}
	else if( ! CmListIsEmpty(&dev->r1b.send.queue) ) {

		CM_UPPER_RQB_PTR_TYPE rb;
		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix;

		rb = dev->r1b.send.resource;
		dev->r1b.send.resource = LSA_NULL;

		CmListRemoveHead(&dev->r1b.send.queue, prefix, CM_SV_R1BMSG_PREFIX_PTR_TYPE);

		{
		CM_UPPER_SV_R1BRIDGE_SEND_PTR_TYPE param = rb->args.sv.r1bridge_send;

		CM_ASSERT(param->device_nr == dev->device_nr);

		param->session_key = prefix->session_key;
		param->message_size = prefix->message_size + CM_SV_R1BMSG_SIZE_HEADER;
		param->message_data = prefix->message_hdr;
		param->message_raw  = prefix; /* see cm_sv_opc_r1bridge_send_rsp() */
		}

		CM_RQB_SET_OPCODE(rb, CM_OPC_SV_R1BRIDGE_SEND_IND);

		/***/

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev(%u) r1bridge-sending: numb(%u) size(%u)"
			, dev->device_nr, prefix->message_numb, prefix->message_size
			);

		cm_callback(dev->channel, CM_OK, &rb);

		return LSA_FALSE; /* not idle, waiting for send.rsp */
	}
	else {

		return LSA_TRUE; /* idle */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE SEND MESSAGE ================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_send(
	CM_SV_DEVICE_PTR_TYPE dev,
	CLRPC_UUID_CONST_PTR_TYPE ar_uuid,
	LSA_UINT16 session_key,
	LSA_UINT16 alarm_prio,
	LSA_UINT16 message_numb,
	LSA_UINT32 message_body_size, /* without header offset */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {

	if( message_body_size > prefix->message_max ) {

		CM_FATAL();
	}
	else {

		CM_COMMON_MEM_U8_PTR_TYPE header = prefix->message_hdr;

		CM_PUT32_HTON(header, 0, message_body_size);
		CM_PUT16_HTON(header, 4, message_numb);
		CM_PUT_UUID_HTON(header, 6, ar_uuid);

		/***/

		prefix->session_key  = session_key;
		prefix->alarm_prio   = alarm_prio;
		prefix->message_numb = message_numb;
		prefix->message_size = message_body_size;

		CmListInsertTail(&dev->r1b.send.queue, &prefix->link);

		if( cm_sv_r1b_send_next(dev) ) {

			CM_FATAL(); /* a bug */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POST MESSAGE ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post1( /* usage: device context */
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 message_numb,
	LSA_UINT32 message_body_size, /* without header offset */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	cm_sv_r1b_send(dev, &cm_uuid_nil_uuid, 0, 0, message_numb, message_body_size, prefix);
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post2( /* usage: AR context */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 message_numb,
	LSA_UINT32 message_body_size, /* without header offset */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	cm_sv_r1b_send(argr->cm.sv.dev, &argr->ar.ar_uuid, argr->session_key, 0, message_numb, message_body_size, prefix);
}

/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post3( /* usage: AR/ALMI context */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 alarm_prio,
	LSA_UINT16 message_numb,
	LSA_UINT32 message_body_size, /* without header offset */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	cm_sv_r1b_send(argr->cm.sv.dev, &argr->ar.ar_uuid, argr->session_key, alarm_prio, message_numb, message_body_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE POST CONNECT-DONE ERROR =====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_connect_done_error( /* usage: representative-AR could not be created */
	CM_SV_DEVICE_PTR_TYPE dev,
	CLRPC_UUID_CONST_PTR_TYPE ar_uuid,
	LSA_UINT32 pnio_err
) {
	if( cm_is_null(dev->r1b.proxy_connect_done) ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = dev->r1b.proxy_connect_done;
		CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);

		dev->r1b.proxy_connect_done = LSA_NULL;

		CM_PUT32_HTON(message_body, 0, pnio_err);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_CONNECT_DONE == 4U);

		/***/

		cm_sv_r1b_send(dev, ar_uuid, 0, 0, CM_SV_R1BRIDGE_MSG_CONNECT_DONE, CM_SV_R1BMSG_SIZE_CONNECT_DONE, prefix);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED CONNECT-DONE ERROR ===================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_connect_done_error(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {

	if( cm_is_not_null(dev->r1b.proxy_connect_done) ) {

		CM_FATAL();
	}
	else {

		dev->r1b.proxy_connect_done = prefix;

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/
#else
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("***** not compiling server part (CM_CFG_MAX_SERVERS=" CM_STRINGIFY(CM_CFG_MAX_SERVERS) ")")
#endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
