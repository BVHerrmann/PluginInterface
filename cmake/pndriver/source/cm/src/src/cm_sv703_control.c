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
/*  F i l e               &F: cm_sv703_control.c                        :F&  */
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
/*  - Bridge Control                                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7703
#define CM_MODULE_ID       7703

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_control_broken(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
); /* forward */

static LSA_VOID
cm_sv_r1b_control_established_active(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
); /* forward */

static LSA_VOID
cm_sv_r1b_control_established_passive(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
); /* forward */

/*===========================================================================*/
/*=== R1-BRIDGE CONTROL =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_r1bridge_control(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_R1BRIDGE_CONTROL_PTR_TYPE param = rb->args.sv.r1bridge_control;
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
			, "dev(%u) device not R1_enabled"
			, dev->device_nr
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else if( ! CM_SV_DEV_IS_ACTIVE(dev) ) { /* note: CM-SV must control the PDEV */

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) device not active"
			, dev->device_nr
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else if( cm_is_not_null(dev->r1b.control.bridge_broken_rqb) ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) control-broken is running"
			, dev->device_nr
			);

		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}
	else {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) r1bridge-control: cmd(%u), bridge_state(%u)"
			, dev->device_nr, param->cmd, dev->r1b.bridge_state
			);

		switch( param->cmd ) {

		case CM_SV_R1BRIDGE_CONTROL_CMD_BROKEN:
			cm_sv_r1b_control_broken(dev, rb);
			break;

		case CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_ACTIVE:
			cm_sv_r1b_control_established_active(dev, rb);
			break;

		case CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_PASSIVE:
			cm_sv_r1b_control_established_passive(dev, rb);
			break;

		default:
			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) unknown cmd(%u)"
				, dev->device_nr, param->cmd
				);

			CM_FATAL(); /* note: fatal by design (TIA 1495347) */
			break;
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE CONTROL BROKEN ==============================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_control_broken(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	if( dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN ) {
		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}

	if( cm_is_not_null(dev->r1b.control.bridge_broken_rqb) ) {
		CM_FATAL(); /* a bug (see caller) */
	}

	dev->r1b.bridge_state = CM_SV_DEV_R1B_STATE_BROKEN;

	dev->r1b.control.bridge_broken_rqb = rb;

	/***/

	{
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(argr) ) {

			if( argr->cm.sv.r1_flags != 0 ) {

				cm_sv_r1b_cancel_connect_done(argr);
				cm_sv_r1b_cancel_disconnect_done(argr); /* note: kills the representative AR */

				/* note: ALMI has no "expect" flags to be cancelled */
			}
		}
	}
	}

	cm_sv_r1b_cancel_arset(dev);

	cm_sv_r1b_pdev_A_cancel(dev);
	cm_sv_r1b_pdev_B_cancel(dev);

	cm_sv_r1b_stat_A_cancel(dev);
	cm_sv_r1b_stat_B_cancel(dev);

	cm_sv_r1b_cancel_diag(dev);

	/***/

	cm_sv_r1b_join_broken(dev);
}

/*===========================================================================*/
/*=== R1-BRIDGE JOIN BROKEN =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_join_broken(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( cm_is_null(dev->r1b.control.bridge_broken_rqb) ) {

		/* do nothing */
	}
	else {

		if( cm_is_null(dev->r1b.send.resource) ) {

			return; /* CM_OPC_SV_R1BRIDGE_SEND_RSP pending */
		}

		CM_ASSERT(CmListIsEmpty(&dev->r1b.send.queue));

		if( cm_is_null(dev->r1b.proxy_connect_done) ) {

			CM_FATAL(); /* a bug (note: if null then enqueued in send.queue, see check above) */
		}

		if( ! cm_sv_r1b_pdev_A_is_idle(dev) ) {

			return;
		}

		if( ! cm_sv_r1b_pdev_B_is_idle(dev) ) {

			return;
		}

		if( ! cm_sv_r1b_stat_A_is_idle(dev) ) {

			return;
		}

		if( ! cm_sv_r1b_stat_B_is_idle(dev) ) {

			return;
		}

		if( ! cm_sv_r1b_diag_is_idle(dev) ) {

			CM_FATAL(); /* a bug (note: if not-null then enqueued in send.queue, see check above) */
		}

		/* note: cm_sv_r1b_posted_update_begin()   if active then enqueued in send.queue, see check above */
		/* note: cm_sv_r1b_posted_update_diag()    if active then enqueued in send.queue, see check above */
		/* note: cm_sv_r1b_posted_update_connect() if active then enqueued in send.queue, see check above */
		/* note: cm_sv_r1b_posted_update_alarms()  if active then enqueued in send.queue, see check above */
		/* note: cm_sv_r1b_posted_update_end()     if active then enqueued in send.queue, see check above */

		/* note: cm_sv_r1b_posted_pdev_alarm_mirror_set() if active then enqueued in send.queue, see check above */
		/* note: cm_sv_r1b_posted_pdev_alarm_mirror_clr() if active then enqueued in send.queue, see check above */

		/* note: cm_sv_r1b_posted_pdev_diag_replace() if active then enqueued in send.queue, see check above */

		/* note: cm_sv_r1b_posted_reset_to_factory() if active then enqueued in send.queue, see check above */

		/***/

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) R1 bridge-control(broken) done"
			, dev->device_nr
			);

		cm_callback(dev->channel, CM_OK, &dev->r1b.control.bridge_broken_rqb); /* everything idle */

		/***/

		cm_sv_dev_done(dev);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE CHECK BRIDGE-VERSION ========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_check_bridge_version(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_SV_R1BRIDGE_CONTROL_CONST_PTR_TYPE param
){
#if CM_SV_R1BRIDGE_VERSION_OCT0 != 1
#error "must implement code for adaption"
#endif

	if(
		param->bridge_version[0] != CM_SV_R1BRIDGE_VERSION_OCT0
		|| param->bridge_version[1] != CM_SV_R1BRIDGE_VERSION_OCT1
		|| param->bridge_version[2] != CM_SV_R1BRIDGE_VERSION_OCT2
		|| param->bridge_version[3] != CM_SV_R1BRIDGE_VERSION_OCT3
	) {

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) r1bridge-version(%u/%u/%u/%u) is different"
			, dev->device_nr
			, param->bridge_version[0], param->bridge_version[1], param->bridge_version[2], param->bridge_version[3]
			);

		/* note: this code implements the first bridge-version and thus the other's version is higher... */
	}

	if( param->bridge_version[0] == 0 ) {
		CM_FATAL(); /* by design: version 0 was never released (TIA 1490021) */
	}

	LSA_UNUSED_ARG (dev);
}

/*===========================================================================*/
/*=== R1-BRIDGE CONTROL ESTABLISHED-ACTIVE ==================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_control_established_active(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {

	//R1 VV: make function or macro to check bridge-state. (not only here)
	if( ! (dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_UNKNOWN || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN) ) {
		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}

	cm_sv_r1b_check_bridge_version(dev, rb->args.sv.r1bridge_control);

	dev->r1b.bridge_state = CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE;

	cm_callback(dev->channel, CM_OK, &rb);

	/***/

	{ /* reset flag "r1_mirrored" */
	CM_SV_AP_PTR_TYPE ap;

	for( CmListEach(ap, &dev->ap_list, CM_SV_AP_PTR_TYPE) ) {

		CM_SV_SLOT_PTR_TYPE sl;

		for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

			CM_SV_SUBSLOT_PTR_TYPE su;

			for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

				LSA_UINT16 diag_type;

				for( diag_type = 0; diag_type < CM_SV_DIAG_TYPE_MAX; diag_type++ ) {

					CM_SV_DIAG_PTR_TYPE sv_diag;

					for( CmListEach(sv_diag, &su->diag_list[diag_type], CM_SV_DIAG_PTR_TYPE) ) {

						sv_diag->r1_mirrored = LSA_FALSE;
					}
				}
			}
		}
	}
	}

	/* wait for cm_sv_r1b_handle_update_begin() */
}

/*===========================================================================*/
/*=== R1-BRIDGE CONTROL ESTABLISHED-PASSIVE =================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_control_established_passive(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_UNKNOWN ) {
		CM_FATAL(); /* note: fatal by design (TIA 1495347) */
	}

	cm_sv_r1b_check_bridge_version(dev, rb->args.sv.r1bridge_control);

	dev->r1b.bridge_state = CM_SV_DEV_R1B_STATE_ESTABLISHING_PASSIVE;

	cm_callback(dev->channel, CM_OK, &rb);

	/***/

	{
	CM_SV_AP_PTR_TYPE ap;

	for( CmListEach(ap, &dev->ap_list, CM_SV_AP_PTR_TYPE) ) {

		CM_SV_SLOT_PTR_TYPE sl;

		for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

			CM_SV_SUBSLOT_PTR_TYPE su;

			for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

				LSA_UINT16 diag_type;

				for( diag_type = 0; diag_type < CM_SV_DIAG_TYPE_MAX; diag_type++ ) {

					if( ! CmListIsEmpty(&su->diag_list[diag_type]) ) {

						CM_FATAL(); /* a user-bug */
					}
				}
			}
		}
	}
	}

	/***/

	cm_sv_r1b_update_begin(dev);
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
