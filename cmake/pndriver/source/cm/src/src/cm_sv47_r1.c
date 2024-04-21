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
/*  F i l e               &F: cm_sv47_r1.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, AR Alarm things                                               */
/*                                                                           */
/*  - R1                                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7472
#define CM_MODULE_ID       7472

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== R1-BRIDGE POST ALARM BEGIN ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_alarm_begin(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_SV_AR_ALMI_PTR_TYPE almi //R1 VV: pass in opcode + param, set flag in caller (make it a pattern)
) {
	CM_UPPER_RQB_PTR_TYPE curr = almi->curr;
	CM_UPPER_ALARM_PTR_TYPE param = curr->args.sv.ar_alarm_send;
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = almi->r1b.msg_alarm_begin_end;
	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max);

	almi->flags |= CM_SV_AR_ALMI_WF_R1_ALARM_BEGIN_POSTED;

	if( CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

		LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_ALARM_BEGIN_PLUG_GROUP;

		CM_PUT16_HTON(message_body, 0, prio);
		CM_PUT32_HTON(message_body, 2, param->api);
		CM_PUT16_HTON(message_body, 6, param->slot_nr);
		CM_PUT16_HTON(message_body, 8, param->subslot_nr);
		CM_PUT16_HTON(message_body, 10, param->alarm_type);
		CM_PUT16_HTON(message_body, 12, cm_is_null(param->u.sv.void_ptr) ? 1 : 0);	/* is pull module */
		CM_PUT16_HTON(message_body, 14, param->alarm_sequence);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_BEGIN_PLUG_GROUP == 16U);

		if( param->alarm_data_length != 0 ) {
			CM_FATAL(); /* a bug */
		}

		if( message_size > message_max ) {
			CM_FATAL();
		}

		cm_sv_r1b_post3(argr, prio, CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PLUG_GROUP, message_size, prefix);
	}
	else {

		LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_ALARM_BEGIN;

		CM_PUT16_HTON(message_body, 0, prio);
		CM_PUT32_HTON(message_body, 2, param->api);
		CM_PUT16_HTON(message_body, 6, param->slot_nr);
		CM_PUT16_HTON(message_body, 8, param->subslot_nr);
		CM_PUT16_HTON(message_body, 10, param->alarm_type);
		CM_PUT16_HTON(message_body, 12, param->alarm_sequence);
		CM_PUT16_HTON(message_body, 14, param->alarm_tag);
		CM_PUT16_HTON(message_body, 16, param->alarm_data_length);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_BEGIN == 18U);

		if( CM_SV_R1BMSG_SIZE_ALARM_BEGIN + param->alarm_data_length > message_max ) {
			CM_FATAL(); /* a bug */
		}

		CM_MEMCPY(message_body + CM_SV_R1BMSG_SIZE_ALARM_BEGIN, param->alarm_data, param->alarm_data_length);

		message_size += param->alarm_data_length;

		if(
			CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_R1PD_INTERNAL
			|| CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PDEV_GROUP
		) {

			cm_sv_r1b_post3(argr, prio, CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PDEV_GROUP, message_size, prefix);
		}
		else {

			cm_sv_r1b_post3(argr, prio, CM_SV_R1BRIDGE_MSG_ALARM_BEGIN, message_size, prefix);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED ALARM BEGIN ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_alarm_begin(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio, /* range-checked in caller */
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	if(
		prefix != almi->r1b.msg_alarm_begin_end
		|| (almi->flags & CM_SV_AR_ALMI_WF_R1_ALARM_BEGIN_POSTED) == 0
		|| (almi->flags & CM_SV_AR_ALMI_ALARM_REQ) != 0
	) {

		CM_FATAL();
	}
	else {

		almi->flags &= ~CM_SV_AR_ALMI_WF_R1_ALARM_BEGIN_POSTED;

		almi->flags |= CM_SV_AR_ALMI_ALARM_REQ;

		if( cm_ar_com_is_down(argr) ) {

			almi->flags |= CM_SV_AR_ALMI_ALARM_CNF;
			cm_sv_ar_almi_done(argr, prio);

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: posted alam-begin (TIA 2828947) */
		}
		else {

			cm_sv_ar_almi_send_next_continued(argr, almi);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE ALARM BEGIN (USER or PDEV) ===========================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_alarm_begin(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_pdev_alarm,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if(
		cm_is_null(argr)
		|| cm_is_null(argr->cm.sv.arset)
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) == 0
		|| message_size < CM_SV_R1BMSG_SIZE_ALARM_BEGIN
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not REPR or wrong message_size(%u))"
			, dev->device_nr, argr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		LSA_UINT16 prio              = CM_GET16_NTOH(message_body, 0);
		LSA_UINT32 api               = CM_GET32_NTOH(message_body, 2);
		LSA_UINT16 slot_nr           = CM_GET16_NTOH(message_body, 6);
		LSA_UINT16 subslot_nr        = CM_GET16_NTOH(message_body, 8);
		LSA_UINT16 alarm_type        = CM_GET16_NTOH(message_body, 10);
		LSA_UINT16 alarm_sequence    = CM_GET16_NTOH(message_body, 12);
		LSA_UINT16 alarm_tag         = CM_GET16_NTOH(message_body, 14);
		LSA_UINT16 alarm_data_length = CM_GET16_NTOH(message_body, 16);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_BEGIN == 18U);

		if(
			prio > 1
			|| prio != cm_sv_ar_almi_alarm_prio(argr, alarm_type)
			|| alarm_sequence > 0x07FF
			|| CM_ALARM_IS_PLUG_GROUP(alarm_type)
			|| CM_SV_R1BMSG_SIZE_ALARM_BEGIN + alarm_data_length != message_size
		) {

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;
			CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

			CM_SV_TRACE_09(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) prio(%u) alarm_sequence(%u) ap/sl/su(%u/%u/%u) alarm_type(%u) alarm_data_length(%u)"
				, argr->device_nr, argr->ar_nr
				, prio, alarm_sequence, api, slot_nr, subslot_nr, alarm_type, alarm_data_length
				);

			if( alarm_curr->is_used ) {

				CM_SV_TRACE_09(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev/ar(%u/%u) replacing alarm_curr: alarm_sequence(%u) ap/sl/su(%u/%u/%u) alarm_type(%u) is_pdev_alarm(%u) is_pull_mod(%u)"
					, argr->device_nr, argr->ar_nr
					, alarm_curr->alarm_sequence, alarm_curr->api, alarm_curr->slot_nr, alarm_curr->subslot_nr, alarm_curr->alarm_type, alarm_curr->is_pdev_alarm, alarm_curr->is_pull_module
				);
			}

			alarm_curr->is_used        = LSA_TRUE;
			alarm_curr->is_pdev_alarm  = LSA_FALSE;
			alarm_curr->is_pull_module = LSA_FALSE;

			if( is_pdev_alarm ) {

				alarm_curr->is_pdev_alarm = LSA_TRUE;

				if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr) ) {

					if( cm_is_null(arset->r1b.pd_alarm.orig) ) {
						CM_FATAL();
					}
				}
				else {

					if(
						cm_is_null(arset->r1b.pd_alarm.mirr) /* pre-allocated */
						|| CM_RQB_GET_OPCODE(arset->r1b.pd_alarm.mirr) == 0/*unused*/
					) {
						CM_FATAL(); /* must exist, see cm_sv_r1b_handle_pdev_alarm_mirror_set() */
					}
				}
			}

			alarm_curr->api               = api;
			alarm_curr->slot_nr           = slot_nr;
			alarm_curr->subslot_nr        = subslot_nr;
			alarm_curr->alarm_type        = alarm_type;
			alarm_curr->alarm_sequence    = alarm_sequence;
			alarm_curr->alarm_tag         = alarm_tag;
			alarm_curr->alarm_data_length = alarm_data_length;

			if( alarm_curr->alarm_data_length != 0 ) {

				CM_MEMCPY(alarm_curr->alarm_data, message_body + 18, alarm_curr->alarm_data_length);
			}

			arset->alarm[prio].sequence_number = (LSA_UINT16)((alarm_sequence + 1) & 0x07FF);
		}

		/***/

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE ALARM BEGIN PLUG-GROUP ===============================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_alarm_begin_plug_group(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
){
	if(
		cm_is_null(argr)
		|| cm_is_null(argr->cm.sv.arset)
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) == 0
		|| message_size != CM_SV_R1BMSG_SIZE_ALARM_BEGIN_PLUG_GROUP
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not REPR or wrong message_size(%u))"
			, dev->device_nr, argr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		LSA_UINT16 prio           = CM_GET16_NTOH(message_body,  0);
		LSA_UINT32 api            = CM_GET32_NTOH(message_body,  2);
		LSA_UINT16 slot_nr        = CM_GET16_NTOH(message_body,  6);
		LSA_UINT16 subslot_nr     = CM_GET16_NTOH(message_body,  8);
		LSA_UINT16 alarm_type     = CM_GET16_NTOH(message_body, 10);
		LSA_BOOL   is_pull_mod    = CM_GET16_NTOH(message_body, 12) != 0 ? LSA_TRUE : LSA_FALSE;
		LSA_UINT16 alarm_sequence = CM_GET16_NTOH(message_body, 14);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_BEGIN_PLUG_GROUP == 16U);

		if(
			prio > 1
			|| prio != cm_sv_ar_almi_alarm_prio(argr, alarm_type)
			|| alarm_sequence > 0x07FF
			|| ! CM_ALARM_IS_PLUG_GROUP(alarm_type)
		) {

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;
			CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

			CM_SV_TRACE_09(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) prio(%u) alarm_sequence(%u) ap/sl/su(%u/%u/%u) alarm_type(%u) is_pull_mod(%u)"
				, argr->device_nr, argr->ar_nr
				, prio, alarm_sequence, api, slot_nr, subslot_nr, alarm_type, is_pull_mod
				);

			if( alarm_curr->is_used ) {

				CM_SV_TRACE_09(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev/ar(%u/%u) replacing alarm_curr: alarm_sequence(%u) ap/sl/su(%u/%u/%u) alarm_type(%u) is_pdev_alarm(%u) is_pull_mod(%u)"
					, argr->device_nr, argr->ar_nr
					, alarm_curr->alarm_sequence, alarm_curr->api, alarm_curr->slot_nr, alarm_curr->subslot_nr, alarm_curr->alarm_type, alarm_curr->is_pdev_alarm, alarm_curr->is_pull_module
					);
			}

			alarm_curr->is_used        = LSA_TRUE;
			alarm_curr->is_pdev_alarm  = LSA_FALSE;
			alarm_curr->is_pull_module = LSA_FALSE;

			alarm_curr->api               = api;
			alarm_curr->slot_nr           = slot_nr;
			alarm_curr->subslot_nr        = subslot_nr;
			alarm_curr->alarm_type        = alarm_type;
			alarm_curr->alarm_sequence    = alarm_sequence;
			alarm_curr->alarm_tag         = 0;
			alarm_curr->alarm_data_length = 0;

			arset->alarm[prio].sequence_number = (LSA_UINT16)((alarm_sequence + 1) & 0x07FF);

			if( is_pull_mod ) {

				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

				cm_ar_graph_find_api_slot_subslot(argr, api, slot_nr, 0, LSA_NULL, &ar_mod, LSA_NULL);

				if( cm_is_null(ar_mod) ) {

					CM_FATAL(); /* R1 protocol error */
				}
				else {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

					for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_DIRTY;
						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_R1_PP;
						ar_sub->sv.st_told = CM_SUB_STATE_PULLED;

						if( alarm_type == CM_ALARM_TYPE_RELEASED ) {

							CM_SUB_STATE_SET_AR_INFO(ar_sub->sv.st_told, CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED); /* restore to "not owned" */
						}
					}
				}

				alarm_curr->is_pull_module = LSA_TRUE;
			}
			else {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub = LSA_NULL;

				cm_ar_graph_find_api_slot_subslot(argr, api, slot_nr, subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

				if( cm_is_null(ar_sub) ) {

					CM_FATAL(); /* R1 protocol error */
				}
				else {

					ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_DIRTY;
					ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_R1_PP;
					ar_sub->sv.st_told = CM_SUB_STATE_PULLED;

					if( alarm_type == CM_ALARM_TYPE_RELEASED ) {

						CM_SUB_STATE_SET_AR_INFO(ar_sub->sv.st_told, CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED); /* restore to "not owned" */
					}
				}
			}
		}

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POST ALARM END ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_alarm_end(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 prio,
	CM_SV_AR_ALMI_PTR_TYPE almi //R1 VV: pass in opcode + param, set flag in caller (make it a pattern)
) {
	CM_UPPER_RQB_PTR_TYPE curr = almi->curr;
	CM_UPPER_ALARM_PTR_TYPE param = curr->args.sv.ar_alarm_send;
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = almi->r1b.msg_alarm_begin_end;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);

	almi->flags |= CM_SV_AR_ALMI_WF_R1_ALARM_END_POSTED;

	if( CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PLUG_GROUP ) {

		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub = (CM_AR_GRAPH_SUBMODULE_PTR_TYPE)param->u.sv.void_ptr;

		LSA_UINT16 st_told;
		LSA_UINT16 st_user;

		if( cm_is_not_null(ar_sub) ) {

			st_told = ar_sub->sv.st_told;
			st_user = cm_sv_get_submodule_state(LSA_NULL, argr, ar_sub->mod->ap, ar_sub->mod, ar_sub);
		}
		else {

			st_told = CM_SUB_STATE_PULLED;
			st_user = CM_SUB_STATE_PULLED;
		}

		CM_PUT16_HTON(message_body, 0, prio);
		CM_PUT16_HTON(message_body, 2, param->alarm_sequence);
		CM_PUT16_HTON(message_body, 4, param->alarm_type);
		CM_PUT16_HTON(message_body, 6, st_told);
		CM_PUT16_HTON(message_body, 8, st_user);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_END_PLUG_GROUP == 10U);

		cm_sv_r1b_post3(argr, prio, CM_SV_R1BRIDGE_MSG_ALARM_END_PLUG_GROUP, CM_SV_R1BMSG_SIZE_ALARM_END_PLUG_GROUP, prefix);
	}
	else {

		CM_PUT16_HTON(message_body, 0, prio);
		CM_PUT16_HTON(message_body, 2, param->alarm_sequence);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_END == 4U)

		if(
			CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_PDEV_GROUP
			|| CM_RQB_GET_OPCODE(curr) == CM_OPC_SV_AR_ALARM_R1PD_INTERNAL
		) {

			cm_sv_r1b_post3(argr, prio, CM_SV_R1BRIDGE_MSG_ALARM_END_PDEV_GROUP, CM_SV_R1BMSG_SIZE_ALARM_END, prefix);
		}
		else {

			cm_sv_r1b_post3(argr, prio, CM_SV_R1BRIDGE_MSG_ALARM_END, CM_SV_R1BMSG_SIZE_ALARM_END, prefix);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED ALARM END ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_alarm_end(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 prio, /* range-checked in caller */
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	CM_SV_AR_ALMI_PTR_TYPE almi = &argr->cm.sv.almi[prio];

	if(
		prefix != almi->r1b.msg_alarm_begin_end
		|| (almi->flags & CM_SV_AR_ALMI_WF_R1_ALARM_END_POSTED) == 0
	) {

		CM_FATAL();
	}
	else {

		almi->flags &= ~CM_SV_AR_ALMI_WF_R1_ALARM_END_POSTED;

		cm_sv_ar_almi_done(argr, prio);

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: posted alam-end (TIA 2828947) */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE ALARM END (USER or PDEV) =============================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_alarm_end(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_pdev_alarm,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
){
	if(
		cm_is_null(argr)
		|| cm_is_null(argr->cm.sv.arset)
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) == 0
		|| message_size != CM_SV_R1BMSG_SIZE_ALARM_END
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not REPR or wrong message_size(%u))"
			, dev->device_nr, argr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		LSA_UINT16 prio           = CM_GET16_NTOH(message_body, 0);
		LSA_UINT16 alarm_sequence = CM_GET16_NTOH(message_body, 2);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_END == 4U);

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev/ar(%u/%u) prio(%u) alarm_sequence(%u)"
			, argr->device_nr, argr->ar_nr
			, prio, alarm_sequence
			);

		if(
			prio > 1
			|| alarm_sequence > 0x07FF
		) {
			CM_FATAL(); /* R1 protocol error */
		}
		else {

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;
			CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

			cm_sv_arset_check_primary(argr);

			if( cm_is_not_null(arset->primary) ) { /* message is late */

				/* do nothing */
			}
			else if( ! alarm_curr->is_used ) {

				CM_FATAL(); /* a bug */
			}
			else {

				if( alarm_curr->alarm_sequence != alarm_sequence ) {

					CM_FATAL(); /* R1 protocol error */
				}

				if( is_pdev_alarm ) {

					CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

					if( !alarm_curr->is_pdev_alarm ) {
						CM_FATAL();
					}

					if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, alarm_curr->subslot_nr) ) {

						if( cm_is_null(r1pd_alarm->orig) ) {
							CM_FATAL();
						}
						else {

							CM_UPPER_RQB_PTR_TYPE upper = r1pd_alarm->orig;

							CM_RQB_SET_RESPONSE(upper, CM_OK);

							r1pd_alarm->orig_is_completed = LSA_TRUE;

							cm_sv_r1b_pdev_alarm_mirror_clr(argr->cm.sv.dev); /* see cm_sv_r1b_posted_pdev_alarm_mirror_clr() */
						}
					}
					else { /* remote PDEV */

						if(
							cm_is_null(r1pd_alarm->mirr) /* pre-allocated */
							|| CM_RQB_GET_OPCODE(r1pd_alarm->mirr) == 0/*unused*/
						) {

							CM_FATAL(); /* must exist, see cm_sv_r1b_handle_pdev_alarm_mirror_set() */
						}
						else {

							r1pd_alarm->mirr_is_completed = LSA_TRUE;
						}
					}
				}

				alarm_curr->is_used = LSA_FALSE;
			}
		}

		/***/

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE ALARM END PLUG-GROUP =================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_alarm_end_plug_group(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
){
	if(
		cm_is_null(argr)
		|| cm_is_null(argr->cm.sv.arset)
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) == 0
		|| message_size != CM_SV_R1BMSG_SIZE_ALARM_END_PLUG_GROUP
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not REPR or wrong message_size(%u))"
			, dev->device_nr, argr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		LSA_UINT16 prio           = CM_GET16_NTOH(message_body, 0);
		LSA_UINT16 alarm_sequence = CM_GET16_NTOH(message_body, 2);
		LSA_UINT16 alarm_type     = CM_GET16_NTOH(message_body, 4);
		LSA_UINT16 st_told        = CM_GET16_NTOH(message_body, 6);
		LSA_UINT16 st_user        = CM_GET16_NTOH(message_body, 8);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_ALARM_END_PLUG_GROUP == 10U);

		if(
			prio > 1
			|| prio != cm_sv_ar_almi_alarm_prio(argr, alarm_type)
			|| alarm_sequence > 0x07FF
		) {

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

			CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

			CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) prio(%u) alarm_sequence(%u) alarm_type(%u) st_told(0x%04x) st_user(0x%04x)"
				, argr->device_nr, argr->ar_nr
				, prio, alarm_sequence, alarm_type, st_told, st_user
				);

			if(
				!alarm_curr->is_used
				|| alarm_curr->alarm_sequence != alarm_sequence
				|| alarm_curr->alarm_type != alarm_type
			) {

				CM_FATAL(); /* R1 protocol error */
			}
			else {

				if( (alarm_curr->is_pull_module) ) {

					CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
					cm_ar_graph_find_api_slot_subslot(argr, alarm_curr->api, alarm_curr->slot_nr, 0, LSA_NULL, &ar_mod, LSA_NULL);

					if( cm_is_null(ar_mod) ) {
						CM_FATAL(); /* R1 protocol error */
					}
					else {

						CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

						for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

							if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_R1_PP) == 0 ) {

								/* late */
							}
							else {

								ar_sub->sv.st_told = st_told;

								ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_R1_PP;

								if( ! CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su) ) {

									ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;
									ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_WRONG;
									ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_PRMED;
									ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_ARP;
								}
							}
						}
					}
				}
				else {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;
					cm_ar_graph_find_api_slot_subslot(argr, alarm_curr->api, alarm_curr->slot_nr, alarm_curr->subslot_nr, LSA_NULL, LSA_NULL,  &ar_sub);

					if( cm_is_null(ar_sub) ) {
						CM_FATAL(); /* R1 protocol error */
					}
					else {

						if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_R1_PP) == 0 ) {

							/* late */
						}
						else {

							ar_sub->sv.st_told = st_told;

							ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_R1_PP;
							ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;

							ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_WRONG;
							ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_PRMED;
							ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_IS_ARP;

							if( CM_SUB_STATE_IS_OWNED(ar_sub->sv.st_told) ) {
								if( ar_sub->sv.real_su->owner_ar_nr == 0 && ar_sub->sv.real_su->cand[0].ar_nr == argr->ar_nr ) {
									ar_sub->sv.real_su->owner_ar_nr = argr->ar_nr;
								}
							}
							else {
								if( ar_sub->sv.real_su->owner_ar_nr == argr->ar_nr ) {
									ar_sub->sv.real_su->owner_ar_nr = 0;
								}
							}

							if( CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su) ) {

								if( CM_SUB_STATE_IS_WRONG(st_user) ) {

									ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_WRONG;
								}
								else if( CM_SUB_STATE_IS_OK(st_user) || CM_SUB_STATE_IS_SUBSTITUTE(st_user) ) {

									if( CM_SUB_STATE_IS_OWNED(st_user) ) {

										ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_PRMED;

										if( CM_SUB_STATE_GET_AR_INFO(st_user) == CM_SUB_STATE_AR_INFO_APPL_READY_PENDING ) {

											ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_ARP;
										}
									}
								}
								else {

									/* do nothing */
								}
							}
						}
					}
				}

				alarm_curr->is_used = LSA_FALSE;
			}
		}
	}

	cm_callback(dev->channel, CM_OK, &rb);

	cm_sv_dev_led_info_eval(dev); /* trigger: handle alarm-end(plug-group) */
}

/*===========================================================================*/
/*=== R1-BRIDGE CURRENT ALARM MATCHES =======================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_curr_alarm_matches(
	CM_SV_ARSET_PTR_TYPE arset,
	CM_UPPER_ALARM_CONST_PTR_TYPE al,
	LSA_UINT16 prio,
	LSA_UINT16 * sequence_number /* is updated if returns TRUE */
) {
	CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &arset->r1b.alarm.curr[prio];

	if( alarm_curr->is_used ) {

		if(
			alarm_curr->api == al->api
			&& alarm_curr->slot_nr == al->slot_nr
			&& alarm_curr->subslot_nr == al->subslot_nr
			&& alarm_curr->alarm_type == al->alarm_type
			&& alarm_curr->alarm_tag == al->alarm_tag
			&& alarm_curr->alarm_data_length == al->alarm_data_length
			&& (alarm_curr->alarm_data_length == 0 || CM_MEMCMP(alarm_curr->alarm_data, al->alarm_data, alarm_curr->alarm_data_length) == 0)
		) {

			alarm_curr->is_used = LSA_FALSE; /* note: will be reset in cm_sv_arset_alarm_restart() if not matched */

			*sequence_number = alarm_curr->alarm_sequence;

			return LSA_TRUE; /* did match */
		}
	}

	return LSA_FALSE; /* did not match */
}

/*===========================================================================*/
/*=== R1-BRIDGE CANCEL ARSET ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_cancel_arset( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	/* remove internal R1PD alarms */

	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev(%u) dequeue R1PD-alarm (mirror) from AR-set, mirror-used(%u)"
		, dev->device_nr
		, (cm_is_not_null(r1pd_alarm->mirr) ? CM_RQB_GET_OPCODE(r1pd_alarm->mirr) : 0)
	);

	{
	LSA_UINT16 prio;

	for( prio = 0; prio < 2; ++prio ) {

		CM_UPPER_RQB_PTR_TYPE curr;

		for( CmListEach(curr, &arset->alarm[prio].queue, CM_UPPER_RQB_PTR_TYPE) ) {

			if( curr == r1pd_alarm->mirr ) {

				CM_UPPER_RQB_PTR_TYPE temp = curr;

				curr = CmListPrevOrHead((CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

				CmListRemoveEntry((CM_LIST_ENTRY_PTR_TYPE)temp);
			}
		}
	}
	}

	/***/

	if( cm_is_not_null(r1pd_alarm->orig) ) {

		/* do nothing */
	}

	if( cm_is_not_null(r1pd_alarm->mirr) ) { /* pre-allocated */

		CM_RQB_SET_OPCODE(r1pd_alarm->mirr, 0/*unused*/); /* "orig" gone */

		r1pd_alarm->mirr_is_completed = LSA_FALSE;
	}

	/* note: the CM_SV_AR_SMFL_R1_PP flag is reset via cm_sv_arset_ownership_swap() */
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
