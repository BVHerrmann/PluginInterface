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
/*  F i l e               &F: cm_sv41.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, AR things                                                     */
/*                                                                           */
/*  - CM-SV-AR state-machine                                                 */
/*    (connect, check, config-change, ar-info)                               */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7410
#define CM_MODULE_ID       7410

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

#define CM_PNIO_ERR_CO_PROBLEM  CM_PNIO_FAULTY_Connect__CMRPC_OutOfARResources()
#define CM_PNIO_ERR_SC_PROBLEM  CM_PNIO_FAULTY_IOCRBlockReq(10)
#define CM_PNIO_ERR_PO_PROBLEM  CM_PNIO_FAULTY_Connect__CMRPC_PdevAlreadyOwned()
#define CM_PNIO_ERR_IR_PROBLEM  CM_PNIO_FAULTY_IRInfoBlock(5)

static LSA_UINT32
cm_sv_ar_connect_indication_unparked(
	CM_AR_GRAPH_PTR_TYPE argr
); /* forward */

static LSA_BOOL
cm_sv_ar_connect_indication_CO_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
); /* forward */

static LSA_BOOL
cm_sv_ar_connect_indication_SC_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 pdev_send_clock,
	LSA_BOOL is_rr_fixed
); /* forward */

static LSA_BOOL
cm_sv_ar_connect_indication_PO_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
); /* forward */

static LSA_BOOL
cm_sv_ar_connect_indication_IR_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
); /* forward */

static LSA_VOID
cm_sv_ar_rebind(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
); /* forward */

static LSA_VOID
cm_sv_ar_ownership_indication_join(
	CM_AR_GRAPH_PTR_TYPE argr
); /* forward */

static LSA_VOID
cm_sv_ar_ownership_conn_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb
); /* forward */

static LSA_VOID
cm_sv_ar_ownership_plug_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb
); /* forward */

static LSA_VOID
cm_sv_ar_ownership_pullpdev_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb
); /* forward */

static LSA_BOOL
cm_sv_ar_ownership_is_pdev_locked(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership
); /* forward */

/*===========================================================================*/
/*=== RPC CONNECT INDICATION ================================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_rpc_connect_indication(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_AR_GRAPH_PTR_TYPE argr;

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u) connect.ind"
		, dev->device_nr
		);

	argr = cm_sv_argr_alloc_and_init(dev); /* note: argr->ar_nr = 0, will be assigned in cm_sv_ar_alloc() */

	if( cm_is_null(argr) ) {

		LSA_UINT32 pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) could not cm_sv_argr_alloc_and_init, pnio_err(0x%08x)"
			, dev->device_nr
			, pnio_err
			);

		return pnio_err;
	}

	/***/

	{
	LSA_UINT32 pnio_err = cm_sv_argr_build(argr, rpc, LSA_NULL, 0); /* note: argr->ar_nr may be 0 and/or ar_uuid may be NIL */

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) could not cm_sv_argr_build, pnio_err(0x%08x)"
			, dev->device_nr
			, pnio_err
			);

		cm_sv_ar_free(argr, pnio_err, CM_AR_REASON_CONNECT);

		return pnio_err;
	}
	}

	/* attach the AR to the submodules and preallocate memory */

	{
	LSA_UINT32 pnio_err;

	if( ! cm_sv_ar_attach(argr) ) {

		cm_sv_ar_detach(argr); /* undo partial attach */

		pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();
	}
	else {

		if( ! cm_sv_argr_prealloc(argr) ) {

			pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();
		}
		else {

			pnio_err = CM_PNIO_ERR_NONE;
		}
	}

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		cm_sv_ar_free(argr, pnio_err, CM_AR_REASON_CONNECT);

		return pnio_err;
	}
	}

	/***/

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

		CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
		LSA_UINT32 data_length;
		LSA_UINT16 nr_of_cons;

		data_ptr    = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in caller */
		data_length = rpc->args.sv.call->ndr_len  - CLRPC_PNIO_OFFSET;

		nr_of_cons = 0;

		{
		CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

		for( CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

			if( ! iocr->cm.is_multicast && iocr->cm.is_consumer ) {

				nr_of_cons += 1;
			}
		}
		}

		argr->cm.sv.r1b.msg_connect = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_CONNECT(nr_of_cons) + data_length);

		if( cm_is_null(argr->cm.sv.r1b.msg_connect) ) {

			CM_FATAL(); //R1 VV: move to OK-path of alloc to avoiding this fatal.
		}
		else {

			LSA_UINT32 message_max;
			CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(argr->cm.sv.r1b.msg_connect, &message_max);
			LSA_UINT16 alarm_ref = 0;
			LSA_UINT32 offs;

			if( cm_is_not_null(argr->alarm_cr) ) { /* note: exists, see PDU-checks */

				alarm_ref = argr->alarm_cr->local_alarm_ref;
			}

			CM_PUT16_HTON(message_body, 0, alarm_ref);
			CM_PUT16_HTON(message_body, 2, nr_of_cons);

			CM_ASSERT(CM_SV_R1BMSG_SIZE_CONNECT(0) == 4U);

			offs = 4;

			{
			CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

			for( CmListForeach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

				if( ! iocr->cm.is_multicast && iocr->cm.is_consumer ) {

					CM_PUT16_HTON(message_body, offs + 0, iocr->iocr_ref);
					CM_PUT16_HTON(message_body, offs + 2, iocr->frame_id);

					offs += 4;
				}
			}
			}

			if( offs != CM_SV_R1BMSG_SIZE_CONNECT(nr_of_cons) ) {
				CM_FATAL();
			}

			CM_MEMCPY(message_body + offs, data_ptr, data_length); /* connect-pdu */

			if( offs + data_length != message_max ) { /* note: if this changes, must change cm_sv_r1b_post_connect() too! */
				CM_FATAL();
			}
		}
	}

	/***/

	if(
		argr->ar_nr == 0
		|| cm_is_not_null(argr->cm.sv.curr_rpc_ind)
		|| (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_UNPARK) != 0
	) {
		CM_FATAL();
	}

	argr->cm.sv.curr_rpc_ind = rpc; /* pending */

	argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_UNPARK;

	if( CM_SV_DEV_IS_PARKING(dev) ) {

		return CM_PNIO_ERR_NONE; /* pending (note: the PDEV is being parameterized via CM-PD) */
	}
	else {

		argr = cm_sv_ar_unpark(argr, CM_AR_REASON_NONE);

		if( cm_is_null(argr) ) {

			return CM_PNIO_ERR_NONE; /* done */
		}
		else {

			return CM_PNIO_ERR_NONE; /* pending */
		}
	}
}

/*===========================================================================*/
/*===========================================================================*/
/*=== AR UNPARK =============================================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_unpark(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 reason
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev/ar(%u/%u) reason(%u)"
		, argr->device_nr, argr->ar_nr
		, reason
		);

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_UNPARK) == 0 ) {

		/* do nothing (see callers: for all ARs ...) */
	}
	else {

		LSA_UINT32 pnio_err;

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_UNPARK;

		/***/

		if( reason != CM_AR_REASON_NONE ) {

			pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfARResources();
		}
		else {

			pnio_err = cm_sv_ar_connect_indication_unparked(argr);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				reason = CM_AR_REASON_CONNECT;
			}
		}

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

				CM_FATAL();
			}
			else {

				CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev; /* save, see cm_sv_ar_free() below */

				CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
				argr->cm.sv.curr_rpc_ind = LSA_NULL;

				cm_sv_ar_free(argr, pnio_err, reason);

				cm_sv_rpc_call_response(dev, rpc, CLRPC_OK, pnio_err, 0/*nice*/);
			}

			return LSA_NULL;
		}
	}

	return argr;
}

/*===========================================================================*/
/*=== AR CONNECT-INDICATION UNPARKED ========================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_ar_connect_indication_unparked(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

		/* must not attach to CM-PD (no send-clock and must not interfere with RT-ARs) */

		argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PD_ATTACHED; /* sanity, flag is already reset */

		/***/

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_IPSUITE_LOCK;

		cm_oha_control_ip_suite(argr, OHA_LOCK_SET, argr->cm.sv.lock_rqb); /* see comment in cm_sv_ar_control_ip_suite_done() */

		/***/

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_FRAMES_ADD; /* sanity */

		cm_sv_ar_connect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_FRAMES_ADD (faked, easy-supervisor) */

		return CM_PNIO_ERR_NONE; /* pending */
	}
	else {

		LSA_UINT32 pnio_err = CM_PNIO_ERR_NONE;
		LSA_BOOL is_rr_fixed;
		LSA_UINT16 pdev_send_clock;

		/* logic revised with V5.0i4 (AP00914416) and V5.2i1.26 (AP01344221, see document in the attachment) */
		/* limitation of send-clock to 1ms (Class1+UDP, 2^x) was obsoleted with V4.2i2.11 (AP01009553, August 2010) */

		pdev_send_clock = cm_pdusr_is_send_clock_fixed(argr->ar_com.channel, &is_rr_fixed);

		if( pdev_send_clock == 0 ) { /* not fixed */

			pdev_send_clock = argr->ar_com.ar_send_clock; /* this is the new PDEV send-clock */
		}

		/* prepare the parameters for "add" */

		{
		CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

		for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

			ar_iocr->cm.adpt.send_clock      = pdev_send_clock; /* see adaption in cm_sv_ar_connect_indication_SC_problem() */
			ar_iocr->cm.adpt.reduction_ratio = ar_iocr->reduction_ratio;
			ar_iocr->cm.adpt.phase           = ar_iocr->phase;

			if( ar_iocr->cm.is_consumer ) {

				ar_iocr->cm.adpt.sequence = 0; /* sanity (see EDD, consumers don't have a "sequence") */
				ar_iocr->cm.adpt.data_hold_factor = ar_iocr->data_hold_factor;
			}
			else { /* provider */

				ar_iocr->cm.adpt.sequence = argr->cm.sv.provider_sequence; /* AP00619993, see cm_sv_dev_calc_provider_sequence() */
				ar_iocr->cm.adpt.data_hold_factor = 0;
			}
		}
		}

		/* check if there is a combinatorial problem */

		if( cm_sv_ar_connect_indication_CO_problem(argr) ) {

			pnio_err = CM_PNIO_ERR_CO_PROBLEM;

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) CO-problem, pnio_err(0x%08x)"
				, argr->device_nr, argr->ar_nr
				, pnio_err
				);

			return pnio_err; /* cannot continue, caller will cm_sv_logbook_write() */
		}

		/* check if there is a problem with the send-clock */

		if( cm_sv_ar_connect_indication_SC_problem(argr, pdev_send_clock, is_rr_fixed) ) {

			pnio_err = CM_PNIO_ERR_SC_PROBLEM;

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) SC-problem, pnio_err(0x%08x)"
				, argr->device_nr, argr->ar_nr
				, pnio_err
				);

			return pnio_err; /* TIA 1868862 (V7.0i1.3x) updates AP01344221 */
		}

		/* R1: check for overlapping ownership */

		if( argr->cm.sv.dev->R1_enabled ) { /* not: argr->cm.sv.r1_flags */

			if( cm_sv_r1b_ownership_overlapping(argr) ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_ARSetStateConflictDuringConnectionEstablishment(); /* AR-set state conflict (overlapping ARs not supported) */
			}
		}

		/* queue for ownership (sets the pdev-owner as a side-effect) */

		cm_sv_ownership_queue_for(argr);

		/* if no problem, check if there is a problem with the pdev-ownership */

		if( pnio_err == CM_PNIO_ERR_NONE ) {

			if( cm_sv_ar_connect_indication_PO_problem(argr) ) {

				pnio_err = CM_PNIO_ERR_PO_PROBLEM; /* PDEV is central or another AR with PDEV already exists */

				CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev/ar(%u/%u) PO-problem, pnio_err(0x%08x)"
					, argr->device_nr, argr->ar_nr
					, pnio_err
					);

				return pnio_err; /* TIA 1678268 (V7.0i1.7) updates AP01344221 */
			}
		}

		/* if no problem, check if there is a problem with the IRDataUUID */

		if( pnio_err == CM_PNIO_ERR_NONE ) {

			if( cm_sv_ar_connect_indication_IR_problem(argr) ) {

				pnio_err = CM_PNIO_ERR_IR_PROBLEM;

				CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev/ar(%u/%u) IR-problem, pnio_err(0x%08x)"
					, argr->device_nr, argr->ar_nr
					, pnio_err
					);
			}
		}

		/* if there is a problem, set the problem-flag and make a log-entry */

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			argr->cm.sv.ar_flags |= CM_SV_AR_FL_AR_PROBLEM; /* make all submodules so-locked */

			cm_sv_logbook_write(argr, pnio_err, CM_AR_REASON_NONE/*!*/);
		}

		/* if no SC-problem, attach to the send-clock */

		if( pnio_err != CM_PNIO_ERR_SC_PROBLEM ) {

			LSA_UINT16 rc = cm_pdusr_attach(argr->ar_com.channel, pdev_send_clock);

			if( rc == CM_OK || rc == CM_OK_PENDING ) {

				argr->cm.sv.ar_flags |= CM_SV_AR_FL_PD_ATTACHED;

				if( rc == CM_OK_PENDING ) {

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_SEND_CLOCK;
				}
			}
			else {

				CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "dev/ar(%u/%u) could not cm_pdusr_attach, rc(%u)"
					, argr->device_nr, argr->ar_nr
					, rc
					);

				/* note: must not cm_sv_ar_sendclock_changed(failed) here, see return value of cm_sv_ar_unpark() */

				return CM_PNIO_ERR_SC_PROBLEM; /* cannot continue (a bug or out of mem) */
			}
		}

		/***/

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_SEND_CLOCK) == 0 ) {

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_SEND_CLOCK;

			cm_sv_ar_sendclock_changed(argr, pdev_send_clock); /* fake */
		}

		/***/

		return CM_PNIO_ERR_NONE; /* pending */
	}
}

/*===========================================================================*/
/*=== AR CONNECT-INDICATION CO-PROBLEM ======================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_connect_indication_CO_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	LSA_UINT16 cnt_rtc3_advanced = 0;
	LSA_UINT16 cnt_rtc3_legacy = 0;
	LSA_UINT16 cnt_rtc12UDP_advanced = 0;
	LSA_UINT16 cnt_rtc12UDP_legacy = 0;
	LSA_UINT16 cnt_rtc12UDP_pdev = 0;
	LSA_BOOL co_problem = LSA_FALSE;

	/* check if there is a combinatorial problem (note: cannot be done in cm_sv_ar_alloc() because of nr_of_pdev_submodules) */

	{
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(temp_argr) && temp_argr != argr ) {

			if( CM_SV_AR_IS_RTCLASS3(temp_argr) ) {

				if( CM_SV_AR_IS_ADVANCED(temp_argr) ) {

					cnt_rtc3_advanced += 1;
				}
				else {

					cnt_rtc3_legacy += 1;
				}
			}
			else { /* RTClass 1/2/UDP */

				if( CM_SV_AR_IS_ADVANCED(temp_argr) ) {

					cnt_rtc12UDP_advanced += 1;
				}
				else {

					cnt_rtc12UDP_legacy += 1;
				}

				if( temp_argr->cm.sv.nr_of_pdev_submodules != 0 ) {

					cnt_rtc12UDP_pdev += 1;
				}
			}
		}
	}
	}

	/***/

	if( CM_SV_AR_IS_RTCLASS3(argr) ) {

		if( ! CM_SV_DEV_IS_PDEV_DEVICE(dev) ) {

			/* note: if this condition is removed, the IR-problem check+abort logic (for non-PDEV-ARs) must be reviewed */

			co_problem = LSA_TRUE; /* RTClass3 advanced is supported only in the pdev-device */
		}

		if( CM_SV_AR_IS_ADVANCED(argr) ) {

			if( cnt_rtc3_legacy != 0 ) {

				co_problem = LSA_TRUE; /* legacy exists, thus advanced is not allowed (AP00914416) */
			}

			if( cnt_rtc12UDP_pdev != 0 ) {

				if( argr->cm.sv.nr_of_pdev_submodules != 0 ) {

					/* allowed, is a PO-problem */
				}
				else {

					co_problem = LSA_TRUE; /* RTC3-AR advanced and RTC12UDP-PDEV-AR is not allowed (AP00914416) */
				}
			}
		}
		else { /* legacy */

			if( cnt_rtc3_advanced != 0 ) {

				co_problem = LSA_TRUE; /* advanced exists, thus legacy is not allowed (AP00914416) */
			}

			if( cnt_rtc3_legacy != 0 ) {

				co_problem = LSA_TRUE; /* only 1 legacy RTC3-AR is allowed (AP00914416, AP00774491) */
			}

			if( cnt_rtc12UDP_pdev != 0 ) {

				/* RTC3-AR legacy (i.e., with PDEV) and RTC12UDP-PDEV-AR was possible before (is a PO-problem) */
			}
		}
	}
	else { /* RTClass 1/2/UDP */

		if( CM_SV_AR_IS_ADVANCED(argr) ) {

			if( cnt_rtc12UDP_legacy != 0 ) {

				/* legacy and advanced is allowed (V5.2i1.26, AP01344221 updates AP00914416) */
			}
		}
		else { /* legacy */

			if( cnt_rtc12UDP_advanced != 0 ) {

				/* legacy and advanced is allowed (V5.2i1.26, AP01344221 updates AP00914416) */
			}
		}

		if( argr->cm.sv.nr_of_pdev_submodules != 0 ) {

			if( cnt_rtc3_legacy != 0 ) {

				/* RTC12UDP-PDEV-AR and RTC3-AR legacy (i.e., with PDEV) was possible before (is a PO-problem) */
			}

			if( cnt_rtc3_advanced != 0 ) {

				co_problem = LSA_TRUE; /* RTC12UDP-PDEV-AR and RTC3-AR is not allowed (AP00914416) */
			}
		}
	}

	/***/

	if( co_problem ) {

		CM_SV_TRACE_10(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev(%u/%u) pdev-device(%u) is_rtc3(%u) is_adv(%u), rtc3_adv(%u) rtc3_leg(%u) rtc12UDP_adv(%u) rtc12UDP_leg(%u) rtc12UDP_pdev(%u)"
			, argr->device_nr, argr->ar_nr
			, CM_SV_DEV_IS_PDEV_DEVICE(dev)
			, CM_SV_AR_IS_RTCLASS3(argr)
			, CM_SV_AR_IS_ADVANCED(argr)
			, cnt_rtc3_advanced, cnt_rtc3_legacy
			, cnt_rtc12UDP_advanced, cnt_rtc12UDP_legacy, cnt_rtc12UDP_pdev
			);
	}

	/***/

	return co_problem;
}

/*===========================================================================*/
/*=== AR CONNECT-INDICATION SC-PROBLEM ======================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_connect_indication_SC_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 pdev_send_clock,
	LSA_BOOL is_rr_fixed
) {
	LSA_BOOL sc_problem;

	/* check if there is a problem with the send-clock */

	if( argr->ar_com.ar_send_clock == pdev_send_clock ) {

		sc_problem = LSA_FALSE;
	}
	else { /* try to adapt the RR (and phase), calculation works only for clock = 2^x */

		if( is_rr_fixed ) {

			sc_problem = LSA_TRUE; /* adaption not allowed */
		}
		else if( CM_SV_AR_IS_RTCLASS3(argr) ) {

			sc_problem = LSA_TRUE; /* adaption not possible */
		}
		else if( ! argr->ar_com.is_power_of_two ) {

			sc_problem = LSA_TRUE; /* adaption not possible */
		}
		else if( ! cm_is_power_of_2(pdev_send_clock) ) {

			sc_problem = LSA_TRUE; /* adaption not possible */
		}
		else { /* note: both clocks are 2^x here, RR and phase are within bounds */

			sc_problem = LSA_FALSE;

			{
			CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

			for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

				LSA_UINT32 rr = ar_iocr->cm.adpt.reduction_ratio;
				LSA_UINT32 ph = ar_iocr->cm.adpt.phase;

				rr *= argr->ar_com.ar_send_clock;
				rr /= pdev_send_clock;

				ph -= 1;
				ph *= argr->ar_com.ar_send_clock;
				ph /= pdev_send_clock;
				ph += 1;

				if( ! cm_ar_graph_reduction_ratio_range_ok((LSA_UINT16)rr, LSA_TRUE, (ar_iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK)) ) {

					sc_problem = LSA_TRUE; /* adaption not possible */

					rr = ph = 0; /* sanity */
				}

				ar_iocr->cm.adpt.reduction_ratio = (LSA_UINT16)rr;
				ar_iocr->cm.adpt.phase           = (LSA_UINT16)ph;
			}
			}
		}
	}

	/***/

	return sc_problem;
}

/*===========================================================================*/
/*=== AR CONNECT-INDICATION PO-PROBLEM ======================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_connect_indication_PO_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {

	if( argr->cm.sv.nr_of_pdev_submodules != 0 ) {

		if( cm_is_not_null(argr->cm.sv.arset) ) {

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

			{
			LSA_UINT16 idx;

			for( idx = 0; idx < CM_SV_ARSET_MAX_ARS; ++idx ) {

				CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[idx];

				if( cm_is_not_null(temp_argr) ) {

					if( cm_sv_ar_is_pdev_owner(temp_argr) ) {

						return LSA_FALSE;
					}
				}
			}
			}

			return LSA_TRUE; /* problem: no AR of the AR-set is PDEV-owner */
		}
		else {

			if( ! cm_sv_ar_is_pdev_owner(argr) ) {

				return LSA_TRUE; /* problem: the PDEV-AR is not PDEV-owner */
			}
		}
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== AR LOOKUP ANOTHER RTCLASS3 ADVANCED ===================================*/
/*===========================================================================*/

static CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup_another_rtc3adv(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_CONST_PTR_TYPE this_argr /* another = not this */
) {
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(temp_argr) ) {

			if( temp_argr != this_argr && CM_SV_AR_IS_RTCLASS3(temp_argr) && CM_SV_AR_IS_ADVANCED(temp_argr) ) {

				return temp_argr; /* another AR with "RTClass3 advanced" exists */
			}
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== AR CONNECT-INDICATION IR-PROBLEM ======================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_connect_indication_IR_problem(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_BOOL ir_problem;

	/* check if there is a problem with the IRDataUUID */

	if( ! CM_SV_AR_IS_RTCLASS3(argr) ) {

		ir_problem = LSA_FALSE; /* IRDataUUID is don't care for RTClass 1/2/UDP (legacy and advanced) */
	}
	else if( ! CM_SV_AR_IS_ADVANCED(argr) ) {

		ir_problem = LSA_FALSE; /* RTClass3 legacy: is the PDEV-owner and will parameterize the PDEV with the correct PDIRData */
	}
	else if( cm_is_null(argr->ir_info) ) {

		ir_problem = LSA_TRUE; /* actually unreachable */
	}
	else { /* i.e., RTClass3 advanced -- multiple ARs are possible */

		CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;

		CLRPC_UUID_PTR_TYPE pdev_ir_data_uuid = cm_pdusr_get_ir_data_uuid(dev->channel);

		CLRPC_UUID_PTR_TYPE argr_ir_data_uuid = &argr->ir_info->ir_data_uuid;

		/***/

		if( dev->central_pdev ) {

			/* if the IRDataUUID matches, the AR can be established */

			if( cm_uuid_equal(argr_ir_data_uuid, pdev_ir_data_uuid) ) {

				ir_problem = LSA_FALSE;
			}
			else {

				ir_problem = LSA_TRUE;
			}
		}
		else { /* i.e., RTClass3 advanced, decentral PDEV, within the pdev-device (see CO-problem) */

			CM_AR_GRAPH_PTR_TYPE rtc3adv_argr = cm_sv_ar_lookup_another_rtc3adv(dev, argr);

			if( cm_is_null(rtc3adv_argr) ) { /* another AR with "RTClass3 advanced" does _not_ yet exist */

				/* this AR determines the IRDataUUID */
				/* if the current PDEV-IRDataUUID matches, the AR can be established */
				/* a reparameterization with the same IRDataUUID has no effect on the IOCRs of this AR */

				if( cm_uuid_equal(argr_ir_data_uuid, pdev_ir_data_uuid) ) {

					ir_problem = LSA_FALSE;
				}
				else if( cm_sv_ar_is_pdev_owner(argr) ) {

					ir_problem = LSA_FALSE; /* this PDEV-AR will parameterize the PDEV with the correct PDIRData */
				}
				else {

					ir_problem = LSA_TRUE; /* must wait until the corresponding PDEV-AR commits the PDEV */
				}
			}
			else if( cm_is_null(rtc3adv_argr->ir_info) ) {

				ir_problem = LSA_TRUE; /* actually unreachable */
			}
			else if( cm_uuid_equal(argr_ir_data_uuid, &rtc3adv_argr->ir_info->ir_data_uuid) ) { /* AR-IRDataUUID == existing AR-IRDataUUID */

				/* if the current PDEV-IRDataUUID matches, the AR can be established */
				/* a reparameterization with the same IRDataUUID has no effect on the IOCRs of this AR */

				if( cm_uuid_equal(argr_ir_data_uuid, pdev_ir_data_uuid) ) {

					ir_problem = LSA_FALSE;
				}
				else if( cm_sv_ar_is_pdev_owner(argr) ) {

					ir_problem = LSA_FALSE; /* this PDEV-AR will parameterize the PDEV with the correct PDIRData */
				}
				else {

					ir_problem = LSA_TRUE; /* must wait until the corresponding PDEV-AR commits the PDEV */
				}
			}
			else { /* AR-IRDataUUID != existing AR-IRDataUUID */

				ir_problem = LSA_TRUE;
			}
		}
	}

	/***/

	return ir_problem;
}

/*===========================================================================*/
/*=== SENDCLOCK CHANGED =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_sendclock_changed(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 send_clock
) {
	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_SEND_CLOCK) == 0 ) {

		/* do nothing (see callers: for all ARs ...) */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_SEND_CLOCK;

		/***/

		if( send_clock == 0 ) {

			LSA_UINT32 pnio_err = CM_PNIO_ERR_SC_PROBLEM; /* cannot set send_clock */

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) could not set send_clock, pnio_err(0x%08x)"
				, argr->device_nr, argr->ar_nr
				, pnio_err
				);

			/***/

			if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

				CM_FATAL();
			}
			else {

				CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev; /* save, see cm_sv_ar_free() below */

				CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
				argr->cm.sv.curr_rpc_ind = LSA_NULL; /* unlink */

				cm_sv_ar_free(argr, pnio_err, CM_AR_REASON_FRAME);

				cm_sv_rpc_call_response(dev, rpc, CLRPC_OK, pnio_err, 0/*nice*/);
			}
		}
		else {

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_IPSUITE_LOCK;

			cm_oha_control_ip_suite(argr, OHA_LOCK_SET, argr->cm.sv.lock_rqb); /* see comment in cm_sv_ar_control_ip_suite_done() */

			/***/

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_AR_PROBLEM) != 0 ) {

				/* see cm_sv_ar_connect_indication_unparked()
				 * - CO-problem .......................... does not come here
				 * - SC-problem + legacy ................. does not come here
				 * - SC-problem + advanced ............... send-clock was not set: must not add IOCRs
				 * - PO-problem [only PDEV-AR] ........... SO-locked: PDEV "empty" instead of the AR's PDEV parameters
				 * - IR-problem [only RTC3 + advanced] ... no matching IRData: no received-in-red
				 */

				if( CM_SV_AR_IS_ADVANCED(argr) ) {

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_FRAMES_ADD;

					if( !CM_SV_AR_IS_RTCLASS3(argr) && (argr->cm.sv.ar_flags & CM_SV_AR_FL_PD_ATTACHED) != 0 ) {

						cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_ADD, 0);
					}
					else { /* do not add */

						/* note: operations on IOCRs will have no effect, see cm_arsm.c */

						cm_sv_ar_frames_add_done(argr); /* fake */
					}
				}
				else { /* legacy, must add */

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_FRAMES_ADD;

					cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_ADD, 0);
				}
			}
			else {

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_FRAMES_ADD;

				cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_ADD, 0);
			}
		}
	}
}

/*===========================================================================*/
/*=== FRAMES ADD DONE =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_frames_add_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_FRAMES_ADD) == 0 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_FRAMES_ADD;

		/***/

		if( argr->cm.sv.r1_flags != 0 ) {

			if(
				(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) == 0
				|| (argr->cm.sv.r1_flags & (CM_SV_AR_R1FL_WF_CONN_POSTED | CM_SV_AR_R1FL_EX_CONN_DONE)) != 0
			) {

				CM_FATAL(); /* a bug */
			}
			else {

				if(
					cm_ar_com_is_down(argr)
					|| argr->cm.sv.dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHED /* TIA 1737437 */
				) {

					cm_sv_ar_connect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_FRAMES_ADD (R1, down or bridge-broken) */
				}
				else {

					/* note: posting "connect" is done here because of the case frame-add.cnf(-) */
					/* note: alarm-open and rpc-rebind do not fail if everything is configured correctly */

					cm_sv_r1b_post_connect(argr);
				}
			}
		}
		else { /* non R1 */

			cm_sv_ar_connect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_FRAMES_ADD (non R1) */
		}
	}
}

/*===========================================================================*/
/*=== CONTROL IP-SUITE (LOCK/UNLOCK) DONE ===================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_control_ip_suite_done(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_OHA_LOWER_RQB_PTR_TYPE oha
) {
	CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup1(channel, device_nr, ar_nr);

	/*
	 * CM V4.1i1.4, Release Note: ARs will not be accepted if the local IP-address is conflicting with another station or it is an "emergency IP-Address"
	 *
	 * AP01271744 (lock-and-read IP-suite) supersedes AP00685671, AP01189191 and AP01271668
	 *
	 * OHA_LOCK_SET locks the IP-suite of the interface of this CM instance (and returns the locked IP-suite)
	 * if there is only one interface this solves the problem
	 * if there is more than one interface a solution to the problem does not exist
	 * - it is not possible to derive the interface from the caller's IP-address
	 *   because the call could come in via a router (i.e., not matching any local subnet)
	 *   or because some interfaces have temporarily the same IP-suite
	 * - locking an interface other than the interface of this CM instance is not feasible
	 *   because it would interfere with the shutdown of the other interface
	 *
	 * luckily the "cross case" has only the usecase: "IOSAR with device access" (easy-supervisor)
	 * an IO-AR will not and cannot be established because the RT frames will not find their way
	 */

	if(
		cm_is_null(argr)
		|| (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_IPSUITE_LOCK) == 0
		|| oha != argr->cm.sv.lock_rqb
	) {

		CM_FATAL();
	}
	else {

		LSA_UINT16 oha_rsp = CM_OHA_GET_RESPONSE(oha);
		OHA_UPPER_DCP_CONTROL_PTR_TYPE oha_param = &oha->args.DcpControl;

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_IPSUITE_LOCK;

		CM_OHA_SET_OPCODE(oha, 0/*unused*/); /* note: accessing oha->args after this line (which is ok) */

		/***/

		if( oha_param->Mode == OHA_LOCK_SET ) {

			if( oha_rsp != OHA_OK ) {

				CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev/ar(%u/%u) could not lock, oha_rsp(%u)"
					, argr->device_nr, argr->ar_nr
					, oha_rsp
					);

				argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_IS_IPSUITE_LOCKED; /* sanity */

				cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_IPSUITE, 0);
				cm_ar_com_down(argr, CM_AR_REASON_IPSUITE);
			}
			else {

				OHA_UPPER_ADDR_INFO_PTR_TYPE info = oha_param->pAddrInfo;
				LSA_UINT32 ip_addr = CM_GET32_NTOH(info->CurrentIPSuite.IpAddr, 0);

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_IS_IPSUITE_LOCKED;

				if(
					ip_addr == 0 /* reset in the meantime */
					|| (info->CurrentIPSuiteIsEmergency == OHA_CURRENT_IPSUITE_IS_EMERGENCY_YES ? LSA_TRUE : LSA_FALSE)
					|| (info->CurrentIPSuiteConflictDetected == OHA_CURRENT_IPSUITE_CONFLICT_DETECTED_YES ? LSA_TRUE : LSA_FALSE)
				) {

					CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "dev/ar(%u/%u) ip_addr(%08x) is_emergency(%u) is_conflicting(%u)"
						, argr->device_nr, argr->ar_nr
						, ip_addr, info->CurrentIPSuiteIsEmergency, info->CurrentIPSuiteConflictDetected
						);

					cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_IPSUITE, 0);
					cm_ar_com_down(argr, CM_AR_REASON_IPSUITE);
				}
			}

			cm_sv_ar_connect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_IPSUITE_LOCK (lock) */
		}
		else { /* unlock */

			if( oha_rsp != OHA_OK ) {

				CM_FATAL();
			}
			else {

				CM_ASSERT((argr->cm.sv.cn_flags & CM_SV_AR_CNFL_IS_IPSUITE_LOCKED) == 0); /* flag is reset before OHA_UNLOCK_SET */

				if( cm_ar_com_is_down(argr) ) {

					cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_IPSUITE_LOCK (unlock) */
				}
				else {

					CM_FATAL();
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== AR CONNECT-INDICATION JOIN ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_connect_indication_join(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_FRAMES_ADD | CM_SV_AR_CNFL_WF_IPSUITE_LOCK)) != 0 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) frames-add or ip-suite-lock pending, cn_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags
			);
	}
	else if( (argr->cm.sv.r1_flags & (CM_SV_AR_R1FL_WF_CONN_POSTED | CM_SV_AR_R1FL_EX_CONN_DONE)) != 0 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) connect-posted or expect-connect-done pending, r1_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.r1_flags
			);
	}
	else { /* joined */

		if( cm_ar_com_is_down(argr) ) {

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_EARLY_ABORT;
		}

		/***/

		if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_JOIN_CONN | CM_SV_AR_CNFL_WF_CONN_RSP | CM_SV_AR_CNFL_WF_REBIND | CM_SV_AR_CNFL_WF_ALARM_ADD)) != 0 ) {

			CM_FATAL(); /* a bug */
		}
		else {

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_JOIN_CONN; /* lock-out synchronous callbacks */

			/***/

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_EARLY_ABORT) == 0 ) {

				CM_UPPER_RQB_PTR_TYPE event = cm_sv_ar_connect_build_notnull(argr);

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONN_RSP;

				cm_callback(argr->ar_com.channel, CM_OK, &event);
			}

			/***/

			if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

				/* doesn't have a clrpc-client allocated, see cm_sv_ar_alloc() */

				/* doesn't have an alarm-cr */
			}
			else {

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_REBIND;

				cm_sv_ar_rebind(argr, argr->cm.sv.rebind_rqb);

				/***/

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_ALARM_ADD;

				cm_arcq_request(argr, CM_ARCQ_OPC_ALARM_ADD, 0);
			}

			/***/

			argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_JOIN_CONN;

			cm_sv_ar_ownership_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_JOIN_CONN */
		}
	}
}

/*===========================================================================*/
/*=== AR CONNECT RESPONSE (OPCODE) ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_connect_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev/ar(%u/%u) rsp(%u)"
		, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
		, CM_RQB_GET_RESPONSE(rb)
		);

	/***/

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_CONN_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else {

		LSA_UINT16 rsp = CM_RQB_GET_RESPONSE(rb);

		if( rsp == CM_OK ) { /* user-supplied parameters are valid */

			if( ! cm_sv_ar_connect_update(argr, param->u.sv.ar_connect) ) {

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "dev/ar(%u/%u) cannot cm_sv_ar_connect_update"
					, argr->device_nr, argr->ar_nr
					);

				CM_FATAL(); /* bug or cm-user fiddled with the RQB */
			}

			/***/

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_EXP_USER_REQ; /* expect user-requests after ar-connect.rsp (user saw the conn.ind) */
		}
		else if( rsp == CM_ERR_RESOURCE ) { /* user-supplied parameters are invalid */

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_EARLY_ABORT;

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_ABORT, 0);
			cm_ar_com_down(argr, CM_AR_REASON_ABORT);
		}
		else if( rsp == CM_ERR_ARVENDORBLOCK ) { /* user could not decode ARVendorBlockReq (V5.2i3, AP01354956) */

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_EARLY_ABORT;

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
			cm_ar_com_down(argr, CM_AR_REASON_PROT); /* see cm_sv_rpc_connect_response_join() */
		}
		else if( rsp == CM_ERR_APPL_RESOURCE ) { /* AR parameters violate GSD MaxApplicationXXXLength (V7.0i2.10, TIA 2414553) */

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_EARLY_ABORT;

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_CONNECT/*!*/, 0);
			cm_ar_com_down(argr, CM_AR_REASON_1); /* tricky: see cm_sv_rpc_connect_response_join() */
		}
		else { /* undefined */

			CM_FATAL(); /* fatal because not as documented */
		}

		/***/

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_CONN_RSP; /* reset after possible call to cm_ar_com_down(), see above */

		/***/

		param->u.sv.ar_connect = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		cm_sv_ar_ownership_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_CONN_RSP */
	}
}

/*===========================================================================*/
/*=== AR REBIND =============================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_rebind(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) rebind to host_ip(%08X)"
		, argr->device_nr, argr->ar_nr
		, CM_NTOH32(argr->ar.host_ip)
		);

	if( cm_is_null(rpc) || CM_RPC_GET_OPCODE(rpc) != 0/*unused*/ ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CLRPC_UPPER_CL_BIND_PTR_TYPE param = rpc->args.cl.bind;

		param->client_id = cm_sv_get_rpc_client_id(argr);

		param->host_ip  = argr->ar.host_ip;
		param->udp_port = 0; /* NOTE: this is not the port from the call-indication! */

		cm_rpc_get_IPNIO_Controller(
			&param->if_uuid,
			&param->if_version_major,
			&param->if_version_minor,
			LSA_NULL, LSA_NULL
			);

		param->object_uuid = argr->ar.cmi_obj_uuid;

		cm_rpc_request_lower(argr->ar_com.channel, rpc, CLRPC_OPC_CL_REBIND, argr->device_nr, argr->ar_nr);
	}
}

/*===========================================================================*/
/*=== RPC REBIND DONE =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_rebind_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) rebind-done"
		, argr->device_nr, argr->ar_nr
		);

	if(
		(argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_REBIND) == 0
		|| rpc != argr->cm.sv.rebind_rqb
		|| CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK /* note: rebind does not fail, see CLRPC */
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_REBIND;

		CM_RPC_SET_OPCODE(rpc, 0/*unused*/);

		cm_sv_ar_ownership_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_REBIND */
	}
}

/*===========================================================================*/
/*=== ALARM ADD DONE =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_alarm_add_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_ALARM_ADD) == 0 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_ALARM_ADD;

		/***/

		cm_sv_ar_ownership_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_ALARM_ADD */
	}
}

/*===========================================================================*/
/*=== AR OWNERSHIP INDICATION JOIN ==========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_ownership_indication_join(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_JOIN_CONN | CM_SV_AR_CNFL_WF_CONN_RSP | CM_SV_AR_CNFL_WF_REBIND | CM_SV_AR_CNFL_WF_ALARM_ADD)) != 0 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) join-conn or conn.rsp or rebind or alarm-add pending, cn_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags
			);
	}
	else { /* joined */

		if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_JOIN_OSHP | CM_SV_AR_CNFL_WF_OWNERSHIP_RSP | CM_SV_AR_CNFL_WF_PROV_ACT)) != 0 ) {

			CM_FATAL();
		}
		else {

			argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_JOIN_OSHP; /* lock-out synchronous callbacks */

			/***/

			if( cm_ar_com_is_down(argr) ) { /* early-abort or ar-abort.req or frames-add.cnf(-) */

				/* no ownership.ind and do not activate the providers */
			}
			else if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

				/* no ownership.ind and no providers to activate */
			}
			else {

				{
				CM_UPPER_RQB_PTR_TYPE event = cm_sv_ar_ownership_build(argr, CM_SV_AR_CONTEXT_CONN);

				if( cm_is_not_null(event) ) { /* always true for connect-context */

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_OWNERSHIP_RSP;

					cm_callback(argr->ar_com.channel, CM_OK, &event);
				}
				}

				/***/

				if( CM_SV_AR_IS_RTCLASS3(argr) ) {

					if( CM_SV_AR_IS_ADVANCED(argr) ) {

						/* CM_ARCQ_OPC_PROV_IOPARAMS_SET must be done on cm_sv_pd_prm_end_confirmation() because EDDI needs the FCW */
						/* and the buffer-exchange will work at the time of ArRir.IND */
					}
					else {

						argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PROV_ACT;
						cm_arcq_request(argr, CM_ARCQ_OPC_PROV_ACTIVATE_AUX, 0); /* note: without config-provider-autostop */
					}
				}
				else {

					/* AP01256809: RTClass 1/2/UDP advanced mode, activate provider and consumer early */

					argr->cm.sv.ar_flags |= CM_SV_AR_FL_DO_CONS_12UDP;

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_PROV_ACT;
					cm_arcq_request(argr, CM_ARCQ_OPC_PROV_ACTIVATE, 0); /* note: with config-provider-autostop */
				}
			}

			/***/

			argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_JOIN_OSHP;

			cm_sv_rpc_connect_response_join(argr); /* trigger: CM_SV_AR_CNFL_WF_JOIN_OSHP */
		}
	}
}

/*===========================================================================*/
/*=== AR OWNERSHIP RESPONSE (OPCODE) ========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_ownership_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	/***/

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_OWNERSHIP_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else {

		LSA_UINT16 idx;

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u)"
			, argr->device_nr, argr->ar_nr
			);

		/* param check */
		if (param->u.sv.ar_ownership->set_mrp_off != CM_SET_MRP_OFF_NO && param->u.sv.ar_ownership->set_mrp_off != CM_SET_MRP_OFF_YES) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "set_mrp_off(%u) is out of range"
				, param->u.sv.ar_ownership->set_mrp_off
			);

			CM_FATAL(); /* bug of cm-user */
		}

		for (idx = 0; idx < param->u.sv.ar_ownership->nr_of_elems; ++idx) {

			CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE elem = &param->u.sv.ar_ownership->elem[idx];

			if (elem->is_wrong != CM_SV_IS_WRONG_NO && elem->is_wrong != CM_SV_IS_WRONG_YES) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "is_wrong(%u) is out of range"
					, elem->is_wrong
				);

				CM_FATAL(); /* bug of cm-user */
			}

		}

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_OWNERSHIP_RSP;

		/***/

		switch( param->u.sv.ar_ownership->ar_context ) {

		case CM_SV_AR_CONTEXT_CONN:
			cm_sv_ar_ownership_conn_response(argr, rb);
			break;

		case CM_SV_AR_CONTEXT_DISC:
			rb->args.sv.ar_event->u.sv.ar_ownership = LSA_NULL; /* sanity */
			cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);
			/***/
			cm_sv_ar_ownership_disc_response(argr);
			break;

		case CM_SV_AR_CONTEXT_PLUG:
			cm_sv_ar_ownership_plug_response(argr, rb);
			break;

		case CM_SV_AR_CONTEXT_PULLPDEV:
			cm_sv_ar_ownership_pullpdev_response(argr, rb);
			break;

		default:
			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
			break;
		}

		/***/

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_DO_OWNERSHIP_DISC) != 0 ) {

			argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_DO_OWNERSHIP_DISC;

			cm_sv_ar_ownership_disc_indication(argr);
		}
	}
}

/*===========================================================================*/
/*=== AR OWNERSHIP RESPONSE (CONNECT) =======================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_ownership_conn_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership = rb->args.sv.ar_event->u.sv.ar_ownership;

	/***/

	if( ar_ownership->nr_of_elems != argr->cm.sv.nr_of_submodules ) {

		CM_FATAL();
	}
	else {

		LSA_BOOL do_action = LSA_FALSE;
		LSA_BOOL is_pdev_locked = LSA_FALSE;
		LSA_UINT16 idx;

		if( argr->cm.sv.r1_flags != 0 && ar_ownership->set_mrp_off == CM_SET_MRP_OFF_YES) { /* TIA 1434352, see cm_sv_pd_prm_end_remote_request() */

			CM_FATAL(); /* user-bug (see documentation, "set_mrp_off" not supported for R1-ARs) */
		}

		argr->cm.sv.set_mrp_off = ar_ownership->set_mrp_off == CM_SET_MRP_OFF_YES ? LSA_TRUE : LSA_FALSE; /* AP00824058 */

		/***/

		if( argr->cm.sv.nr_of_pdev_submodules != 0 ) { /* TIA 1449259 */

			is_pdev_locked = cm_sv_ar_ownership_is_pdev_locked(argr->cm.sv.dev, ar_ownership);
		}

		/***/

		for( idx = 0; idx < ar_ownership->nr_of_elems; ++idx ) {

			CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE elem = &ar_ownership->elem[idx];

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			cm_ar_graph_find_api_slot_subslot(argr, elem->api, elem->slot_nr, elem->subslot_nr, LSA_NULL, &ar_mod, &ar_sub);

			if( cm_is_null(ar_sub) ) {

				CM_FATAL(); /* a user-bug */
			}
			else {

				if( argr->cm.sv.nr_of_pdev_submodules != 0 ) { /* TIA 1449259 */

					if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, elem->subslot_nr) ) {

						if( elem->owner_session_key != 0 && is_pdev_locked ) {

							CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
								, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) owner_session_key not zero"
								, argr->device_nr, argr->ar_nr, elem->api, elem->slot_nr, elem->subslot_nr
								);

							CM_FATAL(); /* a user-bug */
						}
					}
				}

				/***/

				if( cm_sv_get_submodule_state_ident(ar_mod, ar_sub) != CM_SUB_STATE_IDENT_WRONG ) {

					elem->is_wrong = CM_SV_IS_WRONG_NO; /* see "evaluated only ..." in the documentation (AP01278993) */
				}

				/***/

				{
				LSA_BOOL aplrdy_cancelled = cm_sv_ar_submodule_state_reset(ar_sub, LSA_FALSE/*clean*/, (elem->is_wrong == CM_SV_IS_WRONG_YES ? LSA_TRUE : LSA_FALSE)); /* clean! user gave information for all submodules */

				if( aplrdy_cancelled ) {

					CM_FATAL(); /* a bug */
				}
				}

				/***/

				{
				LSA_UINT16 prev_ar_nr = cm_sv_ownership_action(argr, ar_sub->sv.real_su, elem->owner_session_key);

				CM_SV_TRACE_09(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) OSHP-RSP: assigned owner_ar_nr(%u) elem_owner_ar_nr(%u) is_wrong(%u) prev_ar_nr(%u)"
					, argr->device_nr, argr->ar_nr
					, elem->api, elem->slot_nr, elem->subslot_nr
					, ar_sub->sv.real_su->owner_ar_nr, CM_SV_SESSION_KEY_TO_AR_NR(elem->owner_session_key)
					, elem->is_wrong
					, prev_ar_nr
					);

				if( prev_ar_nr != 0 ) {

					CM_AR_GRAPH_PTR_TYPE prev_argr = cm_sv_ar_lookup2(argr->cm.sv.dev, prev_ar_nr);

					if( cm_is_not_null(prev_argr) ) { /* always true */

						cm_sv_ar_submodule_state_changed(prev_argr, elem->api, elem->slot_nr, elem->subslot_nr); /* supervisor takeover */

						do_action = LSA_TRUE; /* do actions afterwards */
					}
				}
				}
			}
		}

		/***/

		rb->args.sv.ar_event->u.sv.ar_ownership = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		if( do_action ) {

			CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
			LSA_UINT16 ar_nr;

			for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

				CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

				if( cm_is_not_null(temp_argr) ) {

					if( temp_argr != argr ) { /* not this */

						cm_sv_ar_problem_indicator_eval(temp_argr); /* trigger: ar-ownership.rsp (takeover) */

						cm_sv_ar_submodule_state_action(temp_argr); /* trigger: ar-ownership.rsp (takeover) */
					}
				}
			}
		}

		/***/

		cm_sv_rpc_connect_response_join(argr); /* trigger: CM_SV_AR_CNFL_WF_OWNERSHIP_RSP (conn) */
	}
}

/*===========================================================================*/
/*=== AR OWNERSHIP RESPONSE (PLUG) ==========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_ownership_plug_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership = rb->args.sv.ar_event->u.sv.ar_ownership;

	/***/

	if( ar_ownership->nr_of_elems > argr->cm.sv.nr_of_submodules ) {

		CM_FATAL();
	}
	else {

		LSA_UINT16 idx;

		for( idx = 0; idx < ar_ownership->nr_of_elems; ++idx ) {

			CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE elem = &ar_ownership->elem[idx];

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			cm_ar_graph_find_api_slot_subslot(argr, elem->api, elem->slot_nr, elem->subslot_nr, LSA_NULL, &ar_mod, &ar_sub);

			if( cm_is_null(ar_sub) ) {

				CM_FATAL();
			}
			else {

				if( cm_sv_get_submodule_state_ident(ar_mod, ar_sub) != CM_SUB_STATE_IDENT_WRONG ) {

					elem->is_wrong = CM_SV_IS_WRONG_NO; /* see "evaluated only ..." in the documentation (AP01278993) */
				}

				/***/

				if( elem->is_wrong == CM_SV_IS_WRONG_YES ) {

					ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_WRONG;
				}
				else {

					ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_WRONG;
				}

				/***/

				{
				LSA_UINT16 prev_ar_nr = cm_sv_ownership_action(argr, ar_sub->sv.real_su, elem->owner_session_key);

				CM_SV_TRACE_09(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) OSHP-RSP: assigned owner_ar_nr(%u) elem_owner_ar_nr(%u) is_wrong(%u) prev_ar_nr(%u)"
					, argr->device_nr, argr->ar_nr
					, elem->api, elem->slot_nr, elem->subslot_nr
					, ar_sub->sv.real_su->owner_ar_nr, CM_SV_SESSION_KEY_TO_AR_NR(elem->owner_session_key)
					, elem->is_wrong
					, prev_ar_nr
					);

				if( prev_ar_nr != 0 ) {

					CM_FATAL(); /* a bug */
				}
				}
			}
		}

		/***/

		rb->args.sv.ar_event->u.sv.ar_ownership = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		cm_sv_ar_submodule_state_action(argr); /* trigger: ar-ownership.rsp */
	}

	/***/

	cm_sv_dev_led_info_eval(argr->cm.sv.dev); /* trigger: ownership.rsp/plug (owned -> so-locked) */
}

/*===========================================================================*/
/*=== AR OWNERSHIP RESPONSE (PULL PDEV) =====================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_ownership_pullpdev_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE rb
) {

	rb->args.sv.ar_event->u.sv.ar_ownership = LSA_NULL; /* sanity */

	cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

	/***/

	cm_sv_ar_submodule_state_action(argr); /* trigger: ar-ownership.rsp */
}

/*===========================================================================*/
/*=== AR OWNERSHIP ENSURE LOCKED STATE (PDEV) ===============================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_ownership_is_pdev_locked(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership
) {
	LSA_BOOL has_port = LSA_FALSE;
	LSA_UINT16 port_id;

	for( port_id = 0; port_id < dev->nr_of_pdev_mappings; port_id++ ) {

		LSA_UINT16 slot_nr;
		LSA_UINT16 subslot_nr;
		LSA_UINT32 mod_ident;
		LSA_UINT32 sub_ident;
		LSA_UINT8  im0_bits;
		LSA_UINT16 pdev_properties;

		LSA_BOOL port_exists = cm_pdsv_port_from_id(dev->channel, port_id, &slot_nr, &subslot_nr, &mod_ident, &sub_ident, &im0_bits, &pdev_properties);

		if( !port_exists || !CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			LSA_BOOL is_compact = CM_PDEV_TYPE_IS_COMPACT(pdev_properties);
			LSA_BOOL is_found   = LSA_FALSE;
			LSA_BOOL is_wrong   = LSA_FALSE;
			LSA_BOOL is_locked  = LSA_FALSE;

			LSA_UINT16 idx;

			for( idx = 0; idx < ar_ownership->nr_of_elems; ++idx ) {

				CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE elem = &ar_ownership->elem[idx];

				if( elem->api == 0 && elem->slot_nr == slot_nr && elem->subslot_nr == subslot_nr ) {

					is_found = LSA_TRUE;
					is_wrong = elem->is_wrong == CM_SV_IS_WRONG_YES ? LSA_TRUE : LSA_FALSE;
					is_locked = elem->owner_session_key == 0 ? LSA_TRUE : LSA_FALSE;
					break;
				}
			}

			if( is_compact ) {

				if( !is_found ) {

					return LSA_TRUE;
				}

				if( is_wrong ) {

					return LSA_TRUE;
				}

				if( is_locked ) {

					return LSA_TRUE;
				}
			}

			if( is_found && CM_SV_SUBSLOT_IS_PDEV_PORT(subslot_nr) ) {

				has_port = LSA_TRUE;
			}
		}
	}

	if( !has_port ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== PROVIDER ACTIVATE DONE ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_prov_activate_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PROV_ACT) == 0 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_PROV_ACT;

		/***/

		if( CM_SV_AR_IS_RTCLASS3(argr) ) {

			if( CM_SV_AR_IS_ADVANCED(argr) ) {

				cm_sv_ar_almi_rtc3_action_done(argr);
			}
			else {

				if( cm_ar_com_is_down(argr) ) {

					cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PROV_ACT (RTClass 3 legacy, RED) */
				}
				else {

					cm_sv_rpc_appl_ready_request(argr); /* trigger: CM_SV_AR_CNFL_WF_PROV_ACT (RTClass 3 legacy, RED) */
				}
			}
		}
		else {

			cm_sv_rpc_connect_response_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PROV_ACT (RTClass 1/2/UDP) */
		}
	}
}

/*===========================================================================*/
/*=== AUX-PROVIDER ACTIVATE DONE ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_prov_activate_aux_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	/* RTC3 legacy */

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PROV_ACT) == 0 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_PROV_ACT;

		/***/

		cm_sv_rpc_connect_response_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PROV_ACT (RTClass 3 legacy, AUX) */
	}
}

/*===========================================================================*/
/*=== PROVIDER SET IOPARAMS DONE ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_prov_ioparams_set_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	/* RTC3 advanced */

	if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PROV_ACT) == 0 ) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_PROV_ACT;

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_PROV_ACT (RTClass3 advanced, set-ioparams) */
		}
		else {

			cm_sv_rpc_appl_ready_request(argr); /* trigger: CM_SV_AR_CNFL_WF_PROV_ACT (RTClass3 advanced, set-ioparams) */
		}
	}
}

/*===========================================================================*/
/*=== RPC CONNECT RESPONSE JOIN =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_connect_response_join(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( (argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_JOIN_OSHP | CM_SV_AR_CNFL_WF_OWNERSHIP_RSP | CM_SV_AR_CNFL_WF_PROV_ACT)) != 0 ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev/ar(%u/%u) join-oshp or ownership.rsp or prov-act pending, cn_flags(0x%08x)"
			, argr->device_nr, argr->ar_nr
			, argr->cm.sv.cn_flags
			);
	}
	else { /* joined */

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_MASK) != 0 ) {

			CM_FATAL();
		}
		else {

			CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;

			/***/

			if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) { //R1 marker (special case representative-AR)

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_CONNECT_PHASE;  /* end of connect-phase (R1 representative-AR) */

				cm_ar_com_ready(argr); /* enable cm_sv_ar_com_down() indication */

				cm_sv_dev_led_info_eval(argr->cm.sv.dev); /* trigger: representative-AR */

				return;
			}

			/***/

			if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_CONNECT_PHASE; /* end of connect-phase (easy-supervisor) */
			}
			else {

				argr->cm.sv.ar_flags |= CM_SV_AR_FL_EXP_PRM_END; /* expect prm-end */

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_PRM_BACKUP; /* sanity */

				if( cm_is_not_null(argr->cm.sv.arset) ) { /* TIA 2329293 */

					if( ! cm_sv_arset_is_first(argr) ) {

						argr->cm.sv.ar_flags |= CM_SV_AR_FL_PRM_BACKUP;
					}
				}

				if( cm_sv_ar_is_pdev_owner(argr) ) {

					/* note: AR may be down here, see too "relinquish" */
					cm_sv_pd_prm_begin(argr, 0, 0); /* Connect */

					if(
						argr->cm.sv.r1_flags != 0
						&& argr->cm.sv.dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED
					) {

						cm_sv_pd_prm_begin_remote(argr, 0, 0); /* Connect */
					}
				}
			}

			/***/

			{
			CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;
			argr->cm.sv.curr_rpc_ind = LSA_NULL; /* unlink */

			if( cm_is_null(rpc) ) {

				CM_FATAL();
			}
			else {

				LSA_UINT32 ndr_length = CLRPC_PNIO_OFFSET;
				LSA_UINT32 pnio_err;

				/***/

				if( cm_ar_com_is_down(argr) ) {

					CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "argr(%08X) down(%u) when building rpc-connect.rsp"
						, argr
						, argr->ar_com.down
						);

					if( argr->ar_com.down == CM_AR_REASON_FRAME ) {

						/* assuming something wrong with the iocr-parameters which CM cannot check
						 *
						 * formerly we returned the undocumented CM_PNIO_FAULTY_IOCRBlockReq(254)
						 * since there is no better return-code, we say now 'something wrong with sequence'
						 *
						 * note: the field 'sequence' is 'not checked' in the pdu-checking rules, so we use
						 * it as a hint that one has to look into EDD to find out what was going wrong.
						 *
						 * note: CM_AR_REASON_FRAME is also used to signal that the send-clock could not be
						 * set in EDD. See cm_sv_ar_sendclock_changed(). However, this should not happen.
						 */

						pnio_err = CM_PNIO_FAULTY_IOCRBlockReq(13/*sequence*/);
					}
					else if( argr->ar_com.down == CM_AR_REASON_IPSUITE ) {

						pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_StateConflict(); /* AP01271744 */
					}
					else if( argr->ar_com.down == CM_AR_REASON_PROT ) {

						pnio_err = CM_PNIO_FAULTY_ARVendorBlockReq(6); /* AP01354956 */
					}
					else if( argr->ar_com.down == CM_AR_REASON_R1_CONN ) {

						argr->ar_com.down = CM_AR_REASON_CONNECT; /* note: exchanging reason for LogBook */
						pnio_err = argr->ar_com.down_pnio_status; //R1 VV: solve this differently
					}
					else if( argr->ar_com.down == CM_AR_REASON_1 ) { /* tricky: see cm_sv_opc_ar_connect_response(), TIA 2414553 */

						argr->ar_com.down = CM_AR_REASON_CONNECT; /* note: exchanging reason for LogBook */
						pnio_err = CM_PNIO_FAULTY_IOCRBlockReq(8);
					}
					else {

						/* possible case: ar-abort before connect.rsp
						 *
						 * changed with V4.1i2 (due to update of PNIO-Spec)
						 *   from "CMRPC / Out of AR Resources"
						 *   to "User / User abort"
						 *
						 * note: earlier version returned the undocumented CM_PNIO_FAULTY_ARBlockReq(254)
						 */

						pnio_err = CM_PNIO_FAULTY_Connect__UserSpecific_UserAbort();
					}
				}
				else {

					LSA_UINT32 len = cm_sv_build_connect_response(argr, rpc);

					if( len == 0 ) {

						CM_FATAL(); /* see "cres_length" in cm_sv_argr_build() */
					}
					else {

						ndr_length += len;
					}

					pnio_err = CM_PNIO_ERR_NONE;
				}

				/***/

				cm_sv_rpc_call_response(dev, rpc, CLRPC_OK, pnio_err, ndr_length);

				/***/

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_RETRIGGER_TIMER;

				cm_sv_ar_cmi_timer_trigger(argr); /* trigger: rpc-connect.rsp */

				/***/

				cm_ar_com_ready(argr); /* enable cm_sv_ar_com_down() and cm_sv_ar_new_data_indication() indications */

				/*
				if( AR was down before calling cm_ar_com_ready() ) { // e.g., because frame-add.cnf(-)
					then cm_ar_com_ready() did call cm_sv_ar_com_down()
				}
				// thus: don't call cm_sv_ar_disconnect_indication_join() here if down (argr may be gone)
				*/
			}
			}

			/***/

			cm_sv_dev_led_info_eval(dev); /* trigger: rpc-connect.rsp */
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
