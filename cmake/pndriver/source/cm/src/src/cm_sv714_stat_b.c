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
/*  F i l e               &F: cm_sv714_stat_b.c                         :F&  */
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
/*  - apdu-status side "B"                                                   */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	7714
#define CM_MODULE_ID		7714

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== R1-BRIDGE STAT-B HANDLE ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_B_handle(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_ASSERT(CM_RQB_GET_OPCODE(rb) == CM_OPC_SV_R1BRIDGE_RECEIVED);

	if( cm_is_not_null(dev->r1b.stat_B.br_next) ) {

		CM_FATAL(); /* R1 protocol error ("stat_A" did not wait) */
	}
	else {

		dev->r1b.stat_B.br_next = rb; /* append to "queue" */

		cm_sv_r1b_stat_B_next(dev); /* trigger: handle */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-B NEXT =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_B_next(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( dev->r1b.stat_B.wf_posted ) {

		/* wait for "posted" */
	}
	else if( cm_is_null(dev->r1b.stat_B.br_next) ) {

		/* idle, nothing to do */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE rb = dev->r1b.stat_B.br_next;
		dev->r1b.stat_B.br_next = LSA_NULL;

		{
		CM_UPPER_SV_R1BRIDGE_RECEIVED_PTR_TYPE param = rb->args.sv.r1bridge_received; /* note: parameter were checked, see cm_sv_opc_r1bridge_received() */
		CM_COMMON_MEM_U8_PTR_TYPE header = (CM_COMMON_MEM_U8_PTR_TYPE)param->message_data;
		CM_COMMON_MEM_U8_PTR_TYPE message_body = header + CM_SV_R1BMSG_SIZE_HEADER;
		LSA_UINT32 message_size = CM_GET32_NTOH(header, 0);

		if( message_size != CM_SV_R1BMSG_SIZE_GET_APDU_STATUS ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) R1 protocol error (wrong message_size(%u))"
				, dev->device_nr, message_size
				);

			CM_FATAL(); /* R1 protocol error */
		}
		else {

			LSA_UINT16 rc;
			LSA_UINT16 iocr_ref;
			CLRPC_UUID_TYPE ar_uuid;

			iocr_ref = CM_GET16_NTOH(  message_body, 0);
			CM_GET_UUID_NTOH(&ar_uuid, message_body, 2);

			CM_ASSERT(CM_SV_R1BMSG_SIZE_GET_APDU_STATUS == 18U);

			{
			CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup_by_aruuid(dev, &ar_uuid);

			if(
				cm_is_null(argr)
				|| (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) == 0
			) {

				CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "argr(0x%x) not found or not ORIG", argr
					);

				rc = CM_OK_CANCELLED;
			}
			else {

				CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr = cm_ar_graph_find_io_cr(argr, iocr_ref);

				if( cm_is_null(iocr) ) {

					CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "iocr_ref(%u) not found", iocr_ref
						);

					rc = CM_OK_CANCELLED;
				}
				else {

					rc = cm_acp_frame_get_apdu_status(dev->channel, iocr, dev->r1b.stat_B.iter);
				}
			}
			}

			if( rc != CM_OK_PENDING ) {

				cm_sv_r1b_stat_B_post(dev, 0, 0, 0); /* R1 stat-B next: error */
			}
			else {

				/* see cm_sv_frame_get_apdu_status_done() */
			}
		}
		}

		/***/

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-B POST =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_B_post(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 cycle_counter,
	LSA_UINT8 data_status,
	LSA_UINT8 transfer_status
) {
	if(
		cm_is_null(dev->r1b.stat_B.buffer)
		|| dev->r1b.stat_B.wf_posted
	) {
		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = dev->r1b.stat_B.buffer;
		CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);

		CM_PUT16_HTON(message_body, 0, cycle_counter);
		CM_PUT8_HTON( message_body, 2, data_status);
		CM_PUT8_HTON( message_body, 3, transfer_status);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_GET_APDU_STATUS_DONE == 4U);

		dev->r1b.stat_B.wf_posted = LSA_TRUE;

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS_DONE, CM_SV_R1BMSG_SIZE_GET_APDU_STATUS_DONE, prefix);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-B POSTED ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_B_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		! dev->r1b.stat_B.wf_posted
		|| prefix != dev->r1b.stat_B.buffer
	) {
		CM_FATAL(); /* a bug */
	}
	else {

		dev->r1b.stat_B.wf_posted = LSA_FALSE;

		cm_sv_r1b_stat_B_next(dev); /* trigger: posted */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-B CANCEL ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_B_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( cm_is_not_null(dev->r1b.stat_B.br_next) ) {

		CM_UPPER_RQB_PTR_TYPE rb = dev->r1b.stat_B.br_next;
		dev->r1b.stat_B.br_next = LSA_NULL;

		cm_callback(dev->channel, CM_OK, &rb); /* note: could be CM_OK_CANCELLED for clarity; but: needs doc-change and no benefit for the user */
	}

	if( dev->r1b.stat_B.wf_posted ) {

		/* do nothing (cannot be cancelled) */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-B IS IDLE ==============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_stat_B_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {
	if(
		cm_is_not_null(dev->r1b.stat_B.br_next)
		|| dev->r1b.stat_B.wf_posted
	) {

		return LSA_FALSE;
	}

	return LSA_TRUE;
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
