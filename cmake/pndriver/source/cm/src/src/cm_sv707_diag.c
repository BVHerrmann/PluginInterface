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
/*  F i l e               &F: cm_sv707_diag.c                           :F&  */
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
/*  - diagnoses                                                              */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7707
#define CM_MODULE_ID       7707

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== R1-BRIDGE DIAG POST ELEMS =============================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_diag_post_elems(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( cm_is_null(dev->r1b.diag.current) ) {

		CM_FATAL();
	}
	else {

		CM_UPPER_SV_DIAG_ADD_PTR_TYPE param = dev->r1b.diag.current->args.sv.diag_add;

		if( dev->r1b.diag.curr_elem_idx >= param->nr_of_elems ) {

			CM_FATAL();
		}
		else {

			LSA_BOOL is_add = CM_RQB_GET_OPCODE(dev->r1b.diag.current) == CM_OPC_SV_DIAG_ADD2 ? LSA_TRUE : LSA_FALSE;

			LSA_UINT32 message_max;
			CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(dev->r1b.diag.msg_user_diag, &message_max);
			LSA_UINT32 offset = CM_SV_R1BMSG_SIZE_DIAG_CHANGED;

			CM_ASSERT(CM_SV_R1BMSG_SIZE_DIAG_CHANGED == 0U); /* no header, body: diagnoses */

			if( message_max < (14 + 4 + CM_ALARM_PAYLOAD_MAX_woUSI) ) { //R1 VV: make defines for DIAG-MSG and replace the numbers.
				CM_FATAL(); /* a bug */
			}

			for( ; dev->r1b.diag.curr_elem_idx < param->nr_of_elems; ++dev->r1b.diag.curr_elem_idx ) {

				CM_UPPER_SV_DIAG_ADD_ELEMENT_PTR_TYPE elem = &param->elem[dev->r1b.diag.curr_elem_idx];

				if( elem->response == CM_OK || elem->response == CM_OK_REPLACED ) {

					if( !cm_sv_r1b_marshal_diag(elem, message_body, message_max, &offset, is_add) ) {

						CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "dev(%u) posting diags in chunks: up to elem(%u/%u)"
							, dev->device_nr, dev->r1b.diag.curr_elem_idx - 1, param->nr_of_elems - 1
							);

						if( offset == CM_SV_R1BMSG_SIZE_DIAG_CHANGED ) {
							CM_FATAL(); /* a bug (buffer too small for 1 diagnosis) */
						}

						break; /* note: at least 1 elem was marshalled */
					}
				}
			}

			if( offset > message_max ) {
				CM_FATAL(); /* a bug */
			}

			/* note: empty message if all elem[] had an error */

			cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_DIAG_CHANGED, offset, dev->r1b.diag.msg_user_diag);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE DIAG POST ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_diag_post(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CmListInsertTail(&dev->r1b.diag.queue, (CM_LIST_ENTRY_PTR_TYPE)rb);
	cm_sv_r1b_diag_post_next(dev);
}

/*===========================================================================*/
/*=== R1-BRIDGE DIAG POST NEXT ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_diag_post_next(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	if( cm_is_not_null(dev->r1b.diag.current) ) {

		/* wait for "posted" */
	}
	else if( CmListIsEmpty(&dev->r1b.diag.queue) ) {

		/* idle, nothing to do */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE rb;

		CmListRemoveHead(&dev->r1b.diag.queue, rb, CM_UPPER_RQB_PTR_TYPE);

		dev->r1b.diag.current = rb;

		if(dev->r1b.diag.curr_elem_idx != 0) {
			CM_FATAL();
		}

		cm_sv_r1b_diag_post_elems(dev);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED DIAG CHANGED =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_diag_changed(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if( prefix != dev->r1b.diag.msg_user_diag ) {

		CM_FATAL();
	}
	else {

		if( cm_is_not_null(dev->r1b.diag.current) ) {

			CM_UPPER_RQB_PTR_TYPE rb = dev->r1b.diag.current;

			if( dev->r1b.diag.curr_elem_idx >= rb->args.sv.diag_add->nr_of_elems ) {

				dev->r1b.diag.curr_elem_idx = 0;

				dev->r1b.diag.current = LSA_NULL;

				cm_callback(dev->channel, CM_RQB_GET_RESPONSE(rb), &rb);

				cm_sv_r1b_diag_post_next(dev);
			}
			else {

				cm_sv_r1b_diag_post_elems(dev);
			}
		}
		else {

			/* see cm_sv_r1b_cancel_diag() */
		}

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE DIAG CHANGED =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_diag_changed(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
) {
	CM_ASSERT(CM_SV_R1BMSG_SIZE_DIAG_CHANGED == 0U); /* no header, body: diagnoses */

	/* note: message may be empty */

	cm_sv_r1b_parse_and_change_diag(dev, message_size, message_body);

	cm_callback(dev->channel, CM_OK, &rb);
}

/*===========================================================================*/
/*=== R1-BRIDGE CANCEL DIAG =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_cancel_diag( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
) {

	if( cm_is_not_null(dev->r1b.diag.current) ) { /* cancel "current" */

		CM_UPPER_RQB_PTR_TYPE current = dev->r1b.diag.current;
		dev->r1b.diag.current = LSA_NULL;

		cm_callback(dev->channel, CM_RQB_GET_RESPONSE(current), &current);
	}

	dev->r1b.diag.curr_elem_idx = 0;
	dev->r1b.diag.update_pending = LSA_FALSE;

	while( !CmListIsEmpty(&dev->r1b.diag.queue) ) { /* cancel "queue" */

		CM_UPPER_RQB_PTR_TYPE rb;

		CmListRemoveHead(&dev->r1b.diag.queue, rb, CM_UPPER_RQB_PTR_TYPE);

		cm_callback(dev->channel, CM_RQB_GET_RESPONSE(rb), &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE DIAG IS IDLE ================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_diag_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {

	if( cm_is_not_null(dev->r1b.diag.current) || !CmListIsEmpty(&dev->r1b.diag.queue) ) {

		return LSA_FALSE;
	}
	else if( dev->r1b.diag.update_pending ) {

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== R1-BRIDGE MARSHAL DIAG ================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_marshal_diag(
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 size,
	LSA_UINT32 * offset,
	LSA_BOOL is_add
) {
	LSA_UINT32 temp_offset = *offset; /* update only when return LSA_TRUE */

	if( temp_offset + 14 > size ) {
		return LSA_FALSE;
	}

	CM_PUT32_HTON(message_body, temp_offset +  0, elem->api);
	CM_PUT16_HTON(message_body, temp_offset +  4, elem->slot_nr);
	CM_PUT16_HTON(message_body, temp_offset +  6, elem->subslot_nr);

	if( is_add ) {

		CM_PUT16_HTON(message_body, temp_offset + 8, 1 /* add */);
	}
	else { /* remove */

		if( elem->diag_tag != 0  ) {

			CM_PUT16_HTON(message_body, temp_offset + 8, 2 /* remove */);
		}
		else {

			CM_PUT16_HTON(message_body, temp_offset + 8, 3 /* remove all */);
			temp_offset += 10;

			*offset = temp_offset;

			return LSA_TRUE;
		}
	}

	CM_PUT16_HTON(message_body, temp_offset + 10, elem->channel_number);
	CM_PUT16_HTON(message_body, temp_offset + 12, elem->channel_properties);

	temp_offset += 14;

	switch( elem->diag_type ) { /* note: "diag_type" is private, must use normative constants for bridge messages */

	case CM_SV_DIAG_TYPE_CHANNEL:
		if( temp_offset + 4 > size ) {
			return LSA_FALSE;
		}

		CM_PUT16_HTON(message_body, temp_offset + 0, 0x8000); /* ChannelDiagnosis */
		CM_PUT16_HTON(message_body, temp_offset + 2, elem->u.ch.channel_error_type);

		temp_offset += 4;
		break;

	case CM_SV_DIAG_TYPE_EXT_CHANNEL:
		if( temp_offset + 10 > size ) {
			return LSA_FALSE;
		}

		CM_PUT16_HTON(message_body, temp_offset + 0, 0x8002); /* ExtChannelDiagnosis */
		CM_PUT16_HTON(message_body, temp_offset + 2, elem->u.ext.channel_error_type);
		CM_PUT16_HTON(message_body, temp_offset + 4, elem->u.ext.ext_channel_error_type);
		CM_PUT32_HTON(message_body, temp_offset + 6, elem->u.ext.ext_channel_add_value);

		temp_offset += 10;
		break;

	case CM_SV_DIAG_TYPE_QUALIFIED:
		if( temp_offset + 14 > size ) {
			return LSA_FALSE;
		}

		CM_PUT16_HTON(message_body, temp_offset + 0, 0x8003); /* QualifiedChannelDiagnosis */
		CM_PUT16_HTON(message_body, temp_offset + 2, elem->u.qua.channel_error_type);
		CM_PUT16_HTON(message_body, temp_offset + 4, elem->u.qua.ext_channel_error_type);
		CM_PUT32_HTON(message_body, temp_offset + 6, elem->u.qua.ext_channel_add_value);
		CM_PUT32_HTON(message_body, temp_offset + 10, elem->u.qua.qualified_channel_qualifier);

		temp_offset += 14;
		break;

	case CM_SV_DIAG_TYPE_GENERIC:
		if( temp_offset + 4 + elem->u.gen.info_length > size || elem->u.gen.info_tag > 0x7FFF ) {
			return LSA_FALSE;
		}

		CM_PUT16_HTON(message_body, temp_offset + 0, elem->u.gen.info_tag); /* ManufacturerSpecificDiagnosis */
		CM_PUT16_HTON(message_body, temp_offset + 2, elem->u.gen.info_length);

		CM_MEMCPY(message_body + temp_offset + 4, elem->u.gen.info_data, elem->u.gen.info_length);

		temp_offset += 4 + elem->u.gen.info_length;
		break;

	default:
		CM_FATAL(); /* unreachable */
		break;
	}

	*offset = temp_offset;

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== R1-BRIDGE PARSE AND CHANGE DIAG =======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_parse_and_change_diag(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 message_size,
	CM_COMMON_MEM_U8_PTR_TYPE message_body
) {
	LSA_UINT32 offset = 0;

	while( offset < message_size ) {

		CM_SV_DIAG_ADD_ELEMENT_TYPE diag;
		LSA_UINT16 diag_code;
		LSA_BOOL is_add;
		LSA_UINT16 usi;
		LSA_UINT16 rsp;

		/***/

		if(offset + 10 > message_size) {
			CM_FATAL(); /* R1 protocol error */
		}

		diag.api        = CM_GET32_NTOH(message_body, offset + 0);
		diag.slot_nr    = CM_GET16_NTOH(message_body, offset + 4);
		diag.subslot_nr = CM_GET16_NTOH(message_body, offset + 6);
		diag_code       = CM_GET16_NTOH(message_body, offset + 8);

		offset += 10;

		switch( diag_code ) { /* see cm_sv_r1b_marshal_diag() */
		case 1:
			is_add = LSA_TRUE;
			diag.diag_tag = 1;
			break;
		case 2:
			is_add = LSA_FALSE;
			diag.diag_tag = 1;
			break;
		case 3:
			is_add = LSA_FALSE;
			diag.diag_tag = 0; /* remove all */
			break;
		default:
			CM_FATAL(); /* R1 protocol error */
			/*lint -unreachable*/
			return;
		}

		/***/

		if( diag.diag_tag != 0 ) {

			if( offset + 6 > message_size ) {
				CM_FATAL(); /* R1 protocol error */
			}

			diag.channel_number     = CM_GET16_NTOH(message_body, offset + 0);
			diag.channel_properties = CM_GET16_NTOH(message_body, offset + 2);
			usi                     = CM_GET16_NTOH(message_body, offset + 4);

			offset += 6;

			CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) received diag for ap/sl/su(%u/%u/%u) with USI(0x%04x)"
				, dev->device_nr, diag.api, diag.slot_nr, diag.subslot_nr, usi
				);

			switch( usi ) {

			case 0x8000:
				diag.diag_type = CM_SV_DIAG_TYPE_CHANNEL;

				if(offset + 2 > message_size) {
					CM_FATAL(); /* R1 protocol error */
				}

				diag.u.ch.channel_error_type = CM_GET16_NTOH(message_body, offset + 0);

				offset += 2;
				break;

			case 0x8002:
				diag.diag_type = CM_SV_DIAG_TYPE_EXT_CHANNEL;

				if( offset + 8 > message_size ) {
					CM_FATAL(); /* R1 protocol error */
				}

				diag.u.ext.channel_error_type     = CM_GET16_NTOH(message_body, offset + 0);
				diag.u.ext.ext_channel_error_type = CM_GET16_NTOH(message_body, offset + 2);
				diag.u.ext.ext_channel_add_value  = CM_GET32_NTOH(message_body, offset + 4);

				offset += 8;
				break;

			case 0x8003:
				diag.diag_type = CM_SV_DIAG_TYPE_QUALIFIED;

				if( offset + 12 > message_size ) {
					CM_FATAL(); /* R1 protocol error */
				}

				diag.u.qua.channel_error_type          = CM_GET16_NTOH(message_body, offset + 0);
				diag.u.qua.ext_channel_error_type      = CM_GET16_NTOH(message_body, offset + 2);
				diag.u.qua.ext_channel_add_value       = CM_GET32_NTOH(message_body, offset + 4);
				diag.u.qua.qualified_channel_qualifier = CM_GET32_NTOH(message_body, offset + 8);

				offset += 12;
				break;

			default:
				diag.diag_type = CM_SV_DIAG_TYPE_GENERIC;

				if( offset + 2 > message_size ) {
					CM_FATAL(); /* R1 protocol error */
				}

				diag.u.gen.info_tag    = usi;
				diag.u.gen.info_length = CM_GET16_NTOH(message_body, offset + 0);

				offset += 2;

				if( offset + diag.u.gen.info_length > message_size ) {
					CM_FATAL(); /* R1 protocol error */
				}

				diag.u.gen.info_data = message_body + offset;

				offset += diag.u.gen.info_length;
				break;
			}
		}
		else {

			diag.diag_type = CM_SV_DIAG_TYPE_MAX; /* as documented */
		}

		rsp = cm_sv_diag_change_elem(dev, &diag, is_add, LSA_FALSE /*is_user*/);

		if( ! (rsp == CM_OK || rsp == CM_OK_REPLACED) ) {

			if( rsp == CM_ERR_SEQUENCE ) {

				/* ok, pulled in the meantime */
			}
			else {

				CM_FATAL(); /* e.g. no-mem */
			}
		}
	}

	if( offset != message_size ) {
		CM_FATAL(); /* R1 protocol error */
	}

	/* note: message may be empty */

	cm_sv_dev_submodule_or_diag_changed_trigger(dev); /* R1 diag-changed */
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
