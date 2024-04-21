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
/*  F i l e               &F: cm_sv712_pdev_b.c                         :F&  */
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
/*  - PDEV side "B"                                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7712
#define CM_MODULE_ID       7712

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_read(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
); /* forward */

static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_write(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
); /* forward */


static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_begin(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
); /* forward */

static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_end(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
); /* forward */

/*===========================================================================*/
/*=== R1-BRIDGE PD-RESPONSE TO BRIDGE-CODING ================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_r1b_pd_rsp_to_r1_rsp( /* see cm_sv_r1b_r1_rsp_to_pd_rsp() */
	LSA_RESPONSE_TYPE pd_rsp
) {
	switch( pd_rsp ) {

	case CM_OK:
		return 0;

	case CM_ERR_PRM_INDEX:
		return 1;

	case CM_ERR_PRM_PORTID:
		return 2;

	case CM_ERR_PRM_DATA:
		return 3;

	case CM_ERR_PRM_BLOCK:
		return 4;

	case CM_ERR_PRM_CONSISTENCY:
		return 5;

	case CM_ERR_PRM_OTHER:
		return 6;

	case CM_ERR_PRM_VERSION:
		return 7;

	case CM_ERR_NOT_ACCESSIBLE:
		return 8;

	case CM_ERR_PRM_ACCESS_DENIED:
		return 9;

	default:
		return 10; /* note: not code of CM_ERR_PRM_OTHER here */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE BRIDGE-CODING TO PRM-END FLAGS ==============================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_r1b_r1_code_to_prm_end_flags( /* see cm_sv_r1b_prm_end_flags_to_r1_code() */
	LSA_UINT16 r1_code_flags
) {
	LSA_UINT16 prm_end_flags = 0;

	if( (r1_code_flags & 0x0001) != 0 ) {

		prm_end_flags |= CM_PD_PRM_END_PULLED;
	}

	if( (r1_code_flags & 0x0002) != 0 ) {

		prm_end_flags |= CM_PD_PRM_END_WRONG;
	}

	if( (r1_code_flags & 0x0004) != 0 ) {

		prm_end_flags |= CM_PD_PRM_END_LOCKED;
	}

	if( (r1_code_flags & 0x0008) != 0 ) {

		prm_end_flags |= CM_PD_PRM_END_DIRTY;
	}

	if( (r1_code_flags & 0x0010) != 0 ) {

		prm_end_flags |= CM_PD_PRM_END_WRITE;
	}

#if CM_PD_PRM_END_RESERVED != 0xFFE0
#error "check this!"
#endif

	return prm_end_flags;
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B HANDLE READ WRITE BEGIN END ==========================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_B_handle_read_write_begin_end(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {

	CM_ASSERT(CM_RQB_GET_OPCODE(rb) == CM_OPC_SV_R1BRIDGE_RECEIVED);

	if( cm_is_not_null(dev->r1b.pdev_B.br_next) ) {

		CM_FATAL(); /* R1 protocol error ("pdev_A" did not wait) */
	}
	else {

		dev->r1b.pdev_B.br_next = rb; /* append to "queue" */

		cm_sv_r1b_pdev_B_next(dev);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B NEXT =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_B_next(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	if( cm_is_not_null(dev->r1b.pdev_B.pd) ) {

		/* wait for result from CM-PD */
	}
	else if( dev->r1b.pdev_B.wf_posted ) {

		/* wait for "posted" */
	}
	else if( cm_is_null(dev->r1b.pdev_B.br_next) ) {

		/* idle, nothing to do */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE rb = dev->r1b.pdev_B.br_next;
		dev->r1b.pdev_B.br_next = LSA_NULL;

		{
		CM_UPPER_SV_R1BRIDGE_RECEIVED_PTR_TYPE param = rb->args.sv.r1bridge_received; /* note: parameter were checked cm_sv_opc_r1bridge_received() */
		CM_COMMON_MEM_U8_PTR_TYPE header = (CM_COMMON_MEM_U8_PTR_TYPE)param->message_data;
		CM_COMMON_MEM_U8_PTR_TYPE message_body = header + CM_SV_R1BMSG_SIZE_HEADER;
		LSA_UINT32 message_size = CM_GET32_NTOH(header, 0);
		LSA_UINT16 message_numb = CM_GET16_NTOH(header, 4);

		switch( message_numb ) {

		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ:
			cm_sv_r1b_pdev_B_handle_prm_read(dev, rb, message_body, message_size);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE:
			cm_sv_r1b_pdev_B_handle_prm_write(dev, rb, message_body, message_size);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN:
			cm_sv_r1b_pdev_B_handle_prm_begin(dev, rb, message_body, message_size);
			break;

		case CM_SV_R1BRIDGE_MSG_PDEV_PRM_END:
			cm_sv_r1b_pdev_B_handle_prm_end(dev, rb, message_body, message_size);
			break;

		default:
			CM_FATAL(); /* a bug */
			break;
		}
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B HANDLE PRM READ ======================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_read(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = dev->r1b.pdev_B.buffer;
	CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_B.rqb;

	LSA_UINT16 user_id;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;
	LSA_UINT32 record_index;
	LSA_UINT16 record_data_length;
	LSA_UINT32 cls;
	LSA_UINT16 pd_response;

	if( cm_is_null(prefix) || cm_is_null(pd) ) {
		CM_FATAL();
	}

	if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_READ ) {
		CM_FATAL(); /* R1 protocol error */
	}

	user_id            = CM_GET16_NTOH(message_body,  0);
	slot_nr            = CM_GET16_NTOH(message_body,  2);
	subslot_nr         = CM_GET16_NTOH(message_body,  4);
	record_index       = CM_GET32_NTOH(message_body,  6);
	record_data_length = CM_GET16_NTOH(message_body, 10);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_READ == 12U);

	cls = cm_record_index_classify(dev->channel, 0/*!*/, record_index, dev->vendor_id);

	if( ! CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr) ) { /* always false (see "pdev_A") */

		pd_response = CM_ERR_PRM_INDEX;
	}
	else if( (cls & CM_REC_IDX_PDEV) == 0 || (cls & CM_REC_IDX_8000) == 0 ) {

		pd_response = CM_ERR_PRM_INDEX;
	}
	else {

		CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

		LSA_UINT32 message_max;
		CM_COMMON_MEM_U8_PTR_TYPE read_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max);
		CM_COMMON_MEM_U8_PTR_TYPE record_data = read_body + CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE;

		if( message_max < CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE ) {
			CM_FATAL(); /* a bug */
		}
		else {

			LSA_UINT32 max_payload = message_max - CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE;

			if( max_payload > 0xFFFF ) { /* see cast below */

				max_payload = 0xFFFF;
			}

			if( record_data_length > max_payload ) {

				record_data_length = (LSA_UINT16)max_payload; /* so that answer fits into pdev_B.buffer */
			}
		}

		prm_rdwr->slot_nr    = slot_nr;
		prm_rdwr->subslot_nr = subslot_nr;

		prm_rdwr->record_index = record_index;

		prm_rdwr->record_data_length = record_data_length;
		prm_rdwr->record_data        = record_data;

		prm_rdwr->is_local    = CM_PD_IS_LOCAL_NO; /* don't care */
		prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_NO; /* don't care */

		prm_rdwr->cls = cls;

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) r1b_pdev_B: prm-read, sl/su(%u/0x%04x), index(0x%x), record_data_length(%u)"
			, dev->device_nr, slot_nr, subslot_nr, record_index, record_data_length
			);

		/***/

		dev->r1b.pdev_B.pd      = pd;
		dev->r1b.pdev_B.user_id = user_id;

		cm_sv_pdev_request(dev->channel, CM_OPC_PD_PRM_READ_SERVER, dev->device_nr, CM_SV_R1B_PDEV_B_AR_NR, pd);

		pd_response = CM_OK_PENDING;
	}

	/***/

	if( pd_response != CM_OK_PENDING ) {

		//R1 VV: make a common function for "error read/write"

		CM_COMMON_MEM_U8_PTR_TYPE error_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);

		LSA_UINT16 r1_response = cm_sv_r1b_pd_rsp_to_r1_rsp(pd_response);

		CM_PUT16_HTON(error_body, 0, user_id);
		CM_PUT16_HTON(error_body, 2, 0 /* record data length */);
		CM_PUT16_HTON(error_body, 4, r1_response);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE == 6U);

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) r1b_pdev_B: prm-read, sl/su(%u/0x%04x), index(0x%x), pd_response(%u)"
			, dev->device_nr, slot_nr, subslot_nr, record_index, pd_response
			);

		/***/

		CM_ASSERT(cm_is_null(dev->r1b.pdev_B.pd));

		dev->r1b.pdev_B.wf_posted = LSA_TRUE;

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ_DONE, CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE, prefix);
	}

	/***/

	cm_callback(dev->channel, CM_OK, &rb);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B HANDLE PRM WRITE =====================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_write(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = dev->r1b.pdev_B.buffer;
	CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_B.rqb;

	LSA_UINT16 user_id;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;
	LSA_UINT32 record_index;
	LSA_UINT16 record_data_length;
	LSA_UINT16 mk_remanent;
	LSA_UINT32 cls;
	LSA_UINT16 pd_response;

	if( cm_is_null(prefix) || cm_is_null(pd) ) {
		CM_FATAL();
	}

	if( message_size < CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE ) {
		CM_FATAL(); /* R1 protocol error */
	}

	user_id            = CM_GET16_NTOH(message_body,  0);
	slot_nr            = CM_GET16_NTOH(message_body,  2);
	subslot_nr         = CM_GET16_NTOH(message_body,  4);
	record_index       = CM_GET32_NTOH(message_body,  6);
	record_data_length = CM_GET16_NTOH(message_body, 10);
	mk_remanent        = CM_GET16_NTOH(message_body, 12);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE == 14U);

	if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE + record_data_length ) {
		CM_FATAL(); /* R1 protocol error (write message) */
	}

	cls = cm_record_index_classify(dev->channel, 0/*!*/, record_index, dev->vendor_id);

	if( ! CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr) ) { /* always false (see "pdev_A") */

		pd_response = CM_ERR_PRM_INDEX;
	}
	else if( (cls & CM_REC_IDX_PDEV) == 0 || (cls & CM_REC_IDX_8000) == 0 ) {

		pd_response = CM_ERR_PRM_INDEX;
	}
	else {

		CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

		LSA_UINT32 message_max;
		CM_COMMON_MEM_U8_PTR_TYPE write_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max); /* using message-buffer as storage */

		if( record_data_length + CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE > message_max ) {
			CM_FATAL(); /* a bug */
		}

		/* note: must copy the data, "rb/message_body" is returned to the user below */
		CM_MEMCPY(write_body, message_body + CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE, record_data_length);

		prm_rdwr->slot_nr    = slot_nr;
		prm_rdwr->subslot_nr = subslot_nr;

		prm_rdwr->record_index = record_index;

		prm_rdwr->record_data_length = record_data_length;
		prm_rdwr->record_data        = write_body;

		prm_rdwr->is_local    = CM_PD_IS_LOCAL_NO;
		prm_rdwr->mk_remanent = (mk_remanent ? CM_PD_MK_REMANENT_YES : CM_PD_MK_REMANENT_NO);

		prm_rdwr->cls = cls;

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) r1b_pdev_B: prm-write, sl/su(%u/0x%04x), index(0x%x), record_data_length(%u)"
			, dev->device_nr, slot_nr, subslot_nr, record_index, record_data_length
			);

		/***/

		dev->r1b.pdev_B.pd      = pd;
		dev->r1b.pdev_B.user_id = user_id;

		cm_sv_pdev_request(dev->channel, CM_OPC_PD_PRM_WRITE_SERVER, dev->device_nr, CM_SV_R1B_PDEV_B_AR_NR, pd);

		pd_response = CM_OK_PENDING;
	}

	/***/

	if( pd_response != CM_OK_PENDING ) {

		//R1 VV: make a common function for "error read/write"

		CM_COMMON_MEM_U8_PTR_TYPE error_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);

		LSA_UINT16 r1_response = cm_sv_r1b_pd_rsp_to_r1_rsp(pd_response);

		CM_PUT16_HTON(error_body, 0, user_id);
		CM_PUT16_HTON(error_body, 2, r1_response);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE == 4U);

		CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) r1b_pdev_B: prm-write, sl/su(%u/0x%04x), index(0x%x), pd_response(%u)"
			, dev->device_nr, slot_nr, subslot_nr, record_index, pd_response
			);

		/***/

		CM_ASSERT(cm_is_null(dev->r1b.pdev_B.pd));

		dev->r1b.pdev_B.wf_posted = LSA_TRUE;

		cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE_DONE, CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE + 0, prefix);
	}

	/***/

	cm_callback(dev->channel, CM_OK, &rb);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B HANDLE PRM-BEGIN =====================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_begin(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_B.rqb;
		CM_UPPER_PD_PRM_BEGIN_PTR_TYPE prm_begin = pd->args.pd.prm_begin;
		LSA_UINT16 user_id;

		user_id                             = CM_GET16_NTOH(message_body, 0);
		prm_begin->sv.slot_nr               = CM_GET16_NTOH(message_body, 2);
		prm_begin->sv.subslot_nr            = CM_GET16_NTOH(message_body, 4);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN == 6U);

		CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) r1b_pdev_B: prm-begin, sl/su(%u/0x%04x)"
			, dev->device_nr, prm_begin->sv.slot_nr, prm_begin->sv.subslot_nr
			);

		/***/

		dev->r1b.pdev_B.pd      = pd;
		dev->r1b.pdev_B.user_id = user_id;

		cm_sv_pdev_request(dev->channel, CM_OPC_PD_PRM_BEGIN_SERVER, dev->device_nr, CM_SV_R1B_PDEV_B_AR_NR, pd);

		/***/

		cm_callback(dev->channel, CM_OK, &rb);
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B HANDLE PRM-END =======================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_handle_prm_end(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size < CM_SV_R1BMSG_SIZE_PDEV_PRM_END ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE pd = dev->r1b.pdev_B.rqb;
		CM_UPPER_PD_PRM_END_PTR_TYPE pd_prm_end = pd->args.pd.prm_end;
		CM_PD_AR_PRM_END_PTR_TYPE prm_end_args = &dev->r1b.pdev_B.prm_end_args;
		LSA_UINT16 user_id;

		user_id                             = CM_GET16_NTOH(message_body, 0);
		prm_end_args->ir_data_uuid          = cm_uuid_nil_uuid; /* note: see check in "pdev_A" */
		prm_end_args->max_alarm_data_length = CM_GET16_NTOH(message_body, 2);
		prm_end_args->slot_nr               = CM_GET16_NTOH(message_body, 4);
		prm_end_args->subslot_nr            = CM_GET16_NTOH(message_body, 6);
		prm_end_args->nr_of_elems           = CM_GET16_NTOH(message_body, 8);

		CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_END == 10U);

		if( message_size != CM_SV_R1BMSG_SIZE_PDEV_PRM_END + (prm_end_args->nr_of_elems * CM_SV_R1BMSG_SIZE_PDEV_PRM_END_ELEM) ) {

			CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) R1 protocol error (message_size(%u) vs. nr_of_elems(%u))"
				, dev->device_nr, message_size, prm_end_args->nr_of_elems
				);

			CM_FATAL(); /* R1 protocol error */
		}
		else if( prm_end_args->nr_of_elems > (sizeof(prm_end_args->elem) / sizeof(prm_end_args->elem[0])) ) {

			CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev(%u) R1 protocol error (nr_of_elems(%u) > %u)"
				, dev->device_nr, prm_end_args->nr_of_elems, (sizeof(prm_end_args->elem) / sizeof(prm_end_args->elem[0]))
				);

			CM_FATAL(); /* R1 protocol error (actually a bug; why did connect and/or connect-update succeed?) */
		}
		else {

			LSA_UINT32 offset = CM_SV_R1BMSG_SIZE_PDEV_PRM_END;
			LSA_UINT16 idx;

			for( idx = 0; idx < prm_end_args->nr_of_elems; ++idx ) {

				CM_PD_AR_PRM_END_ELEM_PTR_TYPE elem = &prm_end_args->elem[idx];

				LSA_UINT16 r1_code_flags;

				elem->slot_nr    = CM_GET16_NTOH(message_body, offset + 0);
				elem->subslot_nr = CM_GET16_NTOH(message_body, offset + 2);
				elem->index      = CM_GET32_NTOH(message_body, offset + 4);
				elem->offset     = CM_GET32_NTOH(message_body, offset + 8);
				r1_code_flags    = CM_GET16_NTOH(message_body, offset + 12);

				elem->flags = cm_sv_r1b_r1_code_to_prm_end_flags(r1_code_flags);

				CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_END_ELEM == 14U);

				offset += CM_SV_R1BMSG_SIZE_PDEV_PRM_END_ELEM;
			}

			CM_ASSERT(offset == message_size);

			pd_prm_end->set_mrp_off = CM_SET_MRP_OFF_NO; /* see cm_sv_pd_prm_end_remote_request(), TIA 1434352 */
			pd_prm_end->sv.void_ptr = (LSA_VOID_PTR_TYPE)prm_end_args;

			CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) r1b_pdev_B: prm-end, sl/su(%u/0x%04x)"
				, dev->device_nr, prm_end_args->slot_nr, prm_end_args->subslot_nr
				);

			/***/

			dev->r1b.pdev_B.pd      = pd;
			dev->r1b.pdev_B.user_id = user_id;

			cm_sv_pdev_request(dev->channel, CM_OPC_PD_PRM_END_SERVER, dev->device_nr, CM_SV_R1B_PDEV_B_AR_NR, pd);

			/***/

			cm_callback(dev->channel, CM_OK, &rb);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POST PDEV-B PRM READ-DONE ===================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_post_prm_read_done(
	CM_UPPER_RQB_CONST_PTR_TYPE pd,
	LSA_UINT16 pd_response,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;
	LSA_UINT16 r1_response = cm_sv_r1b_pd_rsp_to_r1_rsp(pd_response);

	LSA_UINT16 data_offset = CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE;
	LSA_UINT32 message_size;
	LSA_UINT32 message_max;
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, &message_max);

	if( prm_rdwr->record_data != message_body + CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE ) {
		CM_FATAL(); /* see cm_sv_r1b_pdev_B_handle_prm_read() */
	}

	if( CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE + prm_rdwr->record_data_length > message_max ) {
		CM_FATAL(); /* see cm_sv_r1b_pdev_B_handle_prm_read() */
	}

	if( prm_rdwr->record_data_length > 0xFFFF ) { /* see cast below */
		CM_FATAL();
	}

	message_size = data_offset + prm_rdwr->record_data_length;

	CM_PUT16_HTON(message_body, 0, dev->r1b.pdev_B.user_id);
	CM_PUT16_HTON(message_body, 2, (LSA_UINT16)prm_rdwr->record_data_length);
	CM_PUT16_HTON(message_body, 4, r1_response);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE == 6U);

	CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) r1b_pdev_B: prm-read-done, sl/su(%u/0x%04x), index(0x%x), record_data_length(%u), pd_response(%u)"
		, dev->device_nr, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
		, prm_rdwr->record_index, prm_rdwr->record_data_length, pd_response
		);

	/***/

	dev->r1b.pdev_B.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ_DONE, message_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B POST PRM WRITE-DONE ==================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_post_prm_write_done(
	CM_UPPER_RQB_CONST_PTR_TYPE pd,
	LSA_UINT16 pd_response,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;
	LSA_UINT16 r1_response = cm_sv_r1b_pd_rsp_to_r1_rsp(pd_response);
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);
	LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE;

	CM_PUT16_HTON(message_body, 0, dev->r1b.pdev_B.user_id);
	CM_PUT16_HTON(message_body, 2, r1_response);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE == 4U);

	CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) r1b_pdev_B: prm-write-done, sl/su(%u/0x%04x), index(0x%x), record_data_length(%u), pd_response(%u)"
		, dev->device_nr, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
		, prm_rdwr->record_index, prm_rdwr->record_data_length, pd_response
		);

	LSA_UNUSED_ARG(prm_rdwr);

	/***/

	dev->r1b.pdev_B.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE_DONE, message_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B POST PRM-BEGIN-DONE ==================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_post_prm_begin_done(
	LSA_UINT16 pd_response,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);
	LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN_DONE;

	/* note: done-message is for flow-control only (prm-begin never fails) */

	if( pd_response != CM_OK ) {
		CM_FATAL(); /* a bug */
	}

	CM_PUT16_HTON(message_body, 0, dev->r1b.pdev_B.user_id);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN_DONE == 2U);

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) r1b_pdev_B: prm-begin-done"
		, dev->device_nr
		);

	/***/

	dev->r1b.pdev_B.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN_DONE, message_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B POST PRM-END-DONE ====================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_r1b_pdev_B_post_prm_end_done(
	CM_UPPER_RQB_CONST_PTR_TYPE pd,
	LSA_UINT16 pd_response,
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {
	CM_UPPER_PD_PRM_END_PTR_TYPE prm_end = pd->args.pd.prm_end;
	LSA_UINT16 r1_response = cm_sv_r1b_pd_rsp_to_r1_rsp(pd_response);

	CM_COMMON_MEM_U8_PTR_TYPE message_body = cm_sv_r1b_get_message_body_from_prefix(prefix, LSA_NULL);
	LSA_UINT32 message_size = CM_SV_R1BMSG_SIZE_PDEV_PRM_END_DONE;

	LSA_UNUSED_ARG(prm_end->is_master_project); /* see cm_sv_r1b_pdev_A_handle_prm_end_done() */

	CM_PUT16_HTON(message_body, 0, dev->r1b.pdev_B.user_id);
	CM_PUT16_HTON(message_body, 2, r1_response);

	CM_ASSERT(CM_SV_R1BMSG_SIZE_PDEV_PRM_END_DONE == 4U);

	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) r1b_pdev_B: prm-end-done, pd_response(%u)"
		, dev->device_nr, pd_response
		);

	/***/

	dev->r1b.pdev_B.wf_posted = LSA_TRUE;

	cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_PDEV_PRM_END_DONE, message_size, prefix);
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B DONE =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_B_done( /* called from cm_sv_pdev_request_done() */
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_CONST_PTR_TYPE pd
) {
	if(
		dev->r1b.pdev_B.pd != pd
		|| dev->r1b.pdev_B.rqb != pd
		|| cm_is_null(dev->r1b.pdev_B.buffer)
		|| dev->r1b.pdev_B.wf_posted
	) {

		CM_FATAL(); /* a bug */
	}
	else {

		LSA_UINT16 pd_response = CM_RQB_GET_RESPONSE(pd);

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = dev->r1b.pdev_B.buffer;

		dev->r1b.pdev_B.pd = LSA_NULL;

		/***/

		switch( CM_RQB_GET_OPCODE(pd) ) {

		case CM_OPC_PD_PRM_READ_SERVER:
			cm_sv_r1b_pdev_B_post_prm_read_done(pd, pd_response, dev, prefix);
			break;

		case CM_OPC_PD_PRM_WRITE_SERVER:
			cm_sv_r1b_pdev_B_post_prm_write_done(pd, pd_response, dev, prefix);
			break;

		case CM_OPC_PD_PRM_BEGIN_SERVER:
			cm_sv_r1b_pdev_B_post_prm_begin_done(pd_response, dev, prefix);
			break;

		case CM_OPC_PD_PRM_END_SERVER:
			cm_sv_r1b_pdev_B_post_prm_end_done(pd, pd_response, dev, prefix);
			break;

		default:
			CM_FATAL(); /* a bug */

		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B POSTED ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_B_posted(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE prefix
) {

	if(
		! dev->r1b.pdev_B.wf_posted
		|| prefix != dev->r1b.pdev_B.buffer
	) {

		CM_FATAL(); /* bug */
	}
	else {

		dev->r1b.pdev_B.wf_posted = LSA_FALSE;

		/***/

		cm_sv_r1b_pdev_B_next(dev);  /* trigger: posted (read-done, write-done, or prm-end-done) */
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B CANCEL ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_pdev_B_cancel( /* called on bridge-broken */
	CM_SV_DEVICE_PTR_TYPE dev
) {

	if( cm_is_not_null(dev->r1b.pdev_B.br_next) ) {

		CM_UPPER_RQB_PTR_TYPE rb = dev->r1b.pdev_B.br_next;
		dev->r1b.pdev_B.br_next = LSA_NULL;

		cm_callback(dev->channel, CM_OK, &rb); /* note: could be CM_OK_CANCELLED for clarity; but: needs doc-change and no benefit for the user */
	}

	if( cm_is_not_null(dev->r1b.pdev_B.pd) ) {

		/* do nothing (cannot be cancelled) */
	}

	if( dev->r1b.pdev_B.wf_posted ) {

		/* do nothing (cannot be cancelled) */
	}

	/* note: see too cm_pdsv_occupant_set_reset() regarding CM_SV_R1B_PDEV_B_AR_NR */
}

/*===========================================================================*/
/*=== R1-BRIDGE PDEV-B IS-IDLE ==============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_r1b_pdev_B_is_idle(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
) {

	if(
		cm_is_not_null(dev->r1b.pdev_B.br_next)
		|| cm_is_not_null(dev->r1b.pdev_B.pd)
		|| dev->r1b.pdev_B.wf_posted
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
