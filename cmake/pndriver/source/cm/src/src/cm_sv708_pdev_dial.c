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
/*  F i l e               &F: cm_sv708_pdev_dial.c                      :F&  */
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
/*  - PDEV diag and alarms                                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7708
#define CM_MODULE_ID       7708

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_join_done(
	CM_SV_DEVICE_PTR_TYPE dev
); /* forward */

/*===========================================================================*/
/*=== R1-BRIDGE MARSHAL PDEV DIAG ===========================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_marshal_pdev_diag(
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	CM_PD_DIAG_CONST_PTR_TYPE diag,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 size,
	LSA_UINT32 * offset,
	LSA_BOOL is_add
) {

	LSA_UINT32 temp_offset = *offset; /* update only when return LSA_TRUE */

	if( temp_offset + 24 > size ) {
		return LSA_FALSE;
	}

	/***/

	CM_PUT32_HTON(message_body, temp_offset +  0, api);
	CM_PUT16_HTON(message_body, temp_offset +  4, slot_nr);
	CM_PUT16_HTON(message_body, temp_offset +  6, subslot_nr);

	if( is_add ) {

		CM_PUT16_HTON(message_body, temp_offset + 8, 1 /* add */);
	}
	else { /* remove */

		if( diag->diag_tag != 0 ) {

			CM_PUT16_HTON(message_body, temp_offset + 8, 2 /* remove */);
		}
		else {

			CM_PUT16_HTON(message_body, temp_offset + 8, 3 /* remove all */);
			temp_offset += 10;

			*offset = temp_offset;
			return LSA_TRUE;
		}
	}

	CM_PUT16_HTON(message_body, temp_offset + 10, 0x8000);
	CM_PUT16_HTON(message_body, temp_offset + 12, diag->channel_properties);

	CM_PUT16_HTON(message_body, temp_offset + 14, 0x8002); /* ExtChannelDiagnosis */
	CM_PUT16_HTON(message_body, temp_offset + 16, diag->channel_error_type);
	CM_PUT16_HTON(message_body, temp_offset + 18, diag->ext_channel_error_type);
	CM_PUT32_HTON(message_body, temp_offset + 20, diag->ext_channel_add_value);

	temp_offset += 24;

	*offset = temp_offset;
	return LSA_TRUE;
}

/*===========================================================================*/
/*=== R1-BRIDGE DIAG REPLACE PDEV ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_diag_replace_pdev(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( dev->r1b.diag.pdev_diag_posted ) {

		dev->r1b.diag.pdev_diag_again = LSA_TRUE;
	}
	else {

		if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHED ) { /* TIA 1737437 */

			/* do nothing */
		}
		else {

			LSA_UINT32 message_max;
			CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(dev->r1b.diag.msg_pdev_diag, &message_max);
			LSA_UINT32 offset = CM_SV_R1BMSG_SIZE_DIAG_REPLACE_PDEV;

			CM_ASSERT(CM_SV_R1BMSG_SIZE_DIAG_REPLACE_PDEV == 0U); /* no header */

			{
			CM_SV_AP_PTR_TYPE ap = cm_sv_ap_lookup(dev, 0/*!*/);

			if( cm_is_not_null(ap) ) {

				CM_SV_SLOT_PTR_TYPE sl;

				for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

					CM_SV_SUBSLOT_PTR_TYPE su;

					for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

						if(
							CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, su->subslot_nr)
							&& CM_SV_SUBSLOT_IS_PLUGGED(su)
						) {

							CM_LIST_ENTRY_PTR_TYPE diag_list = cm_pdsv_diag_read(dev->channel, sl->slot_nr, su->subslot_nr);

							CM_PD_DIAG_PTR_TYPE diag;

							for( CmListEach(diag, diag_list, CM_PD_DIAG_PTR_TYPE) ) {

								if( !cm_sv_r1b_marshal_pdev_diag(ap->api, sl->slot_nr, su->subslot_nr, diag, message_body, message_max, &offset, LSA_TRUE /* add */) ) {

									CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
										, "dev(%u) R1 bridge message 'PDEV diag replace' must fit into one R1 bridge message buffer!"
										, dev->device_nr
										);

									CM_FATAL();
								}
							}
						}
					}
				}
			}
			}

			if( offset > message_max ) {
				CM_FATAL();
			}

			/***/

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u) sending pdev-diag-replace to other head"
				, dev->device_nr
				);

			/* note: "empty" (i.e. all diagnoses gone) is a normal case; diag-replace = del-all + add-none. */

			dev->r1b.diag.pdev_diag_posted = LSA_TRUE;

			cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_DIAG_REPLACE_PDEV, offset, dev->r1b.diag.msg_pdev_diag);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED DIAG REPLACE PDEV ====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_diag_replace_pdev(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		prefix != dev->r1b.diag.msg_pdev_diag
		|| ! dev->r1b.diag.pdev_diag_posted
	) {

		CM_FATAL();
	}
	else {

		dev->r1b.diag.pdev_diag_posted = LSA_FALSE;

		if( dev->r1b.diag.pdev_diag_again ) {

			dev->r1b.diag.pdev_diag_again = LSA_FALSE;

			cm_sv_r1b_diag_replace_pdev(dev); /* trigger: cm_sv_r1b_posted_diag_replace_pdev() */
		}

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE DIAG REPLACE PDEV ====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_diag_replace_pdev(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
) {
	/* 1st: remove all diagnoses on remote PDEV */
	/* 2nd: add PDEV diagnoses from message */

	CM_SV_AP_PTR_TYPE ap = cm_sv_ap_lookup(dev, 0/*!*/);

	if( cm_is_not_null(ap) ) {

		CM_SV_SLOT_PTR_TYPE sl;

		for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

			CM_SV_SUBSLOT_PTR_TYPE su;

			for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

				if(
					CM_SV_SUBSLOT_IS_PDEV_REMOTE(dev, su->subslot_nr)
					&& CM_SV_SUBSLOT_IS_PLUGGED(su)
				) {

					CM_SV_DIAG_ADD_ELEMENT_TYPE elem;

					elem.api        = ap->api;
					elem.slot_nr    = sl->slot_nr;
					elem.subslot_nr = su->subslot_nr;
					elem.diag_tag   = 0; /* remove all */
					elem.diag_type  = CM_SV_DIAG_TYPE_MAX; /* as documented for "remove all" */

					if( cm_sv_diag_change_elem(dev, &elem, LSA_FALSE /*is_add*/, LSA_FALSE /*is_user*/) != CM_OK ) {

						CM_FATAL();
					}
				}
			}
		}
	}

	/* note: "empty" (i.e. all diagnoses gone) is a normal case; diag-replace = del-all + add-none. */

	CM_ASSERT(CM_SV_R1BMSG_SIZE_DIAG_REPLACE_PDEV == 0U); /* no header, body: diagnoses */

	cm_sv_r1b_parse_and_change_diag(dev, message_size, message_body); //R1 VV: don't trust the peer, check for only-remote-pdev-subslots.

	cm_callback(dev->channel, CM_OK, &rb);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-ALARM MIRROR-SET =======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_set(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_CONST_PTR_TYPE alarm
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if(
		r1pd_alarm->wf_set_posted
		|| alarm != r1pd_alarm->orig
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_UPPER_ALARM_PTR_TYPE param = alarm->args.sv.ar_alarm_send;

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = r1pd_alarm->msg_mirror_set;
		LSA_UINT32 message_max;
		CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max);

		LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET;

		CM_PUT16_HTON(message_body, 0, param->slot_nr);
		CM_PUT16_HTON(message_body, 2, param->subslot_nr);
		CM_PUT16_HTON(message_body, 4, param->alarm_type);
		CM_PUT16_HTON(message_body, 6, param->alarm_tag);
		CM_PUT16_HTON(message_body, 8, param->alarm_data_length);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET == 10U);

		if( param->alarm_data_length + CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET > message_max ) {

			CM_FATAL(); /* a bug */
		}

		CM_MEMCPY(message_body + 10, param->alarm_data, param->alarm_data_length);

		message_size += param->alarm_data_length;

		r1pd_alarm->wf_set_posted = LSA_TRUE; /* see cm_sv_r1b_posted_pdev_alarm_mirror_set() */

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_SET, message_size, prefix);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED PDEV-ALARM MIRROR-SET ================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_pdev_alarm_mirror_set(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if(
		prefix != r1pd_alarm->msg_mirror_set
		|| !r1pd_alarm->wf_set_posted
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		r1pd_alarm->wf_set_posted = LSA_FALSE;

		if( cm_is_null(r1pd_alarm->orig) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			if( r1pd_alarm->orig_is_completed ) {

				cm_sv_r1b_pdev_alarm_mirror_join_done(dev);
			}
		}

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE PDEV-ALARM MIRROR-SET ================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_pdev_alarm_mirror_set(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
) {
	if( message_size < CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		LSA_UINT16 slot_nr           = CM_GET16_NTOH(message_body, 0);
		LSA_UINT16 subslot_nr        = CM_GET16_NTOH(message_body, 2);
		LSA_UINT16 alarm_type        = CM_GET16_NTOH(message_body, 4);
		LSA_UINT16 alarm_tag         = CM_GET16_NTOH(message_body, 6);
		LSA_UINT16 alarm_data_length = CM_GET16_NTOH(message_body, 8);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET == 10U);

		if(
			CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET + alarm_data_length != message_size
			|| alarm_data_length > CM_ALARM_PAYLOAD_MAX_woUSI /* see cm_sv_r1b_alloc() */
		) {

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
			CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

			if( arset->is_aborting || cm_is_null(arset->arr[0]) ) { /* TIA 2253099 */
				/* late, do nothing */
				cm_callback(dev->channel, CM_OK, &rb);
				return;
			}

			if(
				cm_is_null(r1pd_alarm->mirr) /* pre-allocated */
				|| CM_RQB_GET_OPCODE(r1pd_alarm->mirr) != 0/*unused*/
				|| r1pd_alarm->mirr_is_completed /* see cm_sv_r1b_handle_pdev_alarm_mirror_clr() */
			) {

				CM_FATAL(); /* R1 protocol error (message unexpected) */
			}
			else {

				CM_UPPER_RQB_PTR_TYPE alarm_rb = r1pd_alarm->mirr;

				CM_RQB_SET_USER_ID_PTR(alarm_rb, LSA_NULL);
				CM_RQB_SET_HANDLE(alarm_rb, CM_INVALID_HANDLE);
				CM_RQB_SET_OPCODE(alarm_rb, CM_OPC_SV_AR_ALARM_R1PD_INTERNAL); /* used (i.e. contains valid data) */

				{
				CM_UPPER_ALARM_PTR_TYPE alarm = alarm_rb->args.sv.ar_alarm_send;

				alarm->device_nr = dev->device_nr;
				alarm->ar_nr = 0;
				alarm->session_key = 0;

				alarm->alarm_priority    = 0;
				alarm->api               = 0;
				alarm->slot_nr           = slot_nr;
				alarm->subslot_nr        = subslot_nr;
				alarm->alarm_type        = alarm_type;
				alarm->alarm_sequence    = 0;
				alarm->alarm_tag         = alarm_tag;
				alarm->alarm_data_length = alarm_data_length;

				if( alarm_data_length != 0 ) {

					CM_MEMCPY(alarm->alarm_data, message_body + CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET, alarm_data_length);
				}
				else {

					alarm->alarm_data = LSA_NULL;
				}
				}

				/***/

				if( dev->pdev_owner_ar_nr != 0 ) {

					CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, dev->pdev_owner_ar_nr);

					if( cm_is_not_null(argr) ) { /* always true */

						if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

							if( argr->cm.sv.arset->primary == argr ) {

								CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
								CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
								CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

								cm_ar_graph_find_api_slot_subslot(argr, /*api*/0, slot_nr, subslot_nr, &ar_ap, &ar_mod, &ar_sub);

								if( cm_is_not_null(ar_sub) ) { /* always true */

									LSA_UINT16 rsp = cm_sv_ar_almi_alarm_request(argr, alarm_rb, ar_ap, ar_mod, ar_sub, LSA_NULL); /* handle-mirror-set of pd_alarm.mirr */

									if( rsp != CM_OK ) {

										CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
											, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) could not send pdev-alarm, rsp(%u)"
											, argr->device_nr, argr->ar_nr
											, /*api*/0, slot_nr, subslot_nr
											, rsp
											);

										cm_sv_r1b_pdev_alarm_mirror_cancel(dev);

										r1pd_alarm->mirr_is_completed = LSA_TRUE; /* no resubmission */
									}
								}
								else {

									CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
										, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) could not send pdev-alarm, subslot not in AR"
										, argr->device_nr, argr->ar_nr
										, /*api*/0, slot_nr, subslot_nr
										);

									cm_sv_r1b_pdev_alarm_mirror_cancel(dev);

									r1pd_alarm->mirr_is_completed = LSA_TRUE; /* no resubmission */
								}
							}
						}
					}
				}
			}
		}
	}

	/***/

	cm_callback(dev->channel, CM_OK, &rb);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-ALARM MIRROR-CANCEL ====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_cancel(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if( r1pd_alarm->wf_cancel_posted ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = r1pd_alarm->msg_mirror_cancel;

		LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CANCEL;

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CANCEL == 0U); /* no parameters */

		r1pd_alarm->wf_cancel_posted = LSA_TRUE; /* see cm_sv_r1b_posted_pdev_alarm_mirror_cancel() */

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CANCEL, message_size, prefix);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED PDEV-ALARM MIRROR-CANCEL =============================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_pdev_alarm_mirror_cancel(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if(
		prefix != r1pd_alarm->msg_mirror_cancel
		|| !r1pd_alarm->wf_cancel_posted
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		r1pd_alarm->wf_cancel_posted = LSA_FALSE;

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE PDEV-ALARM MIRROR-CANCEL =============================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_pdev_alarm_mirror_cancel(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
) {
	if( message_size != CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CANCEL ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
		CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CANCEL == 0U); /* no parameters */
		LSA_UNUSED_ARG(message_body);

		/***/

		if( arset->is_aborting || cm_is_null(arset->arr[0]) ) { /* TIA 2253099 */
			/* late, do nothing */
			cm_callback(dev->channel, CM_OK, &rb);
			return;
		}

		if( cm_is_null(r1pd_alarm->orig) ) {

			CM_FATAL(); /* a bug */
		}

		r1pd_alarm->orig_is_completed = LSA_TRUE; /* no resubmission */

		cm_sv_r1b_pdev_alarm_mirror_clr(dev);

		/***/

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-ALARM MIRROR-CLEAR =====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_clr(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if( r1pd_alarm->wf_clr_posted ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = r1pd_alarm->msg_mirror_clr;

		LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CLR;

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CLR == 0U); /* no parameters */

		r1pd_alarm->wf_clr_posted = LSA_TRUE; /* see cm_sv_r1b_posted_pdev_alarm_mirror_clr() */

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CLR, message_size, prefix);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED PDEV-ALARM MIRROR-CLEAR ==============================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_pdev_alarm_mirror_clr(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if(
		prefix != r1pd_alarm->msg_mirror_clr
		|| !r1pd_alarm->wf_clr_posted
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		r1pd_alarm->wf_clr_posted = LSA_FALSE;

		/***/

		if( cm_is_null(r1pd_alarm->orig) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			if( !r1pd_alarm->orig_is_completed ) {
				/* do nothing (e.g. cancelled because of pull-port) */
			}

			cm_sv_r1b_pdev_alarm_mirror_join_done(dev);
		}

		/* join-on-broken: see cm_sv_r1b_join_broken() */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE PDEV-ALARM MIRROR-CLEAR ==============================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_pdev_alarm_mirror_clr(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body,
	LSA_UINT32 message_size
) {
	if( message_size != CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CLR ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
		CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

		LSA_UNUSED_ARG(message_body); /* no parameters */

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CLR == 0U); /* no parameters */

		/***/

		if( arset->is_aborting || cm_is_null(arset->arr[0]) ) { /* TIA 2253099 */
			/* late, do nothing */
			cm_callback(dev->channel, CM_OK, &rb);
			return;
		}

		if(
			cm_is_null(r1pd_alarm->mirr) /* always false (pre-allocated) */
			|| CM_RQB_GET_OPCODE(r1pd_alarm->mirr) == 0/*unused*/
		) {

			CM_FATAL(); /* R1 protocol error (message unexpected) */
		}
		else {

			CM_RQB_SET_OPCODE(r1pd_alarm->mirr, 0/*unused*/);

			r1pd_alarm->mirr_is_completed = LSA_FALSE;
		}

		/***/

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-ALARM MIRROR JOIN-DONE =================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_alarm_mirror_join_done(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev(%u) R1PD-alarm mirror join done, wf_set_posted(%u) wf_clr_posted(%u) orig_is_completed(%u)"
		, dev->device_nr
		, r1pd_alarm->wf_set_posted, r1pd_alarm->wf_clr_posted, r1pd_alarm->orig_is_completed
	);

	if( r1pd_alarm->wf_set_posted ) {

		/* wait for set posted, see cm_sv_r1b_posted_pdev_alarm_mirror_set() */
	}
	else if( r1pd_alarm->wf_clr_posted ) {

		/* wait for clr posted, see cm_sv_r1b_posted_pdev_alarm_mirror_clr() */
	}
	else { /* joined */

		if( cm_is_null(r1pd_alarm->orig) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE upper = r1pd_alarm->orig;
			r1pd_alarm->orig = LSA_NULL; /* unpark */

			r1pd_alarm->orig_is_completed = LSA_FALSE; /* reset */

			//R1 VV: possbile optimization: only 1 msg-buffer if join(set-posted) before clr-post.

			cm_sv_pdev_callback(dev->channel, upper, CM_RQB_GET_RESPONSE(upper));
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-ALARM RESUBMIT =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_alarm_resubmit(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if(
		cm_is_not_null(r1pd_alarm->orig)
		&& !r1pd_alarm->orig_is_completed
	) {

		CM_UPPER_RQB_PTR_TYPE alarm_rb = r1pd_alarm->orig;
		CM_UPPER_ALARM_PTR_TYPE param = alarm_rb->args.sv.ar_alarm_send;
		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

		cm_ar_graph_find_api_slot_subslot(argr, param->api, param->slot_nr, param->subslot_nr, &ar_ap, &ar_mod, &ar_sub);

		if( cm_is_null(ar_sub) ) { /* CIR, e.g. one port less */

			cm_sv_ar_almi_alarm_confirmation(argr, alarm_rb, CM_OK_CANCELLED);
		}
		else {

			if(
				!CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su)
				|| arset->is_aborting
				|| cm_is_not_null(arset->first)
				|| (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_PRMED) == 0
			) {

				cm_sv_ar_almi_alarm_confirmation(argr, alarm_rb, CM_OK_CANCELLED);
			}
			else {

				if( cm_sv_ar_almi_alarm_request(argr, alarm_rb, ar_ap, ar_mod, ar_sub, LSA_NULL) != CM_OK ) { /* resubmission of pd_alarm.orig */

					CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) could not send pdev-alarm (pd_alarm.orig)"
						, argr->device_nr, argr->ar_nr
						, param->api, param->slot_nr, param->subslot_nr
						);

					CM_FATAL(); /* a bug (unreachable, see checks above) */
				}
			}
		}
	}

	if(
		cm_is_not_null(r1pd_alarm->mirr) /* pre-allocated */
		&& CM_RQB_GET_OPCODE(r1pd_alarm->mirr) != 0/*unused*/
		&& !r1pd_alarm->mirr_is_completed
	) {

		CM_UPPER_RQB_PTR_TYPE alarm_rb = r1pd_alarm->mirr;
		CM_UPPER_ALARM_PTR_TYPE param = alarm_rb->args.sv.ar_alarm_send;
		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

		cm_ar_graph_find_api_slot_subslot(argr, param->api, param->slot_nr, param->subslot_nr, &ar_ap, &ar_mod, &ar_sub);

		if( cm_is_not_null(ar_sub) ) { /* always true */

			if(
				!CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su)
				|| arset->is_aborting
				|| cm_is_not_null(arset->first)
				|| (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_PRMED) == 0
			) {

				CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) could not send pdev-alarm"
					, argr->device_nr, argr->ar_nr
					, /*api*/0, param->slot_nr, param->subslot_nr
					);

				cm_sv_r1b_pdev_alarm_mirror_cancel(argr->cm.sv.dev);

				r1pd_alarm->mirr_is_completed = LSA_TRUE; /* no resubmission */
			}
			else if( cm_sv_ar_almi_alarm_request(argr, alarm_rb, ar_ap, ar_mod, ar_sub, LSA_NULL) != CM_OK ) { /* resubmission of pd_alarm.mirr */

				CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) could not send pdev-alarm (pd_alarm.mirr)"
					, argr->device_nr, argr->ar_nr
					, param->api, param->slot_nr, param->subslot_nr
					);

				CM_FATAL(); /* a bug (unreachable, see checks above) */
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
