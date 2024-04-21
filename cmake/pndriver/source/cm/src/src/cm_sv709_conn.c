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
/*  F i l e               &F: cm_sv709_conn.c                           :F&  */
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
/*  - connect, sub state                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7709
#define CM_MODULE_ID       7709

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== INIT R1B PART OF AR-GRAPH =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_r1b_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	argr->cm.sv.r1_flags = 0;

	argr->cm.sv.r1b.msg_connect = LSA_NULL;

	argr->cm.sv.r1b.msg_connect_done = LSA_NULL;

	argr->cm.sv.r1b.msg_sub_state = LSA_NULL;

	argr->cm.sv.r1b.msg_update_sub_state = LSA_NULL;

	argr->cm.sv.r1b.msg_disconnect = LSA_NULL;

	argr->cm.sv.r1b.msg_disconnect_done = LSA_NULL;

	/***/

	argr->cm.sv.r1b.remote_pdev_begin_rqb = LSA_NULL;

	argr->cm.sv.r1b.remote_pdev_end_rqb = LSA_NULL;

	argr->cm.sv.r1b.remote_prm_end_args = LSA_NULL;
}

/*===========================================================================*/
/*=== PRE-ALLOCATE R1B PART OF AR-GRAPH =====================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_r1b_prealloc(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_CHANNEL_CONST_PTR_TYPE channel = argr->ar_com.channel;

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

		argr->cm.sv.r1b.msg_sub_state = cm_sv_r1b_prefix_alloc(argr->cm.sv.dev, CM_SV_R1B_SUB_STATE_MESSAGE_MAX(argr));

		if( cm_is_null(argr->cm.sv.r1b.msg_sub_state) ) {

			return LSA_FALSE;
		}

		/***/

		argr->cm.sv.r1b.msg_update_sub_state = cm_sv_r1b_prefix_alloc(argr->cm.sv.dev, CM_SV_R1B_SUB_STATE_MESSAGE_MAX(argr));

		if( cm_is_null(argr->cm.sv.r1b.msg_update_sub_state) ) {

			return LSA_FALSE;
		}

		/***/

		argr->cm.sv.r1b.msg_disconnect = cm_sv_r1b_prefix_alloc(argr->cm.sv.dev, CM_SV_R1BMSG_SIZE_DISCONNECT);

		if( cm_is_null(argr->cm.sv.r1b.msg_disconnect) ) {

			return LSA_FALSE;
		}

		/***/

		{
		CM_UPPER_RQB_PTR_TYPE pd;

		pd = cm_upper_alloc_rqb(channel);

		if( cm_is_null(pd) ) {

			return LSA_FALSE;
		}

		CM_RQB_SET_OPCODE(pd, 0/*unused*/);

		argr->cm.sv.r1b.remote_pdev_begin_rqb = pd;

		/***/

		pd = cm_upper_alloc_rqb(channel);

		if( cm_is_null(pd) ) {

			return LSA_FALSE;
		}

		CM_RQB_SET_OPCODE(pd, 0/*unused*/);

		argr->cm.sv.r1b.remote_pdev_end_rqb = pd;
		}

		/***/

		{
		CM_PD_AR_PRM_END_63P_PTR_TYPE args = (CM_PD_AR_PRM_END_63P_PTR_TYPE)cm_sv_upper_mem_alloc(channel, sizeof(*args));

		if( cm_is_null(args) ) {

			return LSA_FALSE;
		}

		argr->cm.sv.r1b.remote_prm_end_args = args;
		}
	}

	/***/

	if ( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {

		argr->cm.sv.r1b.msg_connect_done = cm_sv_r1b_prefix_alloc(argr->cm.sv.dev, CM_SV_R1BMSG_SIZE_CONNECT_DONE);

		if( cm_is_null(argr->cm.sv.r1b.msg_connect_done) ) {

			return LSA_FALSE;
		}

		/***/

		argr->cm.sv.r1b.msg_disconnect_done = cm_sv_r1b_prefix_alloc(argr->cm.sv.dev, CM_SV_R1BMSG_SIZE_DISCONNECT_DONE);

		if( cm_is_null(argr->cm.sv.r1b.msg_disconnect_done) ) {

			return LSA_FALSE;
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== UNDO-INIT R1B PART OF AR-GRAPH ========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_r1b_undo_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;

	if( cm_is_not_null(argr->cm.sv.r1b.msg_connect) ) {
		cm_sv_r1b_prefix_free(channel, argr->cm.sv.r1b.msg_connect);
		argr->cm.sv.r1b.msg_connect = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.r1b.msg_connect_done) ) {
		cm_sv_r1b_prefix_free(channel, argr->cm.sv.r1b.msg_connect_done);
		argr->cm.sv.r1b.msg_connect_done = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.r1b.msg_sub_state) ) {
		cm_sv_r1b_prefix_free(channel, argr->cm.sv.r1b.msg_sub_state);
		argr->cm.sv.r1b.msg_sub_state = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.r1b.msg_update_sub_state) ) {
		cm_sv_r1b_prefix_free(channel, argr->cm.sv.r1b.msg_update_sub_state);
		argr->cm.sv.r1b.msg_update_sub_state = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.r1b.msg_disconnect) ) {
		cm_sv_r1b_prefix_free(channel, argr->cm.sv.r1b.msg_disconnect);
		argr->cm.sv.r1b.msg_disconnect = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.r1b.msg_disconnect_done) ) {
		cm_sv_r1b_prefix_free(channel, argr->cm.sv.r1b.msg_disconnect_done);
		argr->cm.sv.r1b.msg_disconnect_done = LSA_NULL;
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.r1b.remote_pdev_begin_rqb) ) {
		cm_upper_free_rqb(channel, argr->cm.sv.r1b.remote_pdev_begin_rqb);
		argr->cm.sv.r1b.remote_pdev_begin_rqb = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.r1b.remote_pdev_end_rqb) ) {
		cm_upper_free_rqb(channel, argr->cm.sv.r1b.remote_pdev_end_rqb);
		argr->cm.sv.r1b.remote_pdev_end_rqb = LSA_NULL;
	}

	if( cm_is_not_null(argr->cm.sv.r1b.remote_prm_end_args) ) {
		cm_sv_upper_mem_free(channel, argr->cm.sv.r1b.remote_prm_end_args);
		argr->cm.sv.r1b.remote_prm_end_args = LSA_NULL;
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POST CONNECT ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_connect(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT32 message_size = 0;

	(void)cm_sv_r1b_get_message_body_from_prefix(argr->cm.sv.r1b.msg_connect, &message_size); /* see cm_sv_rpc_connect_indication() */

	argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_CONN_POSTED;
	argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_EX_CONN_DONE;

	cm_sv_r1b_post2(argr, CM_SV_R1BRIDGE_MSG_CONNECT, message_size, argr->cm.sv.r1b.msg_connect);
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED CONNECT ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_connect(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
){
	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_CONN_POSTED) == 0
		|| prefix != argr->cm.sv.r1b.msg_connect
	) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_CONN_POSTED;

		cm_sv_ar_connect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_CONN_POSTED */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE CONNECT ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_connect(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be not-LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size,
	CLRPC_UUID_CONST_PTR_TYPE ar_uuid,
	LSA_BOOL is_update /* TRUE if CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT */
) {
	LSA_UINT32 pnio_err = CM_PNIO_ERR_NONE;

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev(%u) R1 handle connect (create representative-AR)"
		, dev->device_nr
		);

	/***/

	if( is_update ) {

		if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHING_PASSIVE ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) bridge_state(%u) not establishing-passive"
				, dev->device_nr, dev->r1b.bridge_state
				);

			CM_FATAL(); /* R1 protocol error */
		}
	}

	/***/

	if( message_size < CM_SV_R1BMSG_SIZE_CONNECT(0) ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		LSA_UINT16 alarm_ref   = CM_GET16_NTOH(message_body, 0);
		LSA_UINT32 nr_of_cons  = CM_GET16_NTOH(message_body, 2);
		LSA_UINT32 header_size = CM_SV_R1BMSG_SIZE_CONNECT(nr_of_cons);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_CONNECT(0) == 4U);

		if( message_size < header_size ) {

			CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) R1 protocol error (message_size(%u) vs. nr_of_cons(%u))"
				, dev->device_nr, message_size, nr_of_cons
				);

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			if( cm_is_not_null(argr) ) {

				/* do nothing here; case is handled in cm_sv_build_ioar(), search for "existing_argr" */
			}

			//cm_sv_rpc_connect_indication()
			//...alloc
			//...build

			argr = cm_sv_argr_alloc_and_init(dev);

			if( cm_is_null(argr) ) {

				CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev(%u) R1 could not cm_sv_argr_alloc_and_init()"
					, dev->device_nr
					);

				pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();
			}
			else {

				pnio_err = cm_sv_argr_build(argr, LSA_NULL, message_body + header_size, message_size - header_size);

				if( pnio_err != CM_PNIO_ERR_NONE ) {

					CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "dev(%u) R1 could not cm_sv_argr_build(), pnio_err(0x%x)"
						, dev->device_nr, pnio_err
						);
				}
				else {

					LSA_UINT32 offs;

					if( cm_is_not_null (argr->alarm_cr) ) { /* always true */
						argr->alarm_cr->local_alarm_ref = alarm_ref;
					}

					for( offs = 4; offs < header_size; offs += 4 ) {

						LSA_UINT16 iocr_ref  = CM_GET16_NTOH(message_body, offs + 0);
						LSA_UINT16 frame_id  = CM_GET16_NTOH(message_body, offs + 2);

						CM_AR_GRAPH_IO_CR_PTR_TYPE iocr = cm_ar_graph_find_io_cr(argr, iocr_ref);

						if( cm_is_not_null(iocr) ) {
							iocr->frame_id = frame_id;
						}
					}
				}
			}

			cm_callback(dev->channel, CM_OK, &rb); /* note: message_body no longer needed */

			/***/

			if( cm_is_not_null(argr) && pnio_err == CM_PNIO_ERR_NONE ) {

				//cm_sv_rpc_connect_indication()
				//... continuing after alloc+build
				//... attach to submodules
				//... preallocate memory

				if( ! cm_sv_ar_attach(argr) ) {

					cm_sv_ar_detach(argr); /* undo partial attach */

					pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();
				}
				else {

					if( ! cm_sv_argr_prealloc(argr) ) {

						pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();
					}
				}
			}

			/***/

			if( cm_is_not_null(argr) && pnio_err == CM_PNIO_ERR_NONE ) {

				//cm_sv_rpc_connect_indication()
				//... before cm_sv_ar_unpark()

				if(
					argr->ar_nr == 0
					|| cm_is_not_null(argr->cm.sv.curr_rpc_ind)
					|| (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_UNPARK) != 0
				) {
					CM_FATAL();
				}

				//cm_sv_ar_connect_indication_unparked()
				//...skipping determine sendclock
				//...skipping XY_problem() functions
				//...skipping attach to send-clock (asynchronous...)
				//note: RTF resets the send-clock but does not kill the representative-AR; thus set-send-clock on connect does not make sense.

				if( cm_sv_r1b_ownership_overlapping(argr) ) {

					pnio_err = CM_PNIO_FAULTY_Connect__CMRPC_ARSetStateConflictDuringConnectionEstablishment(); /* AR-set state conflict (overlapping ARs not supported)*/
				}
				else {

					cm_sv_ownership_queue_for(argr); /* ownership of representative-AR */

					//cm_sv_ar_sendclock_changed()
					//...skipped, see not-attached above
					//...skipped cm_oha_control_ip_suite()
					//...skipped CM_ARCQ_OPC_FRAMES_ADD

					//cm_sv_ar_connect_indication_join()
					//...skipped cm_sv_ar_rebind()
					//...skipped CM_ARCQ_OPC_ALARM_ADD

					if( cm_ar_com_is_down(argr) ) {
						CM_FATAL();
					}

					//cm_sv_ar_ownership_indication_join()
					//...skipped CM_ARCQ_OPC_PROV_ACTIVATE

					//cm_sv_rpc_connect_response_join()
					//...skipped cm_sv_build_connect_response()
					//...skipped cm_sv_rpc_call_response() -- see CONNECT_DONE message
					//...skipped cm_sv_ar_cmi_timer_trigger()
				}
			}

			/***/

			if( cm_is_not_null(argr) && pnio_err == CM_PNIO_ERR_NONE ) {

				if(
					argr->cm.sv.cn_flags != 0
					|| argr->cm.sv.ar_flags != (CM_SV_AR_FL_CONNECT_PHASE | CM_SV_AR_FL_QF_OWNERSHIP)
					|| argr->cm.sv.r1_flags != (CM_SV_AR_R1FL_REPRESENTATIVE | CM_SV_AR_R1FL_EX_DISCONN)
				) {

					CM_FATAL(); /* a bug */
				}
				else {

					if( is_update ) {

						/* no connect-done(+) message; success is implicit and failure is fatal, see below. */
					}
					else {

						CM_COMMON_MEM_U8_PTR_TYPE done_body = cm_sv_r1b_get_message_body_from_prefix(argr->cm.sv.r1b.msg_connect_done, LSA_NULL);
						LSA_UINT32 done_size = CM_SV_R1BMSG_SIZE_CONNECT_DONE;

						CM_PUT32_HTON(done_body, 0, CM_PNIO_ERR_NONE); /* success */

						CM_ASSERT(CM_SV_R1BMSG_SIZE_CONNECT_DONE == 4U);

						argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_CONN_DONE_POSTED;

						cm_sv_r1b_post2(argr, CM_SV_R1BRIDGE_MSG_CONNECT_DONE, done_size, argr->cm.sv.r1b.msg_connect_done);
					}

					cm_sv_rpc_connect_response_join(argr); /* trigger: CM_SV_R1BRIDGE_MSG_CONNECT and CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT */
				}
			}
			else { /* could not create representative-AR */

				if( cm_is_not_null(argr) ) {

					cm_sv_ar_free(argr, pnio_err, CM_AR_REASON_CONNECT);
				}

				/***/

				if( is_update ) {

					CM_FATAL(); /* must kill this side! original-AR is already running (i.e. after rpc-connect.rsp) and cannot handle this case. */
				}
				else {

					cm_sv_r1b_post_connect_done_error(dev, ar_uuid, pnio_err);
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== OWNERSHIP OVERLAPPING =================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_ownership_overlapping(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	// called if dev->R1_enabled
	// called before cm_sv_ownership_queue_for()
	// overlap of R1-AR with "single sided AR" is a problem if "single sided AR" comes first (the R1-AR on the other side does not overlap...)

	LSA_UINT16 sr_ar_nr = 0;

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

		if( arset->arr[0] == argr && cm_is_not_null(arset->arr[1]) ) {

			sr_ar_nr = arset->arr[1]->ar_nr;
		}

		if( arset->arr[1] == argr && cm_is_not_null(arset->arr[0]) ) {

			sr_ar_nr = arset->arr[0]->ar_nr;
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

				CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

				if( su->cand[0].ar_nr == 0 ) {

					/* ok, this AR will be the first candidate */
				}
				else if( sr_ar_nr != 0 ) {

					if( su->cand[0].ar_nr == sr_ar_nr ) {

						/* ok, other SR-AR is the first candidate, this AR will be the second candidate */
					}
					else {

						return LSA_TRUE; /* overlap */
					}
				}
				else {

					return LSA_TRUE; /* overlap */
				}
			}
		}
	}
	}

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED CONNECT-DONE =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_connect_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
){

	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_CONN_DONE_POSTED) == 0
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) == 0
		|| prefix != argr->cm.sv.r1b.msg_connect_done
	) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_CONN_DONE_POSTED;

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_CONN_DONE_POSTED */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE CONNECT-DONE =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_connect_done(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
){
	if(
		cm_is_null(argr)
		|| cm_is_null(argr->cm.sv.arset)
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) == 0
		|| message_size != CM_SV_R1BMSG_SIZE_CONNECT_DONE
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not ORIG or wrong message_size(%u))"
			, dev->device_nr, argr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else { /* argr found */

		LSA_UINT32 pnio_err = CM_GET32_NTOH(message_body, 0);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_CONNECT_DONE == 4U);

		cm_callback(dev->channel, CM_OK, &rb);

		/***/

		if(
			(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_CONN_DONE) == 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_DO_DISCONN) != 0
		) {

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev/ar(%u/%u) R1 protocol error (wrong state, ar_flags(0x%08x), r1_flags(0x%08x)"
				, argr->device_nr, argr->ar_nr
				, argr->cm.sv.ar_flags, argr->cm.sv.r1_flags
				);

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_EX_CONN_DONE;

			/***/

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) R1 handle connect-done, pnio_err(0x%08x)"
				, argr->device_nr, argr->ar_nr, pnio_err
				);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				if( ! cm_ar_com_is_down(argr) ) { /* see FirstAR-logic in cm_sv_build_ioar() */

					cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_CONNECT/*!*/, 0);
					//R1 VV: make cm_ar_com_down3() for the next 2 lines. see cm_sv_rpc_connect_response_join().
					cm_ar_com_down(argr, CM_AR_REASON_R1_CONN);
					argr->ar_com.down_pnio_status = pnio_err;
				}
			}
			else { /* representative-AR exists */

				argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_DO_DISCONN;
			}

			/***/

			cm_sv_ar_connect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_EX_CONN_DONE */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE CANCEL CONNECT-DONE =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_cancel_connect_done( /* called on bridge-broken */
	CM_AR_GRAPH_PTR_TYPE argr
) {
	/* if bridge-broken: cm_sv_r1b_handle_connect_done() will not come */

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_CONN_DONE) != 0 ) {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_EX_CONN_DONE;

		cm_sv_ar_connect_indication_join(argr); /* trigger: cancel CM_SV_AR_R1FL_EX_CONN_DONE */
	}
	else {

		/* do nothing */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POST SUBMODULE STATE ========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_prm_begin_end
) {

	if( argr->cm.sv.dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHED ) { /* TIA 1737437 */

		/* do nothing */
	}
	else {

		if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_SUBSTATE_POSTED) != 0 ) {

			CM_FATAL(); /* bug */
		}
		else {

			LSA_UINT32 message_max;
			CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(argr->cm.sv.r1b.msg_sub_state, &message_max);
			LSA_UINT32 message_size = cm_sv_r1b_build_sub_state_message(argr, message_body, message_max);
			LSA_UINT16 message_numb = is_prm_begin_end ? CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END : CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT;

			argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_SUBSTATE_POSTED;

			cm_sv_r1b_post2(argr, message_numb, message_size, argr->cm.sv.r1b.msg_sub_state);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED SUBMODULE STATE ======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_sub_state(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_SUBSTATE_POSTED) == 0
		|| prefix != argr->cm.sv.r1b.msg_sub_state
	) {

		CM_FATAL(); /* bug */
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_SUBSTATE_POSTED;

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_SUBSTATE_POSTED */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE SUB STATE ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_sub_state(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size,
	LSA_UINT16 message_numb /* CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT or CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE or CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END */
) {
	LSA_UINT16 sub_mod_count;
	LSA_UINT32 data_size;

	if(
		cm_is_null(argr)
		|| cm_is_null(argr->cm.sv.arset)
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) == 0
		|| message_size < CM_SV_R1BMSG_SIZE_SUB_STATE
	) {

		CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not REPR or wrong message_size(%u)), message_numb(%u)"
			, dev->device_nr, argr, message_size, message_numb
			);

		CM_FATAL(); /* R1 protocol error */
	}

	sub_mod_count = CM_GET16_NTOH(message_body, 0);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_SUB_STATE == 2U);

	/***/

	data_size = sub_mod_count * CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM;

	if( argr->cm.sv.nr_of_submodules != sub_mod_count ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) R1 protocol error (sub_mod_count wrong), message_numb(%u)"
			, argr->device_nr, argr->ar_nr, message_numb
			);

		CM_FATAL(); /* R1 protocol error */
	}

	if( message_size != CM_SV_R1BMSG_SIZE_SUB_STATE + data_size ) {

		CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) R1 protocol error (message_size(%u), data_size(%u), sub_mod_count(%u)), message_numb(%u)"
			, argr->device_nr, argr->ar_nr, message_size, data_size, sub_mod_count, message_numb
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		CM_COMMON_MEM_U8_PTR_TYPE sub_state = message_body + CM_SV_R1BMSG_SIZE_SUB_STATE;
		LSA_UINT32 offset;

		for( offset = 0; offset < data_size; offset += CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM ) {

			CM_COMMON_MEM_U8_PTR_TYPE elem = sub_state + offset;
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			LSA_UINT32 api        = CM_GET32_NTOH(elem, 0);
			LSA_UINT16 slot_nr    = CM_GET16_NTOH(elem, 4);
			LSA_UINT16 subslot_nr = CM_GET16_NTOH(elem, 6);
			LSA_UINT16 st_told    = CM_GET16_NTOH(elem, 8);
			LSA_UINT16 st_curr    = CM_GET16_NTOH(elem, 10);

			CM_ASSERT(CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM == 12U);

			cm_ar_graph_find_api_slot_subslot(argr, api, slot_nr, subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

			if( cm_is_null(ar_sub) ) {

				CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "dev/ar(%u/%u) R1 protocol error (ap/sl/su(%u/%u/%05u) not found), message_numb(%u)"
					, argr->device_nr, argr->ar_nr, api, slot_nr, subslot_nr, message_numb
					);

				CM_FATAL(); /* R1 protocol error */
			}
			else {

				CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

				switch( message_numb ) {

				case CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT: /* appl-ready in connect-phase of first-AR signals "physical" changes (TIA 1839430, TIA 1819800) */
				case CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE:
				case CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END:
					ar_sub->sv.st_told = st_told;
					ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_DIRTY;
					break;

				default:
					CM_FATAL();
					break;
				}

				ar_sub->sv.sm_flags &= ~(CM_SV_AR_SMFL_WRONG | CM_SV_AR_SMFL_IS_PRMED | CM_SV_AR_SMFL_IS_ARP);

				if( CM_SUB_STATE_IS_OWNED(st_curr) ) {

					if( su->owner_ar_nr == 0 ) {

						if( su->cand[0].ar_nr == argr->ar_nr ) {

							su->owner_ar_nr = argr->ar_nr;
						}
					}

					if( CM_SUB_STATE_IS_WRONG(st_curr) ) {

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_WRONG;
					}
					else { /* good */

						ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_PRMED;

						if( CM_SUB_STATE_GET_AR_INFO(st_curr) == CM_SUB_STATE_AR_INFO_APPL_READY_PENDING ) {

							ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_IS_ARP;
						}
					}
				}
				else { /* locked */

					if( su->owner_ar_nr == argr->ar_nr ) {

						su->owner_ar_nr = 0;
					}

					ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_WRONG;
				}

				if( (st_curr & CM_SUB_STATE_FORMAT_1) != 0 ) { /* TIA 2037122 */

					cm_sv_ar_submodule_state_reset(ar_sub, LSA_TRUE/*dirty*/, LSA_TRUE/*wrong*/);

					CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "dev/ar(%u/%u) received dirty from other head (ap/sl/su(%u/%u/%04x)) sm_flags(0x%04x) st_curr(0x%04x)"
						, argr->device_nr, argr->ar_nr, api, slot_nr, subslot_nr, ar_sub->sv.sm_flags, st_curr
						);
				}
			}
		}

		/***/

		cm_callback(dev->channel, CM_OK, &rb);

		cm_sv_dev_led_info_eval(dev); /* trigger: handle CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT, CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE and CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE BUILD SUBMODULE STATE MESSAGE ===============================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_r1b_build_sub_state_message(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_COMMON_MEM_U8_PTR_TYPE sub_state,
	LSA_UINT32 buffer_size
) {
	LSA_UINT16 arr_idx = 0;

	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	CM_PUT16_HTON(sub_state, 0, argr->cm.sv.nr_of_submodules);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_SUB_STATE == 2U);

	arr_idx += CM_SV_R1BMSG_SIZE_SUB_STATE;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_COMMON_MEM_U8_PTR_TYPE sub = (sub_state + arr_idx);
				LSA_UINT16 st_curr; /* note: must not use local codings in bridge messages */

				/* note that cm_sv_get_submodule_state() does not evaluate the IS_PRMED flag */
				/* see what cm_sv_r1b_handle_sub_state() does with the information */

				if( ar_sub->sv.real_su->owner_ar_nr != argr->ar_nr) { /* locked */

					st_curr = (CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED | CM_SUB_STATE_IDENT_WRONG);
				}
				else if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_PRMED) != 0 ) { /* meaning: not wrong */

					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_ARP) != 0 ) {

						st_curr = (CM_SUB_STATE_AR_INFO_APPL_READY_PENDING | CM_SUB_STATE_IDENT_OK);
					}
					else {

						st_curr = (CM_SUB_STATE_AR_INFO_OWNED | CM_SUB_STATE_IDENT_OK);
					}
				}
				else {

					st_curr = (CM_SUB_STATE_AR_INFO_OWNED | CM_SUB_STATE_IDENT_WRONG);
				}

				if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_DIRTY) != 0  ) { /* TIA 2037122 */

					CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "dev/ar(%u/%u) signal submodule dirty to other head (ap/sl/su(%u/%u/%04x)) sm_flags(0x%x) st_curr(0x%04x)"
						, argr->device_nr, argr->ar_nr, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr, ar_sub->sv.sm_flags, st_curr
						);

					st_curr |= CM_SUB_STATE_FORMAT_1; /* signal dirty state to other head */
				}

				CM_PUT32_HTON(sub,  0, ar_ap->api);
				CM_PUT16_HTON(sub,  4, ar_mod->slot_nr);
				CM_PUT16_HTON(sub,  6, ar_sub->subslot_nr);
				CM_PUT16_HTON(sub,  8, ar_sub->sv.st_told);
				CM_PUT16_HTON(sub, 10, st_curr);

				CM_ASSERT(CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM == 12U);

				arr_idx += CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM;
			}
		}
	}

	if( arr_idx > buffer_size ) {
		CM_FATAL(); /* bug */
	}

	return arr_idx;
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
