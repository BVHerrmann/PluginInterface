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
/*  F i l e               &F: cm_sv710_disc.c                           :F&  */
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
/*  - disconnect                                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7710
#define CM_MODULE_ID       7710

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== R1-BRIDGE POST DISCONNECT =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_disconnect(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	/* note: bridge is established, see cm_sv_r1b_cancel_disconnect_done() */

	if( (argr->cm.sv.r1_flags & (CM_SV_AR_R1FL_WF_DISCONN_POSTED | CM_SV_AR_R1FL_EX_DISCONN_DONE)) != 0 ) {

		CM_FATAL(); /* bug */
	}
	else {

		CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(argr->cm.sv.r1b.msg_disconnect, LSA_NULL);
		LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_DISCONNECT;

		CM_PUT8_HTON(message_body, 0, argr->ar_com.down);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_DISCONNECT == 1U);

		argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_DISCONN_POSTED;
		argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_EX_DISCONN_DONE;

		cm_sv_r1b_post2(argr, CM_SV_R1BRIDGE_MSG_DISCONNECT, message_size, argr->cm.sv.r1b.msg_disconnect);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED DISCONNECT ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_disconnect(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
){
	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_DISCONN_POSTED) == 0
		|| prefix != argr->cm.sv.r1b.msg_disconnect
	) {
		CM_FATAL();
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_DISCONN_POSTED;

		cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_DISCONN_POSTED */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE DISCONNECT ===========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_disconnect(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* may be LSA_NULL */
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if(
		cm_is_null(argr)
		|| cm_is_null(argr->cm.sv.arset)
		|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) == 0
		|| message_size != CM_SV_R1BMSG_SIZE_DISCONNECT
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not REPR or wrong message_size(%u))"
			, dev->device_nr, argr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else { /* argr found */

		LSA_UINT8 remote_reason = CM_GET8_NTOH(message_body, 0);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_DISCONNECT == 1U);

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev/ar(%u/%u) R1 handle disconnect, remote_reason(%u)"
			, argr->device_nr, argr->ar_nr, remote_reason
			);

		/***/

		if(
			(argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0
			|| cm_ar_com_is_down(argr)
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_DO_DISCONN_DONE) != 0
			|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_DISCONN) == 0
		) {

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev/ar(%u/%u) R1 protocol error (wrong state, ar_flags(0x%08x), r1_flags(0x%08x)"
				, argr->device_nr, argr->ar_nr
				, argr->cm.sv.ar_flags, argr->cm.sv.r1_flags
				);

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			cm_callback(dev->channel, CM_OK, &rb); /* call back first, then abort */

			/***/

			LSA_UNUSED_ARG(remote_reason); /* note: used only in trace above! */

			argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_DO_DISCONN_DONE; /* note: bridge is currently established */

			argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_EX_DISCONN;

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_R1_DISC, 0);
			cm_ar_com_down(argr, CM_AR_REASON_R1_DISC); /* note: by design not "remote_reason" (could be zero); representative-AR does not cm_sv_logbook_write() */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE CANCEL DISCONNECT-DONE ======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_cancel_disconnect_done( /* called on bridge-broken */
	CM_AR_GRAPH_PTR_TYPE argr
) {
	/* if bridge-broken: cm_sv_r1b_handle_disconnect_done() will not come */

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_DO_DISCONN) != 0 ) {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_DO_DISCONN;
	}

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_DO_DISCONN_DONE) != 0 ) {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_DO_DISCONN_DONE; /* sanity (unreachable per Nov. 2015) */
	}

	/***/

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_DISCONN_DONE) != 0) { /* note: the AR is down */

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_EX_DISCONN_DONE;

		cm_sv_ar_disconnect_indication_join(argr); /* trigger: cancel CM_SV_AR_R1FL_EX_DISCONN_DONE */
	}

	/***/

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_DISCONN) != 0 ) {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_EX_DISCONN;

		CM_ASSERT((argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0);

		cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_R1_BRIDGE, 0);
		cm_ar_com_down(argr, CM_AR_REASON_R1_BRIDGE);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED DISCONNECT DONE ======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_disconnect_done(
	CM_AR_GRAPH_PTR_TYPE argr
) {

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_DISCONN_DONE_POSTED) != 0 ) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_DISCONN_DONE_POSTED;

		if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_DO_DISCONN_DONE) != 0 ) {

			LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_DISCONNECT_DONE;

			CM_ASSERT(CM_SV_R1BMSG_SIZE_DISCONNECT_DONE == 0U); /* no parameters */

			cm_sv_r1b_post2(argr, CM_SV_R1BRIDGE_MSG_DISCONNECT_DONE, message_size, argr->cm.sv.r1b.msg_disconnect_done);
		}
		else {

			cm_sv_r1b_posted_disconnect_done(argr, argr->cm.sv.r1b.msg_disconnect_done); /* fake */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED DISCONNECT-DONE ======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_disconnect_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_DISCONN_DONE_POSTED) == 0
		|| (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_FRAMES_RMV) != 0
		|| prefix != argr->cm.sv.r1b.msg_disconnect_done
	) {

		CM_FATAL();
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_DISCONN_DONE_POSTED;

		/***/

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_FRAMES_RMV;

		cm_sv_frames_remove_done(argr); /* note: afterwards the ar-graph is gone! */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE DISCONNECT-DONE ======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_disconnect_done(
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
		|| message_size != CM_SV_R1BMSG_SIZE_DISCONNECT_DONE
	) {

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (argr(0x%x) not found or not ORIG or wrong message_size(%u))"
			, dev->device_nr, argr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else { /* argr found */

		LSA_UNUSED_ARG(message_body); /* no parameters */

		CM_ASSERT(CM_SV_R1BMSG_SIZE_DISCONNECT_DONE == 0U);

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev/ar(%u/%u) R1 handle disconnect-done"
			, argr->device_nr, argr->ar_nr
			);

		/***/

		if(
			(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_DISCONN_DONE) == 0
			|| ! cm_ar_com_is_down(argr)
		) {

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev/ar(%u/%u) R1 protocol error (wrong state, ar_flags(0x%08x), r1_flags(0x%08x)"
				, argr->device_nr, argr->ar_nr
				, argr->cm.sv.ar_flags, argr->cm.sv.r1_flags
				);

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			cm_callback(dev->channel, CM_OK, &rb);

			/***/

			argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_EX_DISCONN_DONE;

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_EX_DISCONN_DONE */
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
