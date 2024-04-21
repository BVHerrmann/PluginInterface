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
/*  F i l e               &F: cm_sv23.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Device things                                                 */
/*                                                                           */
/*  - device control                                                         */
/*  - device led info                                                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7230
#define CM_MODULE_ID       7230

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== DEVICE CONTROL ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_control(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_DEVICE_CONTROL_PTR_TYPE param = rb->args.sv.dev_control;
	LSA_BOOL unpark = LSA_FALSE;
	CM_SV_DEVICE_PTR_TYPE dev;
	LSA_UINT16 rsp;

	dev = cm_sv_dev_get_added(channel, param->device_nr);

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) is not added"
			, param->device_nr
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( cm_is_not_null(dev->curr) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) another request still running, opc(%u)"
			, dev->device_nr, CM_RQB_GET_OPCODE(dev->curr)
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u) cmd(0x%04x)"
			, dev->device_nr, param->cmd
			);

		switch( param->cmd ) {

		case CM_SV_DEVICE_CONTROL_CMD_ACTIVATE:

			if( CM_SV_DEV_IS_ACTIVE(dev) ) {

				rsp = CM_OK;
			}
			else {

				LSA_UINT16 ev, al;

				CmListLength(&dev->res.eventQ, &ev);
				CmListLength(&dev->res.alarmQ, &al);

				if( ev < dev->res.nr_of_event_resources || al < dev->res.nr_of_alarm_resources ) {

					CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "dev(%u) cannot activate, ev(%u < %u) or al(%u < %u)"
						, dev->device_nr
						, ev, dev->res.nr_of_event_resources
						, al, dev->res.nr_of_alarm_resources
						);

					rsp = CM_ERR_SEQUENCE;
				}
				else if( CM_SV_DEV_IS_PDEV_DEVICE(dev) && ! dev->central_pdev && ! cm_pdsv_occupant_is_nobody(channel) ) {

					CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "dev(%u) cannot activate, pdev is in use"
						, dev->device_nr
						);

					rsp = CM_ERR_SEQUENCE;
				}
				else if( dev->R1_enabled && cm_is_null(dev->r1b.send.resource) ) {

					CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "dev(%u) cannot activate, no R1BRIDGE resource"
						, dev->device_nr
						);

					rsp = CM_ERR_SEQUENCE;
				}
				else {

					/* note: API 0 exists always, see cm_sv_opc_device_add() */
					/* note: this enables rpc-call-indications */

					if( CM_SV_DEV_IS_PARKING(dev) ) {

						dev->is_parking = LSA_FALSE;

						unpark = LSA_TRUE;
					}

					if( CM_SV_DEV_IS_PDEV_DEVICE(dev) && ! dev->central_pdev ) {

						cm_pdsv_occupant_set_reset(channel, LSA_TRUE); /* TIA 213234 */
					}

					dev->is_active = LSA_TRUE;

					rsp = CM_OK;
				}
			}
			break;

		case CM_SV_DEVICE_CONTROL_CMD_PASSIVATE:

			if( dev->R1_enabled ) { /* note: passivate is not supported until required; semantics regarding bridge-state have to be defined */

				CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev(%u) passivate not supported if R1_enabled"
					, dev->device_nr
					);

				rsp = CM_ERR_SEQUENCE;
				break;
			}

			dev->is_active = LSA_FALSE;

			dev->is_parking = LSA_FALSE;

			cm_sv_ar_abort_all(dev, CM_AR_REASON_PAS); /* note: may call cm_sv_dev_done() */

			dev->curr = rb; /* pending, set after possible calls to cm_sv_dev_done() */

			rsp = CM_OK_PENDING; /* see below */
			break;

		default:
			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) unknown cmd(0x%04x)"
				, dev->device_nr, param->cmd
				);
			rsp = CM_ERR_PARAM;
			break;
		}
	}

	/***/

	if( rsp != CM_OK_PENDING ) {

		cm_callback(channel, rsp, &rb);
	}

	if( cm_is_not_null(dev) ) {

		if( unpark ) {

			LSA_UINT16 ar_nr;

			for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_TOT; ++ar_nr ) {

				CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

				if( cm_is_not_null(argr) ) {

					argr = cm_sv_ar_unpark(argr, CM_AR_REASON_NONE);

					if( cm_is_null(argr) ) {

						/* ok */
					}
				}
			}
		}
		else if( rsp == CM_OK_PENDING ) {

			cm_sv_dev_done(dev); /* trigger: passivate device */
		}
		else {

			/* do nothing */
		}
	}
}

/*===========================================================================*/
/*=== DEVICE LED-INFO =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_led_info_cancel(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {

	while( ! CmListIsEmpty(&dev->led.info_list) ) {

		CM_UPPER_RQB_PTR_TYPE rb;

		CmListRemoveHead(&dev->led.info_list, rb, CM_UPPER_RQB_PTR_TYPE);

		cm_callback(dev->channel, CM_OK_CANCELLED, &rb);
	}
}

/*===========================================================================*/

static LSA_UINT32
cm_sv_dev_led_info_substate(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {
	LSA_UINT32 led_info = 0; /* AP00392295 */
	LSA_UINT16 rt_cnt = 0;
	LSA_UINT16 id_cnt = 0;
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX/*!*/; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(argr) && (argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) == 0 ) {

			rt_cnt += 1;

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) { /* connect-phase */

				/* submodule state is not stable (real-config adaption, ownership, subst/wrong, a.r.p.) */
				/* best we can do is telling something that will not turn on the error LED (AP01481926) */

				led_info |= (CM_SV_DEVICE_LED_INFO_OWNED | CM_SV_DEVICE_LED_INFO_OK);
			}
			else { /* after connect-phase (i.e., after rpc-applready.cnf) */

				if( cm_is_not_null(argr->cm.sv.ar_connect_mem) && argr->cm.sv.ar_connect_mem->disconnect.was_in_data == CM_SV_WAS_IN_DATA_YES ) { /* TIA 1918234 */

					id_cnt += 1;
				}
				else {

					if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {

						id_cnt += 1;
					}
				}

				/***/

				{
				CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

				for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

					CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

					for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

						CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

						for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

							LSA_UINT16 sub_state = cm_sv_get_submodule_state(LSA_NULL/*without-diag*/, argr, ar_ap, ar_mod, ar_sub);

							if( cm_is_not_null(argr->cm.sv.arset) ) { /* system redundancy */

								/* system redundancy means: 1 AR of the 2 ARs of the AR-set is in charge (init: first AR, then: primary AR)
								 * special case: both ARs are backup
								 * special case: CIR-AR with additional submodules (reported as "pulled" in appl-ready; plug-alarms when primary)
								 *
								 * an SR-AR contains the PDEV -- otherwise connect.cnf(-), see cm_sv_argr_build()
								 * an SR-AR is the PDEV owner -- otherwise connect.cnf(-), see cm_sv_ar_connect_indication_unparked()
								 * on change of first/backup/primary the ownership is rearranged ... see cm_sv_arset_ownership_swap()
								 * conclusion: dev->pdev_owner_ar_nr contains the SR-AR that is in charge
								 * where "in charge" means: first, or primary, or the "correct" backup AR
								 */

								CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

								if( argr->ar_nr == dev->pdev_owner_ar_nr ) {

									/* this SR-AR is the AR in charge --> use sub_state */
								}
								else { /* not the SR-AR in charge */

									if( su->cand[0].ar_nr != dev->pdev_owner_ar_nr ) { /* not: cand[0].ar_nr == argr->ar_nr */

										/* CIR-AR with additional submodules --> use sub_state */
									}
									else {

										continue; /* skip */
									}
								}
							}

							CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
								, "dev/ar(%u/%u) led_info_substate (ap/sl/su(%u/%u/%04x)) sm_flags(0x%04x) sub_state(0x%04x)"
								, argr->device_nr, argr->ar_nr, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr, ar_sub->sv.sm_flags, sub_state
								);

							switch( (sub_state & CM_SUB_STATE_AR_INFO_MASK) ) {

							case CM_SUB_STATE_AR_INFO_OWNED:
								led_info |= CM_SV_DEVICE_LED_INFO_OWNED;
								break;

							case CM_SUB_STATE_AR_INFO_APPL_READY_PENDING:
								led_info |= CM_SV_DEVICE_LED_INFO_ARP;
								break;

							case CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED:
								led_info |= CM_SV_DEVICE_LED_INFO_LOCKED_SO;
								break;

							case CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER:
								led_info |= CM_SV_DEVICE_LED_INFO_LOCKED_IOC;
								break;

							case CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR:
								led_info |= CM_SV_DEVICE_LED_INFO_LOCKED_IOS;
								break;

							default: /* unreachable */
								break;
							}

							switch( (sub_state & CM_SUB_STATE_IDENT_MASK) ) {

							case CM_SUB_STATE_IDENT_OK:
								led_info |= CM_SV_DEVICE_LED_INFO_OK;
								break;

							case CM_SUB_STATE_IDENT_SUBSTITUTE:
								led_info |= CM_SV_DEVICE_LED_INFO_SUBST;
								break;

							case CM_SUB_STATE_IDENT_WRONG:
								led_info |= CM_SV_DEVICE_LED_INFO_WRONG;
								break;

							case CM_SUB_STATE_IDENT_NO_SUBMODULE:
								led_info |= CM_SV_DEVICE_LED_INFO_NO;
								break;

							default: /* unreachable */
								break;
							}
						}
					}
				}
				}
			}
		}
	}

	/***/

	if( rt_cnt != 0 && id_cnt == rt_cnt ) {

		led_info |= CM_SV_DEVICE_LED_INFO_RT_AR;
	}

	/***/

	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%u) led_info(0x%04x)"
		, dev->device_nr, led_info
		);

	return led_info;
}

/*===========================================================================*/

#if CM_CFG_SV_AR_HAS_MCONS

static LSA_BOOL
cm_sv_dev_led_info_mcons(
	CM_CHANNEL_PTR_TYPE channel,
	CM_SV_DEVICE_PTR_TYPE dev
) {
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(argr) ) {

			CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

			for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

				if( ar_iocr->cm.is_consumer && ar_iocr->cm.is_multicast ) {

					LSA_UINT16 ext_err = cm_sv_mcons_diag_value(argr, ar_iocr, LSA_NULL, LSA_NULL, LSA_NULL);

					if( ext_err != 0 ) { /* has (appearing) diagnosis */

						return LSA_TRUE;
					}
				}
			}
		}
	}

	/***/

	return LSA_FALSE;
}

#endif

/*===========================================================================*/

LSA_VOID
cm_sv_dev_led_info_eval(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {

	if( CmListIsEmpty(&dev->led.info_list) ) {

		/* the response is not back yet or the user does not use CM_OPC_SV_DEVICE_LED_INFO */
	}
	else {

		LSA_UINT32 led_info = 0;
		LSA_UINT32 mnt_info = 0;

		/***/

		led_info |= cm_sv_dev_led_info_substate(dev);

		/***/

		{
		CM_SV_AP_PTR_TYPE ap;

		for( CmListEach(ap, &dev->ap_list, CM_SV_AP_PTR_TYPE) ) {

			CM_SV_SLOT_PTR_TYPE sl;

			for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su;

				for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

					LSA_UINT32 temp_mnt;

					if( cm_sv_diag_maint_qual(dev, ap, sl, su, LSA_NULL, LSA_NULL, &temp_mnt) ) {

						led_info |= CM_SV_DEVICE_LED_INFO_DIAG;
					}

					mnt_info |= temp_mnt;
				}
			}
		}
		}

		/***/

#if CM_CFG_SV_AR_HAS_MCONS

		if( cm_sv_dev_led_info_mcons(dev) ) {

			led_info |= CM_SV_DEVICE_LED_INFO_MCONS;
		}

#endif

		/***/

		{
		CM_UPPER_RQB_PTR_TYPE curr;

		for( CmListEach(curr, &dev->led.info_list, CM_UPPER_RQB_PTR_TYPE) ) {

			if(
				curr->args.sv.dev_led_info->led_info != led_info
				|| curr->args.sv.dev_led_info->maint_info != mnt_info
			) {
				CM_UPPER_RQB_PTR_TYPE temp = curr; /* has delta */

				curr = CmListPrevOrHead((CM_LIST_ENTRY_PTR_TYPE)curr, CM_UPPER_RQB_PTR_TYPE);

				CmListRemoveEntry((CM_LIST_ENTRY_PTR_TYPE)temp);

				/* update */

				temp->args.sv.dev_led_info->led_info   = led_info;
				temp->args.sv.dev_led_info->maint_info = mnt_info;

				cm_callback(dev->channel, CM_OK, &temp);
			}
		}
		}
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_led_info(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_DEVICE_LED_INFO_PTR_TYPE param = rb->args.sv.dev_led_info;
	CM_SV_DEVICE_PTR_TYPE dev;

	dev = cm_sv_dev_get_added(channel, param->device_nr);

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) is not added"
			, param->device_nr
			);

		cm_callback(channel, CM_ERR_SEQUENCE, &rb);
	}
	else {

		/* dev->curr == don't care */

		CmListInsertTail(&dev->led.info_list, (CM_LIST_ENTRY_PTR_TYPE)rb);

		cm_sv_dev_led_info_eval(dev); /* trigger: LED-info opcode */
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
