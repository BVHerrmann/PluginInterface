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
/*  F i l e               &F: cm_sv711_pdev_a.c                         :F&  */
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
/*  - PDEV init, side "A"                                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7711
#define CM_MODULE_ID       7711

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A READ WRITE ===========================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_r1b_pdev_A_read_write(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_UPPER_MEM_U8_PTR_TYPE ndr_data, /* the clrpc-buffer! */
	LSA_UINT16 ar_nr
) {
	LSA_UINT32 pnio_err;
	CM_UPPER_RQB_PTR_TYPE pd = cm_upper_alloc_rqb(dev->channel);

	if( cm_is_null(pd) ) {

		pnio_err = CM_PNIO_FAULTY_Record__AccessDenied(255); /* no mem */
	}
	else {

		CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;
		LSA_OPCODE_TYPE opcode;

		prm_rdwr->slot_nr    = req->slot_nr;
		prm_rdwr->subslot_nr = req->subslot_nr;

		prm_rdwr->record_index = req->index;

		prm_rdwr->record_data_length = req->record_data_length - CM_RECORD_OFFSET;
		prm_rdwr->record_data        = ndr_data + CM_RECORD_OFFSET;

		prm_rdwr->is_local    = CM_PD_IS_LOCAL_NO;
		prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_YES;

		prm_rdwr->cls = req->cls;

		/***/

		opcode = req->is_read ? CM_OPC_PD_PRM_READ_SERVER : CM_OPC_PD_PRM_WRITE_SERVER;

		cm_sv_r1b_pdev_A_request(dev, opcode, ar_nr, pd); /* read/write */

		pnio_err = CM_PNIO_ERR_NONE; /* pending */
	}

	/***/

	if( !req->is_read && pnio_err != CM_PNIO_ERR_NONE ) {

		if( cm_is_not_null(req->argr) ) { /* always true */

			cm_sv_pdev_write_failed(req->argr, req->cls, req->slot_nr, req->subslot_nr, req->index, 0); /* see above: alloc failed */
		}
	}

	return pnio_err;
}

/*===========================================================================*/
/*=== PDEV PRM-BEGIN REMOTE =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pd_prm_begin_remote(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE) != 0
		|| (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_REMOTE_PDEV_INTERFACE) == 0 /* see cm_sv_argr_build() */
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pd = argr->cm.sv.r1b.remote_pdev_begin_rqb;

		if( cm_is_null(pd) || CM_RQB_GET_OPCODE(pd) != 0/*unused*/ ) {

			CM_FATAL(); /* a bug */
		}
		else {

			argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE;

			cm_sv_ar_pdev_reset_error_info(argr, LSA_TRUE /*remote pdev*/);

			pd->args.pd.prm_begin->sv.slot_nr    = slot_nr;
			pd->args.pd.prm_begin->sv.subslot_nr = subslot_nr;

			cm_sv_r1b_pdev_A_request(argr->cm.sv.dev, CM_OPC_PD_PRM_BEGIN_SERVER, argr->ar_nr, pd); /* prm-begin */
		}
	}
}

/*===========================================================================*/
/*=== PDEV PRM-BEGIN REMOTE DONE ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pd_prm_begin_remote_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev/ar(%u/%u) r1_flags(0x%08x) pd-rsp(%u)"
		, argr->device_nr, argr->ar_nr
		, argr->cm.sv.r1_flags, CM_RQB_GET_RESPONSE(pd)
		);

	if(
		(argr->cm.sv.r1_flags & CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE) == 0
		|| pd != argr->cm.sv.r1b.remote_pdev_begin_rqb
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		argr->cm.sv.r1_flags &= ~CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE;

		if( CM_RQB_GET_RESPONSE(pd) != CM_OK ) {
			/* note: the only possible failure is "bridge-broken" */
		}

		CM_RQB_SET_OPCODE(pd, 0/*unused*/);

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE */
		}
		else {

			/* note: no explicit wait-state in prm-sequence; only a check in cm_sv_pd_prm_end_remote_confirmation() */

			cm_sv_ar_submodule_state_action(argr); /* trigger: cm_sv_pd_prm_begin_remote_done (TIA 2197506) */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE BRIDGE-CODING TO PD-RESPONSE ================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_r1b_r1_rsp_to_pd_rsp( /* see cm_sv_r1b_pd_rsp_to_r1_rsp() */
	LSA_UINT16 r1_rsp
) {

	switch( r1_rsp ) {

	case 0:
		return CM_OK;

	case 1:
		return CM_ERR_PRM_INDEX;

	case 2:
		return CM_ERR_PRM_PORTID;

	case 3:
		return CM_ERR_PRM_DATA;

	case 4:
		return CM_ERR_PRM_BLOCK;

	case 5:
		return CM_ERR_PRM_CONSISTENCY;

	case 6:
		return CM_ERR_PRM_OTHER;

	case 7:
		return CM_ERR_PRM_VERSION;

	case 8:
		return CM_ERR_NOT_ACCESSIBLE;

	case 9:
		return CM_ERR_PRM_ACCESS_DENIED;

	default:
		return CM_ERR_PRM_OTHER;
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PRM-END FLAGS TO BRIDGE-CODING ==============================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_r1b_prm_end_flags_to_r1_code( /* see cm_sv_r1b_r1_code_to_prm_end_flags() */
	LSA_UINT16 prm_end_flags
) {
	LSA_UINT16 r1_code_flags = 0;

	if( (prm_end_flags & CM_PD_PRM_END_PULLED) != 0 ) {

		r1_code_flags |= 0x0001;
	}

	if( (prm_end_flags & CM_PD_PRM_END_WRONG) != 0 ) {

		r1_code_flags |= 0x0002;
	}

	if( (prm_end_flags & CM_PD_PRM_END_LOCKED) != 0 ) {

		r1_code_flags |= 0x0004;
	}

	if( (prm_end_flags & CM_PD_PRM_END_DIRTY) != 0 ) {

		r1_code_flags |= 0x0008;
	}

	if( (prm_end_flags & CM_PD_PRM_END_WRITE) != 0 ) {

		r1_code_flags |= 0x0010;
	}

#if CM_PD_PRM_END_RESERVED != 0xFFE0
#error "check this!"
#endif

	return r1_code_flags;
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A REQUEST ==============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_request(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE pd
) {

	if( cm_is_null(pd) ) {

		CM_FATAL();
	}
	else {

		CM_EDD_SET_USER_ID_HANDLE_DEV_AR (pd, CM_INVALID_HANDLE, 0, ar_nr); /* see cm_sv_r1b_pdev_A_request_done() */

		CM_RQB_SET_HANDLE(pd, CM_INVALID_HANDLE); /* unused */

		CM_RQB_SET_OPCODE(pd, opcode);

		CM_RQB_SET_RESPONSE(pd, CM_OK_PENDING); /* sanity */

		CmListInsertTail(&dev->r1b.pdev_A.queue, (CM_LIST_ENTRY_PTR_TYPE)pd);

		cm_sv_r1b_pdev_A_next(dev); /* trigger: request */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A REQUEST-DONE =========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_A_request_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE pd,
	LSA_UINT16 response
) {
	LSA_UINT16 ar_nr = CM_EDD_GET_USER_ID_AR(pd);

	CM_ASSERT(cm_is_null(dev->r1b.pdev_A.pd));

	CM_RQB_SET_RESPONSE(pd, response);

	if( ar_nr == 0 ) { /* implicit-ar */

		if( CM_RQB_GET_OPCODE(pd) == CM_OPC_PD_PRM_READ_SERVER ) {

			cm_sv_pdev_record_done(dev, LSA_NULL, LSA_TRUE/*read*/, pd);
		}
		else {

			CM_FATAL();
		}
	}
	else {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_null(argr) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			switch( CM_RQB_GET_OPCODE(pd) ) {

			case CM_OPC_PD_PRM_READ_SERVER:
				cm_sv_pdev_record_done(dev, argr, LSA_TRUE/*read*/, pd);
				break;

			case CM_OPC_PD_PRM_WRITE_SERVER:
				cm_sv_pdev_record_done(dev, argr, LSA_FALSE/*write*/, pd);
				break;

			case CM_OPC_PD_PRM_BEGIN_SERVER:
				cm_sv_pd_prm_begin_remote_done(argr, pd);
				break;

			case CM_OPC_PD_PRM_END_SERVER:
				cm_sv_pd_prm_end_remote_confirmation(argr, pd);
				break;

			default:
				CM_FATAL();
				break;
			}
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A POST PRM READ ========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_A_post_prm_read(
	CM_UPPER_RQB_PTR_TYPE pd,
	LSA_UINT16 user_id,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {

	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;
	LSA_UINT16 record_data_length;
	LSA_UINT32 message_size;
	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max);

	if( message_max < CM_SV_R1BMSG_SIZE_PDEV_PRM_READ ) {
		CM_FATAL(); /* a bug */
	}

	if( prm_rdwr->record_data_length > 0xFFFF ) { /* see cast below */

		record_data_length = 0xFFFF;
	}
	else {

		record_data_length = (LSA_UINT16)prm_rdwr->record_data_length;
	}

	CM_PUT16_HTON(message_body, 0, user_id);
	CM_PUT16_HTON(message_body, 2, prm_rdwr->slot_nr);
	CM_PUT16_HTON(message_body, 4, prm_rdwr->subslot_nr);
	CM_PUT32_HTON(message_body, 6, prm_rdwr->record_index);
	CM_PUT16_HTON(message_body, 10, record_data_length);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_READ == 12U);

	message_size = CM_SV_R1BMSG_SIZE_PDEV_PRM_READ;

	dev->r1b.pdev_A.pd = pd;
	dev->r1b.pdev_A.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ, message_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A POST PRM WRITE =======================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_A_post_prm_write(
	CM_UPPER_RQB_PTR_TYPE pd,
	LSA_UINT16 user_id,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;
	LSA_UINT32 message_size;
	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max);

	if(
		prm_rdwr->record_data_length > 0xFFFF /* see cm_sv_pdev_read_write() and cast below */
		|| (CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE + prm_rdwr->record_data_length) > message_max
	) {
		CM_FATAL(); /* bug */
	}

	CM_PUT16_HTON(message_body,  0, user_id);
	CM_PUT16_HTON(message_body,  2, prm_rdwr->slot_nr);
	CM_PUT16_HTON(message_body,  4, prm_rdwr->subslot_nr);
	CM_PUT32_HTON(message_body,  6, prm_rdwr->record_index);
	CM_PUT16_HTON(message_body, 10, (LSA_UINT16)prm_rdwr->record_data_length);
	CM_PUT16_HTON(message_body, 12, (LSA_UINT16)(prm_rdwr->mk_remanent == CM_PD_MK_REMANENT_YES ? 1 : 0));

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE == 14U);

	CM_MEMCPY(message_body + CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE, prm_rdwr->record_data, prm_rdwr->record_data_length);

	message_size = CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE + prm_rdwr->record_data_length;

	dev->r1b.pdev_A.pd = pd;
	dev->r1b.pdev_A.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE, message_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A POST PRM-BEGIN =======================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_A_post_prm_begin(
	CM_UPPER_RQB_PTR_TYPE pd,
	LSA_UINT16 user_id,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	CM_UPPER_PD_PRM_BEGIN_PTR_TYPE prm_begin = pd->args.pd.prm_begin;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);

	CM_PUT16_HTON(message_body, 0, user_id);
	CM_PUT16_HTON(message_body, 2, prm_begin->sv.slot_nr);
	CM_PUT16_HTON(message_body, 4, prm_begin->sv.subslot_nr);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN == 6U);

	dev->r1b.pdev_A.pd = pd;
	dev->r1b.pdev_A.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN, CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A POST PRM-END =========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_A_post_prm_end(
	CM_UPPER_RQB_PTR_TYPE pd,
	LSA_UINT16 user_id,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	CM_UPPER_PD_PRM_END_PTR_TYPE prm_end = pd->args.pd.prm_end;
	CM_PD_AR_PRM_END_PTR_TYPE prm_end_args = (CM_PD_AR_PRM_END_PTR_TYPE)prm_end->sv.void_ptr;
	LSA_UINT32 message_size;
	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max);

	if( prm_end->set_mrp_off == CM_SET_MRP_OFF_YES || !cm_uuid_is_nil(&prm_end_args->ir_data_uuid) ) {
		CM_FATAL();
	}

	CM_PUT16_HTON(message_body, 0, user_id);
	CM_PUT16_HTON(message_body, 2, prm_end_args->max_alarm_data_length);
	CM_PUT16_HTON(message_body, 4, prm_end_args->slot_nr);
	CM_PUT16_HTON(message_body, 6, prm_end_args->subslot_nr);
	CM_PUT16_HTON(message_body, 8, prm_end_args->nr_of_elems);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_END == 10U);

	{
	LSA_UINT32 offset = CM_SV_R1BMSG_SIZE_PDEV_PRM_END;
	LSA_UINT16 idx;

	for( idx = 0; idx < prm_end_args->nr_of_elems; ++idx ) {

		CM_PD_AR_PRM_END_ELEM_PTR_TYPE elem = &prm_end_args->elem[idx];

		LSA_UINT16 r1_code_flags = cm_sv_r1b_prm_end_flags_to_r1_code(elem->flags);

		CM_PUT16_HTON(message_body, offset +  0, elem->slot_nr);
		CM_PUT16_HTON(message_body, offset +  2, elem->subslot_nr);
		CM_PUT32_HTON(message_body, offset +  4, elem->index);
		CM_PUT32_HTON(message_body, offset +  8, elem->offset);
		CM_PUT16_HTON(message_body, offset + 12, r1_code_flags);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_END_ELEM == 14U);

		offset += CM_SV_R1BMSG_SIZE_PDEV_PRM_END_ELEM;
	}

	message_size = offset;
	}

	dev->r1b.pdev_A.pd = pd;
	dev->r1b.pdev_A.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_END, message_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A REQUEST NEXT =========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_next(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHED ) {

		/* wait for "established" (remote is not yet updated) */
	}
	else if( dev->r1b.pdev_A.wf_posted ) {

		/* wait for "posted" */
	}
	else if( cm_is_not_null(dev->r1b.pdev_A.pd) ) {

		/* wait for result from "pdev_B" */
	}
	else if( CmListIsEmpty(&dev->r1b.pdev_A.queue) ) {

		/* idle, nothing to do */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pd;
		LSA_UINT16 user_id;
		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix;

		CmListRemoveHead(&dev->r1b.pdev_A.queue, pd, CM_UPPER_RQB_PTR_TYPE);

		user_id = ++dev->r1b.pdev_A.user_id; /* see "handle done-message" */

		if( cm_is_null(dev->r1b.pdev_A.buffer) ) {
			CM_FATAL(); /* a bug */
		}

		prefix = dev->r1b.pdev_A.buffer;

		switch( CM_RQB_GET_OPCODE(pd) ) {

		case CM_OPC_PD_PRM_READ_SERVER:
			cm_sv_r1b_pdev_A_post_prm_read(pd, user_id, dev, prefix);
			break;

		case CM_OPC_PD_PRM_WRITE_SERVER:
			cm_sv_r1b_pdev_A_post_prm_write(pd, user_id, dev, prefix);
			break;

		case CM_OPC_PD_PRM_BEGIN_SERVER:
			cm_sv_r1b_pdev_A_post_prm_begin(pd, user_id, dev, prefix);
			break;

		case CM_OPC_PD_PRM_END_SERVER:
			cm_sv_r1b_pdev_A_post_prm_end(pd, user_id, dev, prefix);
			break;

		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A POSTED ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {

	if(
		! dev->r1b.pdev_A.wf_posted
		|| prefix != dev->r1b.pdev_A.buffer
	) {

		CM_FATAL(); /* bug */
	}
	else {

		dev->r1b.pdev_A.wf_posted = LSA_FALSE;

		cm_sv_r1b_pdev_A_next(dev); /* trigger: posted */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A HANDLE PRM READ DONE =================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_read_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size < CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		if( cm_is_null(dev->r1b.pdev_A.pd) ) {

			CM_FATAL(); /* R1 protocol error (message unexpected) */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_A.pd;
			dev->r1b.pdev_A.pd = LSA_NULL;

			{
			CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

			LSA_UINT16 user_id            = CM_GET16_NTOH(message_body, 0);
			LSA_UINT16 record_data_length = CM_GET16_NTOH(message_body, 2);
			LSA_UINT16 r1_response        = CM_GET16_NTOH(message_body, 4);

			LSA_UINT16 pd_response        = cm_sv_r1b_r1_rsp_to_pd_rsp(r1_response);

			CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE == 6U);

			if( dev->r1b.pdev_A.user_id != user_id ) {
				CM_FATAL(); /* R1 protocol error */
			}

			if( CM_RQB_GET_OPCODE(pd) != CM_OPC_PD_PRM_READ_SERVER ) {
				CM_FATAL(); /* R1 protocol error (message unexpected) */
			}

			if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE + record_data_length ) {
				CM_FATAL(); /* R1 protocol error */
			}

			if( record_data_length > prm_rdwr->record_data_length ) { /* see memcpy() below */
				CM_FATAL(); /* R1 protocol error */
			}

			/***/

			if( pd_response != CM_OK ) {

				prm_rdwr->record_data_length = 0; /* sanity */
			}
			else {

				CM_MEMCPY(prm_rdwr->record_data, message_body + CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE, record_data_length);

				prm_rdwr->record_data_length = record_data_length;
			}

			prm_rdwr->cm_pnio_err = cm_pd_response_to_pnio_err(pd_response, LSA_TRUE);

			cm_sv_r1b_pdev_A_request_done(dev, pd, pd_response);
			}

			/***/

			cm_callback(dev->channel, CM_OK, &rb);

			/***/

			cm_sv_r1b_pdev_A_next(dev); /* trigger: read/write done */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A HANDLE PRM WRITE DONE ================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_write_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size < CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		if( cm_is_null(dev->r1b.pdev_A.pd) ) {

			CM_FATAL(); /* R1 protocol error (message unexpected) */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_A.pd;
			dev->r1b.pdev_A.pd = LSA_NULL;

			{
			CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

			LSA_UINT16 user_id     = CM_GET16_NTOH(message_body, 0);
			LSA_UINT16 r1_response = CM_GET16_NTOH(message_body, 2);

			LSA_UINT16 pd_response = cm_sv_r1b_r1_rsp_to_pd_rsp(r1_response);

			CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE == 4U);

			if( dev->r1b.pdev_A.user_id != user_id ) {
				CM_FATAL(); /* R1 protocol error */
			}

			if( CM_RQB_GET_OPCODE(pd) != CM_OPC_PD_PRM_WRITE_SERVER ) {
				CM_FATAL(); /* R1 protocol error (message unexpected) */
			}

			if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE ) {
				CM_FATAL(); /* R1 protocol error */
			}

			/***/

			prm_rdwr->cm_pnio_err = cm_pd_response_to_pnio_err(pd_response, LSA_FALSE);

			cm_sv_r1b_pdev_A_request_done(dev, pd, pd_response);
			}

			/***/

			cm_callback(dev->channel, CM_OK, &rb);

			/***/

			cm_sv_r1b_pdev_A_next(dev); /* trigger: read/write done */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A HANDLE PRM-BEGIN DONE ================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_begin_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN_DONE ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		if( cm_is_null(dev->r1b.pdev_A.pd) ) {

			CM_FATAL(); /* R1 protocol error (message unexpected) */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_A.pd;
			dev->r1b.pdev_A.pd = LSA_NULL;

			if( CM_RQB_GET_OPCODE(pd) != CM_OPC_PD_PRM_BEGIN_SERVER ) {

				CM_FATAL(); /* R1 protocol error (message unexpected) */
			}
			else {

				LSA_UINT16 user_id = CM_GET16_NTOH(message_body, 0);

				CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN_DONE == 2U);

				if( dev->r1b.pdev_A.user_id != user_id ) {
					CM_FATAL(); /* R1 protocol error */
				}

				cm_sv_r1b_pdev_A_request_done(dev, pd, CM_OK);
			}

			/***/

			cm_callback(dev->channel, CM_OK, &rb);

			/***/

			cm_sv_r1b_pdev_A_next(dev); /* trigger: prm-begin done */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A HANDLE PRM-END DONE ==================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_handle_prm_end_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_END_DONE ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		if( cm_is_null(dev->r1b.pdev_A.pd) ) {

			CM_FATAL(); /* R1 protocol error (message unexpected) */
		}
		else {

			CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_A.pd;
			dev->r1b.pdev_A.pd = LSA_NULL;

			if( CM_RQB_GET_OPCODE(pd) != CM_OPC_PD_PRM_END_SERVER ) {

				CM_FATAL(); /* R1 protocol error (message unexpected) */
			}
			else {

				CM_UPPER_PD_PRM_END_PTR_TYPE prm_end = pd->args.pd.prm_end;

				LSA_UINT16 user_id     = CM_GET16_NTOH(message_body, 0);
				LSA_UINT16 r1_response = CM_GET16_NTOH(message_body, 2);

				LSA_UINT16 pd_response = cm_sv_r1b_r1_rsp_to_pd_rsp(r1_response);

				CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_END_DONE == 4U);

				if( dev->r1b.pdev_A.user_id != user_id ) {
					CM_FATAL(); /* R1 protocol error */
				}

				prm_end->is_master_project = CM_IS_MASTER_PROJECT_NO; /* sanity */

				cm_sv_r1b_pdev_A_request_done(dev, pd, pd_response);
			}

			/***/

			cm_callback(dev->channel, CM_OK, &rb);

			/***/

			cm_sv_r1b_pdev_A_next(dev); /* trigger: prm-end done */
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A CANCEL ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_A_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
) {

	if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_BROKEN ) {

		CM_FATAL();
	}

	if( cm_is_not_null(dev->r1b.pdev_A.pd) ) { /* cancel "current" */

		CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_A.pd;
		dev->r1b.pdev_A.pd = LSA_NULL;

		/* note: "pdev_A.pd" is not referenced by "pdev_A.wf_posted" (it was copied/marshalled into "pdev_A.buffer") */

		cm_sv_r1b_pdev_A_request_done(dev, pd, CM_ERR_NOT_ACCESSIBLE); /* note: remote pdev-submodules are pulled on bridge-broken */
	}

	while( ! CmListIsEmpty(&dev->r1b.pdev_A.queue) ) { /* cancel "queue" */

		CM_UPPER_RQB_PTR_TYPE pd;

		CmListRemoveHead(&dev->r1b.pdev_A.queue, pd, CM_UPPER_RQB_PTR_TYPE );

		cm_sv_r1b_pdev_A_request_done(dev, pd, CM_ERR_NOT_ACCESSIBLE); /* note: remote pdev-submodules are pulled on bridge-broken */
	}

	if( dev->r1b.pdev_A.wf_posted ) {

		/* do nothing (cannot be cancelled) */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-A IS-IDLE ==============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_pdev_A_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {

	if(
		! CmListIsEmpty(&dev->r1b.pdev_A.queue)
		|| cm_is_not_null(dev->r1b.pdev_A.pd)
		|| dev->r1b.pdev_A.wf_posted
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
