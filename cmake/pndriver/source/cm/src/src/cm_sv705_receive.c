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
/*  F i l e               &F: cm_sv705_receive.c                        :F&  */
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
/*  - Receive message                                                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7705
#define CM_MODULE_ID       7705

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== R1-BRIDGE RECEIVED ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_r1bridge_received(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_R1BRIDGE_RECEIVED_PTR_TYPE param = rb->args.sv.r1bridge_received;
	CM_SV_DEVICE_PTR_TYPE dev;

	dev = cm_sv_dev_get_added(channel, param->device_nr);

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) is not added"
			, param->device_nr
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else if( ! dev->R1_enabled ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) not R1_enabled"
			, dev->device_nr
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else if( dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_UNKNOWN || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 bridge_state(%u) not established"
			, dev->device_nr, dev->r1b.bridge_state
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else {

		if(
			cm_is_null(param->message_data)
			|| param->message_size < CM_SV_R1BMSG_SIZE_HEADER /* too short for a bridge-message */
		) {

			CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) R1 protocol error (message_data(0x%x) is null or message_size(%u) too short)"
				, dev->device_nr, param->message_data, param->message_size
				);

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			CM_COMMON_MEM_U8_PTR_TYPE header = (CM_COMMON_MEM_U8_PTR_TYPE)param->message_data;
			CM_COMMON_MEM_U8_PTR_TYPE message_body = header + CM_SV_R1BMSG_SIZE_HEADER;

			LSA_UINT32 message_size = CM_GET32_NTOH(header, 0);
			LSA_UINT16 message_numb = CM_GET16_NTOH(header, 4);
			CLRPC_UUID_TYPE ar_uuid;
			CM_GET_UUID_NTOH(&ar_uuid, header, 6);

			if( param->message_size != CM_SV_R1BMSG_SIZE_HEADER + message_size ) {

				CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "dev(%u) R1 protocol error (message_numb(%u), received_size(%u) != header_size(%u) + message_size(%u))"
					, dev->device_nr, message_numb, param->message_size, CM_SV_R1BMSG_SIZE_HEADER, message_size
					);

				CM_FATAL(); /* R1 protocol error */
			}
			else {

				CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup_by_aruuid(dev, &ar_uuid); /* Note: error handling is done in cm_sv_r1b_handle_xxx() */

				CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
					, "dev(%u) r1bridge-received: numb(%u) size(%u)"
					, dev->device_nr, message_numb, message_size
					);

				switch(message_numb) {

				case CM_SV_R1BRIDGE_MSG_CONNECT:
					cm_sv_r1b_handle_connect(dev, argr, rb, message_body, message_size, &ar_uuid, LSA_FALSE);
					break;

				case CM_SV_R1BRIDGE_MSG_CONNECT_DONE:
					cm_sv_r1b_handle_connect_done(dev, argr, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT:
					cm_sv_r1b_handle_sub_state(dev, argr, rb, message_body, message_size, message_numb); /* connect */
					break;

				case CM_SV_R1BRIDGE_MSG_DISCONNECT:
					cm_sv_r1b_handle_disconnect(dev, argr, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_DISCONNECT_DONE:
					cm_sv_r1b_handle_disconnect_done(dev, argr, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_UPDATE_BEGIN:
					cm_sv_r1b_handle_update_begin(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_UPDATE_DIAG:
					cm_sv_r1b_handle_update_diag(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT:
					cm_sv_r1b_handle_connect(dev, argr, rb, message_body, message_size, &ar_uuid, LSA_TRUE);
					break;

				case CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE:
					cm_sv_r1b_handle_sub_state(dev, argr, rb, message_body, message_size, message_numb); /* update */
					break;

				case CM_SV_R1BRIDGE_MSG_UPDATE_ALARMS:
					cm_sv_r1b_handle_update_alarms(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_UPDATE_END:
					cm_sv_r1b_handle_update_end(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_ALARM_BEGIN:
					cm_sv_r1b_handle_alarm_begin(dev, argr, rb, LSA_FALSE, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_ALARM_END:
					cm_sv_r1b_handle_alarm_end(dev, argr, rb, LSA_FALSE, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PLUG_GROUP:
					cm_sv_r1b_handle_alarm_begin_plug_group(dev, argr, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_ALARM_END_PLUG_GROUP:
					cm_sv_r1b_handle_alarm_end_plug_group(dev, argr, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PDEV_GROUP:
					cm_sv_r1b_handle_alarm_begin(dev, argr, rb, LSA_TRUE, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_ALARM_END_PDEV_GROUP:
					cm_sv_r1b_handle_alarm_end(dev, argr, rb, LSA_TRUE, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_DIAG_CHANGED:
					cm_sv_r1b_handle_diag_changed(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_DIAG_REPLACE_PDEV:
					cm_sv_r1b_handle_diag_replace_pdev(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ:
				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE:
				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN:
				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_END:
					cm_sv_r1b_pdev_B_handle_read_write_begin_end(dev, rb);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ_DONE:
					cm_sv_r1b_pdev_A_handle_prm_read_done(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE_DONE:
					cm_sv_r1b_pdev_A_handle_prm_write_done(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN_DONE:
					cm_sv_r1b_pdev_A_handle_prm_begin_done(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_PRM_END_DONE:
					cm_sv_r1b_pdev_A_handle_prm_end_done(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_SET:
					cm_sv_r1b_handle_pdev_alarm_mirror_set(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CLR:
					cm_sv_r1b_handle_pdev_alarm_mirror_clr(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CANCEL:
					cm_sv_r1b_handle_pdev_alarm_mirror_cancel(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS:
					cm_sv_r1b_stat_B_handle(dev, rb);
					break;

				case CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS_DONE:
					cm_sv_r1b_stat_A_handle_done(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_RESET_TO_FACTORY:
					cm_sv_r1b_handle_reset_to_factory(dev, rb, message_body, message_size);
					break;

				case CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END:
					cm_sv_r1b_handle_sub_state(dev, argr, rb, message_body, message_size, message_numb); /* prm-begin-end */
					break;

				default:
					CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
						, "dev(%u) R1 protocol error (unknown message_numb(%u))"
						, dev->device_nr, message_numb
						);
					CM_FATAL(); /* R1 protocol error */
					break;
				}
			}
		}
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
