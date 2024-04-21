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
/*  F i l e               &F: cm_sv713_stat_a.c                         :F&  */
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
/*  - apdu-status side "A"                                                   */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	7713
#define CM_MODULE_ID		7713

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== R1-BRIDGE STAT-A REQUEST ==============================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_r1b_stat_A_request(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 iocr_ref,
	CLRPC_UUID_PTR_TYPE ar_uuid,
	LSA_VOID_PTR_TYPE user_id
) {
	CM_LOCAL_MEM_PTR_TYPE local_mem;
	CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE stat;

	CM_ALLOC_LOCAL_MEM(&local_mem, sizeof(*stat));
	stat = (CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE)local_mem;

	if( cm_is_null(stat) ) {

		CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "cannot alloc local-mem"
			);

		return CM_ERR_ALLOC_LOCAL;
	}
	else {

		stat->iocr_ref = iocr_ref;
		stat->ar_uuid  = ar_uuid;
		stat->user_id  = user_id;

		CmListInsertTail(&dev->r1b.stat_A.queue, (CM_LIST_ENTRY_PTR_TYPE)stat);

		cm_sv_r1b_stat_A_next(dev); /* trigger: request */
	}

	return CM_OK_PENDING;
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-A NEXT =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_A_next(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( dev->r1b.stat_A.wf_posted ) {

		/* wait for "posted" */
	}
	else if( cm_is_not_null(dev->r1b.stat_A.curr) ) {

		/* wait for result from side B */
	}
	else if( CmListIsEmpty(&dev->r1b.stat_A.queue) ) {

		/* idle, nothing to do */
	}
	else {

		CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE stat;

		CmListRemoveHead(&dev->r1b.stat_A.queue, stat, CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE);

		cm_sv_r1b_stat_A_post(dev, stat);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-A POST =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_A_post(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE stat
) {
	if(
		cm_is_null(dev->r1b.stat_A.buffer)
		|| dev->r1b.stat_A.wf_posted
	) {
		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = dev->r1b.stat_A.buffer;
		CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);

		CM_PUT16_HTON(   message_body, 0, stat->iocr_ref);
		CM_PUT_UUID_HTON(message_body, 2, stat->ar_uuid);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_GET_APDU_STATUS == 18U);

		dev->r1b.stat_A.curr      = stat;
		dev->r1b.stat_A.wf_posted = LSA_TRUE;

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS, CM_SV_R1BMSG_SIZE_GET_APDU_STATUS, prefix);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-A POSTED ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_A_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {
	if(
		! dev->r1b.stat_A.wf_posted
		|| prefix != dev->r1b.stat_A.buffer
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		dev->r1b.stat_A.wf_posted = LSA_FALSE;

		cm_sv_r1b_stat_A_next(dev); /* trigger: posted */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-A DONE =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_A_handle_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size != CM_SV_R1BMSG_SIZE_GET_APDU_STATUS_DONE ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		if( cm_is_null(dev->r1b.stat_A.curr) ) {
			CM_FATAL(); /* a bug */
		}
		else {

			CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE stat = dev->r1b.stat_A.curr;
			dev->r1b.stat_A.curr = LSA_NULL;

			{
			LSA_UINT16 cycle_counter   = CM_GET16_NTOH(message_body, 0);
			LSA_UINT8  data_status     = CM_GET8_NTOH( message_body, 2);
			LSA_UINT8  transfer_status = CM_GET8_NTOH( message_body, 3);

			CM_ASSERT(CM_SV_R1BMSG_SIZE_GET_APDU_STATUS_DONE == 4U);

			cm_sv_frame_get_apdu_status_done(stat->user_id, cycle_counter, data_status, transfer_status); /* R1 stat-A done */
			}

			{
			LSA_UINT16 ret_val;
			CM_FREE_LOCAL_MEM(&ret_val, stat);
			CM_ASSERT(ret_val == LSA_RET_OK);
			LSA_UNUSED_ARG(ret_val);
			}

			/***/

			cm_sv_r1b_stat_A_next(dev); /* trigger: done */
		}
	}

	/***/

	cm_callback(dev->channel, CM_OK, &rb);
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-A CANCEL ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_stat_A_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_BROKEN ) {
		CM_FATAL(); /* a bug */
	}

	if( cm_is_not_null(dev->r1b.stat_A.curr) ) {

		CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE stat = dev->r1b.stat_A.curr;
		dev->r1b.stat_A.curr = LSA_NULL;

		cm_sv_frame_get_apdu_status_done(stat->user_id, 0, 0, 0); /* R1 bridge-broken, cancel stat-A (current) */

		{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM(&ret_val, stat);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		}
	}

	while( ! CmListIsEmpty(&dev->r1b.stat_A.queue) ) { /* cancel "queue" */

		CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE stat;

		CmListRemoveHead(&dev->r1b.stat_A.queue, stat, CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE);

		cm_sv_frame_get_apdu_status_done(stat->user_id, 0, 0, 0); /* R1 bridge-broken, cancel stat-A (queue) */

		{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM(&ret_val, stat);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		}
	}

	if( dev->r1b.stat_A.wf_posted ) {

		/* do nothing (cannot be cancelled) */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE STAT-A IS IDLE ==============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_stat_A_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {
	if(
		! CmListIsEmpty(&dev->r1b.stat_A.queue)
		|| cm_is_not_null(dev->r1b.stat_A.curr)
		|| dev->r1b.stat_A.wf_posted
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
