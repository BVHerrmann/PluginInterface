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
/*  F i l e               &F: cm_sv43.c                                 :F&  */
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
/*    (read-record, write-record)                                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7430
#define CM_MODULE_ID       7430

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== PARSE HEADER ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_parse_header(
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_PTR_TYPE req,
	LSA_BOOL can_RecordDataReadQuery
) {
	CM_UPPER_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_len;

	/***/

	#if CLRPC_PNIO_OFFSET + CM_BLK_RDWR_SIZE != CM_RECORD_OFFSET
	#error "check this!"
	#endif

	/***/

	data_ptr = rpc->args.sv.call->ndr_data;
	data_len = rpc->args.sv.call->ndr_len;

	if( req->is_read ) { /* read or read-implicit */

		/* IODReadReq ::= IODReadReqHeader, [RecordDataReadQuery] */

		if( data_len < CM_RECORD_OFFSET ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "read, ndr_len(%u) too short"
				, data_len
				);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* read, wrong length */
		}
	}
	else { /* write */

		/* IODWriteReq ::= IODWriteHeader, RecordDataWrite */

		if( data_len < CM_RECORD_OFFSET ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "write, ndr_len(%u) too short"
				, data_len
				);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* write, too few data */
		}
	}

	data_ptr += CLRPC_PNIO_OFFSET;
	data_len -= CLRPC_PNIO_OFFSET;

	/***/

	{
	LSA_UINT16 block_type;
	LSA_UINT16 block_length;
	LSA_UINT16 block_version;

	block_type    = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);
	block_length  = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);
	block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	/***/

	if( req->is_read ) {

		if( block_type != CM_BLOCK_TYPE_READ_REQ ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "read, wrong block_type(%u)"
				, block_type
				);
			return CM_PNIO_FAULTY_Record__Block(0); /* read, wrong block-type */
		}
	}
	else { /* write */

		if( block_type != CM_BLOCK_TYPE_WRITE_REQ ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "write, wrong block_type(%u)"
				, block_type
				);
			return CM_PNIO_FAULTY_Record__Block(0); /* write, wrong block-type */
		}
	}

	if( block_length != CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE) {

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "wrong block_length(%u)"
				, block_length
				);
		return CM_PNIO_FAULTY_Record__Block(1); /* wrong block-length*/
	}

	if( (block_version & 0xFF00) != 0x0100/*V1.0*/ ) {

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "wrong block_version.high(0x%x)"
				, block_version
				);
		return CM_PNIO_FAULTY_Record__Block(2); /* wrong block-version.high */
	}

	if( (block_version & 0x00FF) != 0x0000/*V1.0*/ ) {

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "wrong block_version.low(0x%x)"
				, block_version
				);
		return CM_PNIO_FAULTY_Record__Block(3); /* wrong block-version.low */
	}
	}

	/***/

	req->seq_number  = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_seq_number);

	CM_GET_UUID_NTOH(&req->ar_uuid, data_ptr, CM_BLK_RDWR_ar_uuid);

	req->api         = CM_GET32_NTOH(data_ptr, CM_BLK_RDWR_api);
	req->slot_nr     = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_slot_nr);
	req->subslot_nr  = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_subslot_nr);

	req->padding1    = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_padding_1);
	req->index       = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_index);

	req->record_data_length = CM_GET32_NTOH(data_ptr, CM_BLK_RDWR_record_data_length);

	req->target_ar_uuid_ptr = data_ptr + CM_BLK_RDWR_req_rd_implicit_target_ar_uuid;

	data_ptr += CM_BLK_RDWR_SIZE;
	data_len -= CM_BLK_RDWR_SIZE;

	/***/

	if( req->padding1 != 0 ) {

		/* NOTE: 'index' maybe be expanded to 32 bit in the future */
		/* to allow this, we have to check the padding (the HighWord) for zero */

		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
				, "padding_1 not zero"
				);
		return CM_PNIO_FAULTY_Record__InvalidRange(9); /* padding_1 not zero */
	}

	/***/

	req->record_data_length += CM_RECORD_OFFSET; /* include the rpc- and record-header, see cm_sv_record_event_indicate() */

	if( req->record_data_length < CM_RECORD_OFFSET ) { /* wrap around at 4GB */

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "record-length(%u) too big"
				, req->record_data_length - CM_RECORD_OFFSET
				);
		return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* record-length inconsistent with ndr-length */
	}

	if( req->is_read ) {

		if( req->record_data_length > rpc->args.sv.call->alloc_len ) {

			/* we must accept this record-read, but we need to shorten the length indicated to the user */

			CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH /* not LEVEL_UNEXP */
				, "read, %u + length(%u) > alloc_len(%u), shortening the request"
				, CM_RECORD_OFFSET
				, req->record_data_length - CM_RECORD_OFFSET
				, rpc->args.sv.call->alloc_len
				);

			req->record_data_length = rpc->args.sv.call->alloc_len;
		}

		/***/

		if( data_len != 0 ) { /* RecordDataReadQuery follows */

			LSA_UINT16 block_type;
			LSA_UINT16 block_length;
			LSA_UINT16 block_version;

			if( ! can_RecordDataReadQuery ) {

				CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
					, "readquery, but not supported"
					);
				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks(); /* readquery, but not supported */
			}

			if( data_len < CM_BLK_HEADER_SIZE ) {

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "readquery, ndr_len(%u) too short"
					, rpc->args.sv.call->ndr_len
					);
				return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* readquery, too short */
			}

			block_type    = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);
			block_length  = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);
			block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

			if( block_type != CM_BLOCK_TYPE_READQUERY ) {

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "readquery, wrong block_type(%u)"
					, block_type
					);
				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks(); /* readquery, wrong block-type */
			}

			if( 0UL + 2+2 + block_length != data_len ) {

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
						, "readquery, wrong block_length(%u)"
						, block_length
						);
				return CM_PNIO_FAULTY_Record__Block(1); /* readquery, wrong block-length*/
			}

			if( block_length < 1+1 + 1/*!*/ ) { /* see PNIO-Spec, at least one data-byte */

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
						, "readquery, block_length(%u) too short"
						, block_length
						);
				return CM_PNIO_FAULTY_Record__Block(1); /* readquery, block-length too short*/
			}

			if( (block_version & 0xFF00) != 0x0100/*V1.0*/ ) {

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
						, "readquery, wrong block_version.high(0x%x)"
						, block_version
						);
				return CM_PNIO_FAULTY_Record__Block(2); /* readquery, wrong block-version.high */
			}

			if( (block_version & 0x00FF) != 0x0000/*V1.0*/ ) {

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
						, "readquery, wrong block_version.low(%u)"
						, block_version
						);
				return CM_PNIO_FAULTY_Record__Block(3); /* readquery, wrong block-version.low */
			}
		}

		req->readquery_length = data_len; /* 0 or the length of the RecordDataReadQuery-block */
	}
	else { /* write */

		if( req->record_data_length != rpc->args.sv.call->ndr_len ) {

			CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP
				, "write, %u + length(%u) != ndr_len(%u), inconsistency"
				, CM_RECORD_OFFSET
				, req->record_data_length - CM_RECORD_OFFSET
				, rpc->args.sv.call->ndr_len
				);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* write, record-length inconsistent with ndr-length */
		}

		req->readquery_length = 0; /* sanity */
	}

	/***/

	return CM_PNIO_ERR_NONE; /* ok */
}

/*===========================================================================*/
/*=== UPDATE HEADER =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_record_update_header(
	CM_UPPER_MEM_U8_PTR_TYPE header_ptr,
	LSA_BOOL is_read,
	LSA_UINT32 ndr_length,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
) {

	CM_ASSERT(CM_RECORD_OFFSET == CLRPC_PNIO_OFFSET + CM_BLK_RDWR_SIZE);

	/***/

	if( is_read ) {
		CM_PUT16_HTON(header_ptr, CM_BLK_HEADER_type, CM_BLOCK_TYPE_READ_RSP);
	}
	else {
		CM_PUT16_HTON(header_ptr, CM_BLK_HEADER_type, CM_BLOCK_TYPE_WRITE_RSP);
	}

	CM_PUT16_HTON(header_ptr, CM_BLK_HEADER_length, CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE);

	CM_PUT16_HTON(header_ptr, CM_BLK_HEADER_version, 0x0100/*V1.0*/);

	/*
	 * update the length
	 */

	if( ndr_length == 0 ) {

		/* AP00786836, see cm_sv_record_write_embedded_update() */
	}
	else {

		CM_PUT32_HTON(
			header_ptr
			, CM_BLK_RDWR_record_data_length
			, ndr_length - CM_RECORD_OFFSET
		);
	}

	/*
	 * update the additional-values
	 */

	CM_PUT16_HTON(
		header_ptr
		, CM_BLK_RDWR_rsp_rd_wr_additional_value_1
		, ret_val_1
	);

	CM_PUT16_HTON(
		header_ptr
		, CM_BLK_RDWR_rsp_rd_wr_additional_value_2
		, ret_val_2
	);

	/*
	 * zero out (target_uuid!) before updating
	 */

	CM_MEMSET(
		header_ptr + CM_BLK_RDWR_rsp_rd_wr_padding_2,
		0,
		20
		);

	/*
	 * write-rsp: update the pnio-status
	 */

	if( ! is_read ) {

		CM_PUT32_HTON(
			header_ptr
			, CM_BLK_RDWR_rsp_wr_pnio_status
			, pnio_err
		);
	}
}

/*===========================================================================*/
/*=== CHECK WRITE-MULTIPLE-BODY =============================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_check_write_multiple_body(
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	LSA_UINT16 *record_cnt
) {
	CM_UPPER_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_len;
	LSA_UINT16 seq;

	/***/

	CM_ASSERT(rpc->args.sv.call->ndr_len >= CM_RECORD_OFFSET); /* see cm_sv_record_parse_header() */

	data_ptr = rpc->args.sv.call->ndr_data + CM_RECORD_OFFSET;
	data_len = rpc->args.sv.call->ndr_len  - CM_RECORD_OFFSET;

	/***/

	if( data_len == 0 ) {

		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
				, "no embedded records"
				);
		return CM_PNIO_FAULTY_Record__Block(11); /* consistent write-multiple but no embedded records */
	}

	/***/

	seq = req->seq_number;

	while( data_len > 0 ) {

		LSA_UINT16 tmp16;
		LSA_UINT32 tmp32;
		CLRPC_UUID_TYPE tmpUUID;

		/* another header */

		seq += 1;

		/* align next header on 32 bits */

		tmp32 = ((4 - ((LSA_UINT32)data_ptr & 3)) & 3);

		if( tmp32 > data_len ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
					, "alignment too big"
					);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* inconsistent write-multiple */
		}

		data_ptr += tmp32;
		data_len -= tmp32;

		/***/

		if( data_len < CM_BLK_RDWR_SIZE ) {

			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
					, "too short for a write"
					);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* inconsistent write-multiple */
		}

		tmp16 = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);

		if( tmp16 != CM_BLOCK_TYPE_WRITE_REQ ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_type(%u)"
					, tmp16
					);
			return CM_PNIO_FAULTY_Record__Block(0); /* wrong block-type */
		}

		tmp16 = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);

		if( tmp16 != CM_BLK_RDWR_SIZE - CM_BLK_HEADER_TypLen_SIZE ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_length(%u)"
					, tmp16
					);
			return CM_PNIO_FAULTY_Record__Block(1); /* wrong block-length */
		}

		tmp16 = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

		if( (tmp16 & 0xFF00) != 0x0100/*V1.0*/ ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_version.high(0x%x)"
					, tmp16
					);
			return CM_PNIO_FAULTY_Record__Block(2); /* wrong block-version.high */
		}

		if( (tmp16 & 0x00FF) != 0x0000/*V1.0*/ ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "wrong block_version.low(0x%x)"
					, tmp16
					);
			return CM_PNIO_FAULTY_Record__Block(3); /* wrong block-version.low */
		}

		/***/

		tmp16 = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_seq_number);

		if( tmp16 != seq ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "wrong seq_number(%u)"
					, tmp16
					);
			return CM_PNIO_FAULTY_Record__Block(4); /* wrong seq-number */
		}

		CM_GET_UUID_NTOH(&tmpUUID, data_ptr, CM_BLK_RDWR_ar_uuid);

		if( ! cm_uuid_equal(&tmpUUID, &req->ar_uuid) ) {

			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
					, "not the same ar_uuid"
					);
			return CM_PNIO_FAULTY_Record__Block(5); /* not the same ar_uuid */
		}

		/*
		 * NOTE: api/slot/subslot/index out-of-range is treated
		 *       like an "user-error", not as a body-inconsistency
		 *
		 * NOTE: with the following exceptions
		 */

		tmp16 = CM_GET16_NTOH(data_ptr,CM_BLK_RDWR_padding_1);

		if( tmp16 != 0 ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "padding_1(%u) not zero"
					, tmp16
					);
			return CM_PNIO_FAULTY_Record__InvalidRange(9); /* padding_1 not zero */
		}

		tmp16 = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_index);

		if( tmp16 == 0xE040 ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "index(%u) E040 not allowed in embedded record"
					, tmp16
					);
			return CM_PNIO_FAULTY_Record__Block(10); /* index E040 not allowed */
		}

		tmp32 = CM_GET32_NTOH(data_ptr, CM_BLK_RDWR_record_data_length);

		/***/

		data_ptr += CM_BLK_RDWR_SIZE;
		data_len -= CM_BLK_RDWR_SIZE;

		/***/

		if( tmp32 > data_len ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "record_data_length(%u) too big"
					, tmp32
					);
			return CM_PNIO_FAULTY_Record__CMRPC_ArgsLengthInvalid(); /* inconsistent write-multiple */
		}

		data_ptr += tmp32;
		data_len -= tmp32;

		/***/

		*record_cnt += 1;
	}

	/***/

	return CM_PNIO_ERR_NONE; /* ok */
}

/*===========================================================================*/
/*=== CHECK RANGE ===========================================================*/
/*===========================================================================*/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("BEAUTIFY: use index-classification already here (check range)")
#endif /* CM_MESSAGE */

static LSA_UINT32
cm_sv_record_check_range( /* called for reads and writes */
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_PTR_TYPE req,
	CM_SV_SUBSLOT_PTR_TYPE * su_ptr
) {
	/* read, read-implicit, and write is restricted to range */

	/* see PNIO-Spec, Part 5, "Error code hierarchy of the Record Data object" */

	if( req->index < 0xE000 ) {

		/* ap must exist */

		req->ap = cm_sv_ap_lookup(dev, req->api);

		if( cm_is_null(req->ap) || ! CM_SV_AP_IS_PLUGGED(req->ap) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "api(%u) out of range or not plugged"
				, req->api
				);
			return CM_PNIO_FAULTY_Record__InvalidArea(6); /* api out of range or not plugged */
		}
	}
	else if( req->index < 0xF000 ) {

		if( cm_is_null(req->argr) ) { /* case: implicit-read */

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "index(0x%x) needs target_uuid"
				, req->index
				);
			return CM_PNIO_FAULTY_Record__InvalidParameter(12); /* index needs target_uuid */
		}

		req->api = 0xFFFFFFFF; /* sanity */

		req->ap = LSA_NULL; /* see iterator */
	}
	else if( req->index < 0xF800 ) {

		/* ap must exist */

		req->ap = cm_sv_ap_lookup(dev, req->api);

		if( cm_is_null(req->ap) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "api(%u) out of range or not plugged"
				, req->api
				);
			return CM_PNIO_FAULTY_Record__InvalidArea(6); /* api out of range or not plugged */
		}
		else if( CM_SV_AP_IS_PLUGGED(req->ap) ) {

			/* ok, plugged */
		}
		else if( req->index == 0xF020 ) {

			/* special case: 0xF020 shall return ARData even if the ap is not plugged */
		}
		else {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "api(%u) out of range or not plugged"
				, req->api
				);
			return CM_PNIO_FAULTY_Record__InvalidArea(6); /* api out of range or not plugged */
		}
	}
	else {

		/* API shall be ignored */

		req->api = 0xFFFFFFFF; /* sanity */

		req->ap = LSA_NULL;
	}

	/***/

	{
	CM_SV_SLOT_PTR_TYPE sl;
	CM_SV_SUBSLOT_PTR_TYPE su;

	if( req->index < 0xE000 ) {

		/* slot must be within range */

		if( cm_is_null(req->ap) ) {
			CM_FATAL(); /* a bug, see above */
		}

		sl = cm_sv_slot_lookup(req->ap, req->slot_nr);

		if( cm_is_null(sl) || ! CM_SV_SLOT_IS_PLUGGED(sl) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "slot(%u) out of range or not plugged"
				, req->slot_nr
				);
			return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(7); /* slot out of range or not plugged */
		}
	}
	else {

		req->slot_nr = 0xFFFF; /* sanity */

		sl = LSA_NULL; /* sanity */
	}

	if( req->index < 0xC000 ) {

		/* subslot must be within range */

		if( cm_is_null(req->ap) ) {
			CM_FATAL(); /* a bug, see above */
		}

		if( cm_is_null(sl) ) {
			CM_FATAL(); /* a bug, see above */
		}

		su = cm_sv_subslot_lookup(sl, req->subslot_nr);

		if( cm_is_null(su) || ! CM_SV_SUBSLOT_IS_PLUGGED(su) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "subslot(%u) out of range or not plugged"
				, req->subslot_nr
				);
			return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(8); /* subslot out of range or not plugged */
		}

		if( cm_is_not_null(su_ptr) ) {

			*su_ptr = su;
		}
	}
	else {

		req->subslot_nr = 0xFFFF; /* sanity */

		su = LSA_NULL; /* sanity */
	}
	}

	/***/

	if (!dev->gsd_AssetManagementSupported
		&& req->index == 0xF880
	) {
		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "asset management not supported by device -> return invalid index(0x%x)"
				, req->index
				);
		return CM_PNIO_FAULTY_Record__InvalidIndex(0); /* see US-AMR */
	}

	/***/

	return CM_PNIO_ERR_NONE; /* ok */
}

/*===========================================================================*/
/*=== CHECK SUBSLOT =========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_check_subslot(
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE *ar_sub,
	CM_SV_SUBSLOT_PTR_TYPE *su
) {
	if( (req->cls & (CM_REC_IDX_0000 | CM_REC_IDX_8000 | CM_REC_IDX_C000 | CM_REC_IDX_F000)) != 0 ) {

		CM_AR_GRAPH_AP_PTR_TYPE ar_ap = cm_ar_graph_find_ap(argr, req->api);

		if( cm_is_null(ar_ap) ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "api(%u) not within ar-context"
				, req->api
				);
			return CM_PNIO_FAULTY_Record__InvalidArea(6); /* api not withing ar-context */
		}

		if( !CM_SV_AP_IS_PLUGGED(ar_ap->sv.real_ap) ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "api(%u) not plugged"
				, req->api
				);
			return CM_PNIO_FAULTY_Record__InvalidArea(6); /* api not plugged */
		}

		if( (req->cls & (CM_REC_IDX_0000 | CM_REC_IDX_8000 | CM_REC_IDX_C000)) != 0 ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod = cm_ar_graph_find_module(ar_ap, req->slot_nr);

			if( cm_is_null(ar_mod) ) {

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "api(%u) slot(%u) not within ar-context"
					, req->api, req->slot_nr
					);
				return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(7); /* slot not withing ar-context */
			}

			if( !CM_SV_SLOT_IS_PLUGGED(ar_mod->sv.real_sl) ) {

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "api(%u) slot(%u) not plugged"
					, req->api, req->slot_nr
					);
				return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(7); /* slot not plugged */
			}

			if( (req->cls & (CM_REC_IDX_0000 | CM_REC_IDX_8000)) != 0 ) {

				*ar_sub = cm_ar_graph_find_submodule(LSA_NULL, ar_mod, 0, req->subslot_nr);

				if( cm_is_null(*ar_sub) ) {

					CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "ap/sl/su(%u/%u/%u) not within ar-context"
						, req->api, req->slot_nr, req->subslot_nr
						);
					return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(8); /* subslot not withing ar-context */
				}

				if( !CM_SV_SUBSLOT_IS_PLUGGED((*ar_sub)->sv.real_su) ) {

					CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "ap/sl/su(%u/%u/%u) not plugged"
						, req->api, req->slot_nr, req->subslot_nr
						);

					return CM_PNIO_FAULTY_Record__InvalidSlotSubslot(8); /* subslot not plugged */
				}

				*su = (*ar_sub)->sv.real_su;
			}
		}
	}
	else { /* CM_REC_IDX_E000 or CM_REC_IDX_F800 */

		/* api/slot/subslot is don't care */
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== CHECK WRITABLE ========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_check_writable(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 cls,
	LSA_UINT16 index,
	LSA_BOOL is_COC
) {
	if( (cls & CM_REC_IDX_WRITE) == 0 && !is_COC ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev/ar(%u/%u) index(0x%x) is reserved or not writeable"
			, argr->device_nr, argr->ar_nr
			, index
			);

		LSA_UNUSED_ARG(argr);
		LSA_UNUSED_ARG(index);

		if( (cls & CM_REC_IDX_READ) != 0 ) { /* AP01566285: access denied if readonly record */

			return CM_PNIO_FAULTY_Record__AccessDenied(10);
		}
		else {

			return CM_PNIO_FAULTY_Record__InvalidIndex(10);
		}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== CHECK IM0 PE ==========================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_check_IM0_PE(
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_SV_SUBSLOT_CONST_PTR_TYPE su
) {
	if( !CM_SV_SUBSLOT_IS_PLUGGED(su) ) {

		return CM_PNIO_FAULTY_Record__AccessDenied(8); /* unreachable */
	}

	if( (req->cls & CM_REC_IDX_IM1234) != 0 ) {

		if( (su->im0_bits & CM_SV_IM0_BITS_SUBMODULE) == 0 ) {

			return CM_PNIO_FAULTY_Record__AccessDenied(8); /* submodule is not carrier of I&M data (AP01520725 updates AP01467022) */
		}
	}

	if( req->index == 0x80A0 /* PROFIenergy Record_0 */ ) { /* TIA 1094128 */

		if( (su->im0_bits & CM_SV_IM0_BITS_PROFIENERGY) == 0 ) {

			return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* submodule does not support PROFIenergy */
		}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== CHECK COC =============================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_check_coc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
) {
	/* note: api/slot/subslot are within ar-context, subslot may be pulled */

	if(
		cm_is_null(ar_sub) /* always false */
		|| req->api != 0
		|| !CM_SV_SUBSLOT_IS_PDEV_INTERFACE(ar_sub->subslot_nr)
	) {

		CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) COC: not the pdev-interface"
			, argr->device_nr, argr->ar_nr
			, req->api, req->slot_nr, req->subslot_nr
			);

		/* note: not cm_sv_pdev_write_failed() because PDEV access point is not addressed */

		return CM_PNIO_FAULTY_Record__AccessDenied(10); /* unsupported COC access point */
	}

	if( (argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) == 0 ) {

		cm_sv_pdev_write_failed(argr, req->cls, req->slot_nr, req->subslot_nr, req->index, 0);

		return CM_PNIO_FAULTY_Record__InvalidIndex(0); /* note: COC was not defined prior to CM_AR_PROP_COMBINED_OBJECT_CONTAINER */
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== CHECK FORBIDDEN WINDOW ================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_check_forbidden_window(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_REQ) == 0 ) {

		/*
		 * forbidden window between prm-end.ind and appl-ready.req
		 * (history: AP00305349, check was wrong)
		 *
		 * see PNIO-Spec, CMWREC/CMWRR state table
		 * writing to a submodule that is being parameterized is forbidden
		 * - connect-phase:
		 * -- api/slot/subslot == any: CNF(-, PNIORW, state conflict) + not aborting the AR
		 * - plug-phase:
		 * -- api/slot/subslot == plug-alarm: CNF(-, PNIORW, state conflict) + not aborting the AR
		 * -- api/slot/subslot != plug-alarm: continue
		 *
		 * see PNIO-Spec, CMRREC/CMRDR state table
		 * reading is always possible
		 */

		LSA_BOOL ok = LSA_TRUE;

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

			ok = LSA_FALSE; /* forbidden: all submodules are parameterized */
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) {

			if( cm_is_not_null(ar_sub) && (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

				ok = LSA_FALSE; /* forbidden: this submodule is parameterized */
			}
		}
		else { /* data-phase */

			CM_UPPER_ALARM_PTR_TYPE al = argr->cm.sv.plug_group_alarm;

			if(
				cm_is_not_null(al)
				&& al->api == api
				&& al->slot_nr == slot_nr
				&& al->subslot_nr == subslot_nr
			) {

				ok = LSA_FALSE; /* forbidden: this submodule is parameterized */
			}
		}

		if( !ok ) {

			CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev/ar(%u/%u) write between prm-end.ind and appl-ready.req is not allowed, ap/sl/su(%u/%u/%u) ar_flags(0x%08x)"
				, argr->device_nr, argr->ar_nr
				, api, slot_nr, subslot_nr
				, argr->cm.sv.ar_flags
				);
			return CM_PNIO_FAULTY_Record__StateConflict(5); /* state conflict (telling something wrong with aruuid) */
		}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== CHECK PDEV ============================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_check_pdev(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 cls,
	LSA_UINT16 index,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
) {
	if( cm_is_null(ar_sub) ) { /* always false */

		return CM_PNIO_FAULTY_Record__AccessDenied(10); /* unreachable */
	}
	else if( (cls & CM_REC_IDX_NONPRM) != 0 ) {

		/* non-prm indices can be written always (ownership not necessary) */
	}
	else if( !cm_sv_ar_is_pdev_owner(argr) ) { /* note: easy-supervisor is never the pdev-owner */

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev/ar(%u/%u) pdev-write but not pdev-owner"
			, argr->device_nr, argr->ar_nr
			);
		return CM_PNIO_FAULTY_Record__AccessDenied(10); /* not the pdev-owner */
	}
	else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) { /* connect-phase */

		/* ok */
	}
	else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) { /* prm-begin-end */

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PDEV) == 0 ) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev/ar(%u/%u) pdev-write but pdev is not being prm-ed"
				, argr->device_nr, argr->ar_nr
				);
			return CM_PNIO_FAULTY_Record__AccessDenied(10); /* pdev is not being prm-ed */
		}
	}
	else { /* data-phase */

		CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub = cm_sv_ar_almi_plug_or_release(argr);

		if(
			ar_sub == plg_rel_sub /* write is related to the current plug-alarm */
			&& CM_SV_SUBSLOT_IS_PDEV(plg_rel_sub->subslot_nr)
		) { /* plug-pdev */

			/* history: V6.0 supports only plug-port (and any record-write is an error; parameters come with the COC) */

			/* note: V6.1 supports plug-port (local and remote) and plug-interface (only remote) */
			/*       a record-write to a port is an error; parameters come with the COC) */

			if( CM_SV_SUBSLOT_IS_PDEV_PORT(plg_rel_sub->subslot_nr) ) {

				CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_WARN
					, "dev/ar(%u/%u) plug-pdev subslot_nr(0x%04x): unexpected record index(0x%04x), ar_properties(0x%08x)"
					, argr->device_nr, argr->ar_nr
					, plg_rel_sub->subslot_nr, index, argr->ar.ar_properties
					);

				LSA_UNUSED_ARG(index);

				cm_ar_com_down(argr, CM_AR_REASON_COC);

				return CM_PNIO_FAULTY_Record__InvalidIndex(0); /* unexpected pdev-write */
			}
		}
		else { /* not plug-pdev */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev/ar(%u/%u) pdev-write is not allowed in data-phase"
				, argr->device_nr, argr->ar_nr
				);
			return CM_PNIO_FAULTY_Record__AccessDenied(10); /* pdev-write is not allowed in data-phase */
		}
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== CHECK RANGE AND CONTEXT ===============================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_record_check_range_and_context( /* only called for writes */
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_SV_RECORD_PARAMS_PTR_TYPE req,
	LSA_BOOL is_COC
) {
	/*
	 * write is restricted to range and ar-context
	 *
	 * IOSAR with device-access (easy-supervisor): ar-context means "all", pdev-prm-indices cannot be written
	 *
	 * see PNIO-Spec, Part 5, "Error code hierarchy of the Record Data object"
	 * a) read:  no TargetARUUID --> access / invalid parameter
	 * b) write: is backup --> access / backup
	 * c) rd/wr: no API --> access / invalid area
	 * d) rd/wr: no slot --> access / invalid slot
	 * e) rd/wr: no subslot --> access / invalid subslot (note: same code as invalid slot)
	 * f) rd/wr: unknown index --> access / invalid index
	 * g) write: no write-access --> access / access denied (note: 3 other codes possible)
	 * h) read:  no read-access --> access / access denied (note: 3 other codes possible)
	 * note: clauses i) to n) are not handled in this function
	 */

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		cm_sv_arset_check_primary(argr);

		if(
			! cm_sv_arset_is_primary_or_first(argr)
			|| (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_BACKUP) != 0 /* TIA 2329293 */
		) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) not primary/first AR of the AR-set"
				, argr->device_nr, argr->ar_nr
				);
			return CM_PNIO_FAULTY_Record__Backup(0); /* not primary/first AR of the AR-set (see PNIO-Spec, CMWRR) */
		}
	}

	/***/

	if( cm_is_iosar_with_device_access(argr) ) {

		CM_SV_SUBSLOT_PTR_TYPE su = LSA_NULL;

		/* easy-supervisor: ar-context means "all" (i.e., within range of real-config) */

		LSA_UINT32 pnio_err = cm_sv_record_check_range(argr->cm.sv.dev, req, &su);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			return pnio_err;
		}

		pnio_err = cm_sv_record_check_writable(argr, req->cls, req->index, is_COC);

		if( is_COC ) {

			pnio_err = CM_PNIO_FAULTY_Record__AccessDenied(10); /* unsupported */
		}

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			return pnio_err;
		}

		if( (req->cls & (CM_REC_IDX_0000 | CM_REC_IDX_8000)) != 0 ) {

			if( cm_is_null(su) ) {

				return CM_PNIO_FAULTY_Record__InvalidIndex(10);	/* unreachable (see cm_sv_record_check_range() above) */
			}

			pnio_err = cm_sv_record_check_IM0_PE(req, su);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				return pnio_err;
			}

			if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) {

				if( (req->cls & CM_REC_IDX_PDEV) != 0 ) {

					if( (req->cls & CM_REC_IDX_NONPRM) == 0 ) {

						return CM_PNIO_FAULTY_Record__InvalidIndex(10);
					}
				}
			}
		}
		else { /* not subslot-specific */

			return CM_PNIO_FAULTY_Record__AccessDenied(10); /* e.g. 0xE050 */
		}
	}
	else {

		/* note: using ar_xxx->sv.real_xxx instead of cm_sv_record_check_range() */

		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub = LSA_NULL;
		CM_SV_SUBSLOT_PTR_TYPE su = LSA_NULL;

		LSA_UINT32 pnio_err = cm_sv_record_check_subslot(req, argr, &ar_sub, &su);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			return pnio_err;
		}

		/***/

		if( is_COC ) {

			pnio_err = cm_sv_record_check_coc(argr, req, ar_sub);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				return pnio_err;
			}
		}

		/***/

		if( (req->cls & CM_REC_IDX_PDEV) == 0 ) {

			pnio_err = cm_sv_record_check_writable(argr, req->cls, req->index, is_COC);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				if( cm_is_not_null(ar_sub) && CM_SV_SUBSLOT_IS_PDEV(ar_sub->subslot_nr) ) {

					cm_sv_pdev_write_failed(argr, req->cls, req->slot_nr, req->subslot_nr, req->index, 0);
				}

				return pnio_err;
			}
		}

		/***/

		if( cm_is_null(ar_sub) ) { /* slot, api, AR, device */

			if( (req->cls & CM_REC_IDX_E000) != 0 ) { /* AR specific */

				if( CM_SV_AR_IS_ADVANCED(argr) ) {

					if( req->index == 0xE061/*RS_AckEvent*/ ) {

						/* more checks in cm_sv_ar_record_indicate() */
					}
					else {

						return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* AP01314661 */
					}
				}
				else { /* legacy-AR */

					if( req->index == 0xE050/*ARFSUDataAdjust*/ ) {

						if( !cm_sv_ar_is_pdev_owner(argr) ) {

							/* AP00835094: cnf(-) if the AR is not the pdev-owner, set a.r.p. on all submodules */

							if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

								CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_WARN
									, "prm-write index(0x%x) failed, setting all submodules to appl-ready-pending"
									, req->index
									);

								argr->cm.sv.ar_flags |= CM_SV_AR_FL_ARP_E000; /* see cm_sv_marshal_module_diff_block() */
							}

							return CM_PNIO_FAULTY_Record__InvalidIndex(10);
						}
					}
					else {

						return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* undefined index for legacy-AR (must not indicate to user) */
					}
				}
			}
		}
		else { /* i.e., CM_REC_IDX_0000 or CM_REC_IDX_8000 */

			su = ar_sub->sv.real_su;

			if(
				!CM_SV_SUBSLOT_IS_PLUGGED(su) /* note: always plugged, except if COC (see above) */
				|| su->owner_ar_nr != argr->ar_nr /* not owned by this AR */
			) {

				CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) not owner(%u)"
					, argr->device_nr, argr->ar_nr
					, req->api, req->slot_nr, req->subslot_nr
					, su->owner_ar_nr
					);

				if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) {

					cm_sv_pdev_write_failed(argr, req->cls, req->slot_nr, req->subslot_nr, req->index, 0);
				}

				return CM_PNIO_FAULTY_Record__AccessDenied(8); /* subslot is "so-locked" or "not-owned" */
			}

			/***/

			pnio_err =  cm_sv_record_check_IM0_PE(req, su);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				return pnio_err;
			}

			/***/

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) != 0 ) {

				if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE_PLG_REL) != 0 ) {

					return CM_PNIO_FAULTY_Record__AccessDenied(8); /* AP01208170: cnf(-) while prm-begin-end */
				}
			}
		}

		/***/

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_PRM_END_IND) != 0 ) {

			pnio_err = cm_sv_record_check_forbidden_window(argr, ar_sub, req->api, req->slot_nr, req->subslot_nr);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				return pnio_err;
			}
		}

		/***/

		if( (req->cls & CM_REC_IDX_PDEV) != 0 || is_COC ) { /* see cm_sv_pdev_read_write() */

			pnio_err = cm_sv_record_check_pdev(argr, req->cls, req->index, ar_sub);

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				return pnio_err;
			}
		}

		/***/

		if( cm_is_not_null(ar_sub) && CM_SV_SUBSLOT_IS_PDEV(ar_sub->subslot_nr) ) { /* TIA 1582512 */

			if( (req->cls & CM_REC_IDX_PDEV) != 0 && (req->cls & CM_REC_IDX_NONPRM) == 0 ) {

				if( (argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) != 0 ) {

					if( argr->cm.sv.rec.coc_offs == 0 ) { /* a buggy IOC does a non-COC prm-write to the PDEV COC-group */

						return CM_PNIO_FAULTY_Record__AccessDenied(10); /* just CNF(-), no PRM-Diag */
					}
				}
			}
		}
	}

	/***/

	return CM_PNIO_ERR_NONE; /* ok */
}

/*===========================================================================*/
/*=== INDICATE A RECORD-EVENT ===============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_record_indicate(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_AR_GRAPH_CONST_PTR_TYPE argr, /* LSA_NULL if implicit-AR */
	LSA_BOOL via_CombinedObjectContainer, /* write: record transferred via COC (TIA 1582512) */
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_UPPER_MEM_U8_PTR_TYPE ndr_data,
	LSA_UINT32 *pnio_err
) {
	CM_CHANNEL_PTR_TYPE channel = dev->channel;

	CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) sk(0x%x) is_read(%u) index(0x%04x)"
		, dev->device_nr
		, (argr == LSA_NULL ? 0 : argr->ar_nr)
		, (argr == LSA_NULL ? 0 : argr->session_key)
		, req->is_read
		, req->index
		);

	/***/

	if( ! req->is_read && req->index == 0xE050/*ARFSUDataAdjust*/ ) {

		if(
			cm_is_null(req->argr) /* always false */
			|| CM_SV_AR_IS_ADVANCED(req->argr)
		) {

			*pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* note: advanced ARs use the IODConnectReq::ARFSUBlock (AP01034170) */

			return LSA_TRUE; /* fin */
		}

		if( ! cm_sv_ARFSUDataAdjust_check(ndr_data, req->record_data_length, LSA_FALSE) ) {

			/* AP00511230: cnf(-) if block-structure is wrong */

			*pnio_err = cm_pd_response_to_pnio_err(CM_ERR_PRM_DATA, req->is_read); /* data-length not consistent with block-structure */

			return LSA_TRUE; /* fin */
		}

		if( ! cm_sv_ARFSUDataAdjust_store(req->argr, ndr_data, req->record_data_length, LSA_FALSE) ) {

			/* cnf(-) if record could not be stored */

			*pnio_err = CM_PNIO_FAULTY_Record__CMRPC_OutOfMemory();

			return LSA_TRUE; /* fin */
		}

		if( ! channel->usr.sv.can_ARFSUDataAdjust ) {

			/* AP00511230: if user cannot handle it, just tell cnf(+) */

			*pnio_err = CM_PNIO_ERR_NONE;

			return LSA_TRUE; /* fin */
		}

		/*
		 * pass it on to the user
		 * see cm_sv_opc_ar_record_response() for what is done on rsp(-)
		 */
	}

	if( ! req->is_read && (req->cls & CM_REC_IDX_IM1234) != 0 ) { /* AP01338246 */

		LSA_UINT32 err = cm_sv_IM1234_check(ndr_data, req->record_data_length, req->index);

		if( err != CM_PNIO_ERR_NONE ) {

			*pnio_err = err;

			return LSA_TRUE; /* fin */
		}
	}

	if( req->is_read && CM_SV_SUBSLOT_IS_PDEV(req->subslot_nr) ) {

		if(
			req->index == 0x8028/*RecordInputDataObjectElement*/
			|| req->index == 0x8029/*RecordOutputDataObjectElement*/
		) {

			*pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0); /* TIA 1580816 */

			return LSA_TRUE; /* fin */
		}
	}

	/***/

	if( req->index == 0xE060/*RS_GetEvent*/ ) { /* TIA 1702335 */

		if(
			! req->is_read /* always false */
			|| (cm_is_not_null(argr) && cm_is_null(argr->rs_info)) /* explicit-AR: no RSInfoBlock or legacy-AR or easy-supervisor */
			|| (cm_is_null(argr) && cm_is_not_null(req->argr) && cm_is_null(req->argr->rs_info)) /* implicit-AR: target-AR without RSInfoBlock */
		) {

			*pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10);

			return LSA_TRUE; /* fin */
		}

		if( req->record_data_length < CM_RECORD_OFFSET + 4096 ) {

			*pnio_err = CM_PNIO_FAULTY_Record__InvalidRange(11);

			return LSA_TRUE; /* fin */
		}

		/* see too syntax-check in cm_sv_opc_ar_record_response() */
	}

	if( req->index == 0xE061/*RS_AckEvent*/ ) { /* TIA 1702335 */

		if(
			req->is_read /* always false */
			|| cm_is_null(argr) /* always false */
			|| cm_is_null(argr->rs_info) /* no RSInfoBlock or legacy-AR or easy-supervisor */
		) {

			*pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10);

			return LSA_TRUE; /* fin */
		}

		if(
			req->record_data_length != CM_RECORD_OFFSET + 2+2+1+1+2
			|| CM_GET16_NTOH(ndr_data, CM_RECORD_OFFSET + 0) != 0x0902
			|| CM_GET16_NTOH(ndr_data, CM_RECORD_OFFSET + 2) != 4
			|| CM_GET16_NTOH(ndr_data, CM_RECORD_OFFSET + 4) != 0x0100
			|| (CM_GET16_NTOH(ndr_data, CM_RECORD_OFFSET + 6) & 0xF800) != 0
		) {

			*pnio_err = CM_PNIO_FAULTY_Record__InvalidParameter(11);

			return LSA_TRUE; /* fin */
		}
	}

	/***/

	{
	LSA_OPCODE_TYPE opcode = (LSA_OPCODE_TYPE)(req->is_read ? CM_OPC_SV_AR_READ_IND : CM_OPC_SV_AR_WRITE_IND);
	CM_UPPER_RQB_PTR_TYPE event = cm_sv_dev_get_event_notnull(dev, argr, opcode);
	CM_UPPER_EVENT_PTR_TYPE param = event->args.sv.ar_event;

	if( cm_is_null(argr) ) {

		param->u.sv.ar_rec.is_iosar_with_device_access = CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_NO;

		if( cm_is_not_null(req->argr) ) {
			param->u.sv.ar_rec.target_session_key = req->argr->session_key;
		}
		else {
			param->u.sv.ar_rec.target_session_key = 0;
		}
	}
	else {

		param->u.sv.ar_rec.is_iosar_with_device_access = cm_is_iosar_with_device_access(argr) ? CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_YES : CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_NO;

		param->u.sv.ar_rec.target_session_key = 0;
	}

	CM_ASSERT(req->record_data_length >= CM_RECORD_OFFSET);

	CM_ASSERT(((LSA_UINT32)ndr_data & 3) == 0); /* record headers must be long-aligned */

	param->u.sv.ar_rec.api          = req->api;
	param->u.sv.ar_rec.slot_nr      = req->slot_nr;
	param->u.sv.ar_rec.subslot_nr   = req->subslot_nr;
	param->u.sv.ar_rec.record_index = req->index;
	param->u.sv.ar_rec.via_CombinedObjectContainer = via_CombinedObjectContainer ? CM_VIA_COC_YES : CM_VIA_COC_NO;
	param->u.sv.ar_rec.sequence_nr  = req->seq_number;
	param->u.sv.ar_rec.data_length  = req->record_data_length; /* including CM_RECORD_OFFSET */
	param->u.sv.ar_rec.data         = ndr_data; /* pass-in the clrpc-buffer! */

	param->u.sv.ar_rec.cm_pnio_err  = CM_PNIO_ERR_MAKE(255, 255, 255, 255); /* sanity, out-param */
	param->u.sv.ar_rec.ret_val_1    = 0; /* sanity, out-param */
	param->u.sv.ar_rec.ret_val_2    = 0; /* sanity, out-param */

	param->u.sv.ar_rec.readquery_length = req->readquery_length;

	cm_callback(channel, CM_OK, &event);
	}

	/***/

	*pnio_err = CM_PNIO_ERR_NONE;

	return LSA_FALSE;
}

/*===========================================================================*/
/*=== RECORD-READ (implicit AR) =============================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_read_implicit(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req
) {
	LSA_UINT32 pnio_err;

	/***/

	if( (req->cls & CM_REC_IDX_USER) != 0 ) {

		LSA_BOOL fin;

		fin = cm_sv_ar_record_indicate(
			dev, LSA_NULL, LSA_FALSE,
			req, rpc->args.sv.call->ndr_data, /* pass-in the clrpc-buffer! */
			&pnio_err
		);

		if( pnio_err == CM_PNIO_ERR_NONE ) {

			if( fin ) {
				CM_FATAL(); /* a bug, no such use-case */
			}
		}
	}
	else if( (req->cls & CM_REC_IDX_PDEV) != 0 ) {

		if( dev->implicit_ar.iter.pdev.state != CM_PD_ITER_PDEV_IDLE ) { /* sic! using the CM-PD enumeration */
			CM_FATAL(); /* a bug */
			return 0;
		}

		if( (req->cls & CM_REC_IDX_8000) != 0 ) {

			pnio_err = cm_sv_pdev_read_write(
				dev
				, req
				, rpc->args.sv.call->ndr_data /* pass-in the clrpc-buffer! */
				, 0/*implicit-ar*/
				);
		}
		else {

			pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
		}
	}
	else {

		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== RECORD-READ-WRITE (explicit AR) =======================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_rdwr_explicit(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req
) {
	LSA_UINT32 pnio_err;

	/***/

	if( (req->cls & CM_REC_IDX_USER) != 0 ) {

		LSA_BOOL fin;

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_RECORD_RSP) != 0 ) {
			CM_FATAL();
		}

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_RECORD_RSP;

		fin = cm_sv_ar_record_indicate(
			argr->cm.sv.dev, argr, LSA_FALSE,
			req, rpc->args.sv.call->ndr_data, /* pass-in the clrpc-buffer! */
			&pnio_err
		);

		if( fin ) {

			argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_RECORD_RSP;

			/***/

			if( ! req->is_read && pnio_err != CM_PNIO_ERR_NONE ) { /* write.rsp(-) */

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) {

					if( (req->cls & CM_REC_IDX_E000) != 0 ) {

						/* AP00572036: CM has to set "a.r.p." on all submodules */

						CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "prm-write index(0x%x) failed, setting all submodules to appl-ready-pending"
							, req->index
							);

						argr->cm.sv.ar_flags |= CM_SV_AR_FL_ARP_E000; /* see cm_sv_marshal_module_diff_block() */
					}
				}
			}

			/***/

			if( pnio_err == CM_PNIO_ERR_NONE ) {

				cm_sv_rpc_mark_as_record_succeeded(rpc, req->is_read, CM_RECORD_OFFSET);
			}
		}
		else {

			pnio_err = CM_PNIO_ERR_NONE; /* running */
		}
	}
	else if( (req->cls & CM_REC_IDX_PDEV) != 0 ) {

		if( argr->cm.sv.rec.iter_ptr->pdev.state != CM_PD_ITER_PDEV_IDLE ) { /* sic! using the CM-PD enumeration */
			CM_FATAL(); /* a bug */
			return 0;
		}

		if( (req->cls & CM_REC_IDX_8000) != 0 ) {

			pnio_err = cm_sv_pdev_read_write(
				argr->cm.sv.dev
				, req
				, rpc->args.sv.call->ndr_data /* pass-in the clrpc-buffer! */
				, argr->ar_nr
				);
		}
		else {

			pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
		}
	}
	else {

		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== RPC PARK / UNPARK =====================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_rpc_park( /* TIA 1518690 */
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_write /* true if write, false if prm-end or prm-begin (TIA 2231336) */
) {
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE plg_rel_sub = cm_sv_ar_almi_plug_or_release(argr);

	if( cm_is_not_null(plg_rel_sub) ) {

		if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_APPL_READY_REQ) != 0 ) {

			if( is_write && (argr->cm.sv.ar_flags & CM_SV_AR_FL_PBE_PHASE) == 0 ) { /* TIA 2231336 */

				/* problem: rpc-write.req overlaps with plug/release sequence after rpc-appl-ready.req */
				/* if the controller defers the alarm-acknowledge until after rpc-write.cnf a deadlock occurred before this change */
				/* solution: don't park an rpc-write if not PBE phase (i.e. not the continuation of the plug sequence in case of plug/prm-begin overlap) */

				return LSA_FALSE;
			}

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) parking rpc(0x%08x) is_write(%u) ar_flags(0x%08x), prm-begin/prm-end/write overlaps with plug or release alarm after rpc-appl-ready.req"
				, argr->device_nr, argr->ar_nr
				, rpc, is_write, argr->cm.sv.ar_flags
				);

			argr->cm.sv.curr_rpc_ind = rpc; /* pending */
			argr->cm.sv.curr_rpc_ind_is_parked = LSA_TRUE;
			return LSA_TRUE;
		}
	}

	return LSA_FALSE;
}

LSA_VOID
cm_sv_rpc_unpark( /* TIA 1518690 */
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( argr->cm.sv.curr_rpc_ind_is_parked ) {

		if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {

			CM_FATAL();
		}
		else {

			CM_RPC_LOWER_RQB_PTR_TYPE rpc = argr->cm.sv.curr_rpc_ind;

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) unparking rpc"
				, argr->device_nr
				, argr->ar_nr
				);

			argr->cm.sv.curr_rpc_ind = LSA_NULL;
			argr->cm.sv.curr_rpc_ind_is_parked = LSA_FALSE;


			switch( rpc->args.sv.call->opnum ) {

			case CLRPC_PNIO_OPNUM_WRITE:
				cm_sv_rpc_record_indication(argr->cm.sv.dev, rpc, LSA_FALSE, LSA_FALSE);
				break;

			case CLRPC_PNIO_OPNUM_CONTROL:
				cm_sv_rpc_control_indication(argr->cm.sv.dev, rpc);
				break;

			default:
				CM_FATAL(); /* connect, release, read, etc. are not parked! */
			}

		}
	}
}

/*===========================================================================*/
/*=== RECORD-READ/WRITE DISPATCH ============================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_rpc_record_dispatch(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_PTR_TYPE req,
	CM_AR_GRAPH_PTR_TYPE *explicit_argr_ptr
) {
	LSA_UINT32 pnio_err;
	LSA_UINT16 record_cnt;

	/***/

	CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev(%u) is_read(%u) is_read_implicit(%u)"
		, dev->device_nr
		, req->is_read
		, req->is_read_implicit
		);

	/***/

	pnio_err = cm_sv_record_parse_header(rpc, req, dev->channel->usr.sv.can_RecordDataReadQuery);

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev(%u) cm_sv_record_parse_header, pnio_err(0x%x)"
			, dev->device_nr
			, pnio_err
			);

		/* cannot return a IODXxxResHeader because of malformed/inconsistent request */

		if( ! req->is_read ) { /* write */

			pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(pnio_err);
		}

		cm_sv_rpc_mark_as_record_malformed(rpc, pnio_err);

		return CM_PNIO_ERR_NONE; /* handled */
	}

	/***/

	record_cnt = 1;

	if( ! req->is_read && req->index == 0xE040 ) { /* write-multiple */

		pnio_err = cm_sv_record_check_write_multiple_body(rpc, req, &record_cnt);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev(%u) cm_sv_record_check_write_multiple_body, pnio_err(0x%x)"
				, dev->device_nr
				, pnio_err
				);

			/* cannot return a IODXxxResHeader because of malformed/inconsistent request */

			pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(pnio_err);

			cm_sv_rpc_mark_as_record_malformed(rpc, pnio_err);

			return CM_PNIO_ERR_NONE; /* handled */
		}
	}

	/***/

	req->cls = cm_record_index_classify(dev->channel, req->api, req->index, dev->vendor_id);

	/***/

	if( req->is_read_implicit ) { /* implicit-ar uses TargetARUUID, ARUUID must be NIL */

		CM_ASSERT(cm_is_null(explicit_argr_ptr)); /* not used */

		/***/

		if( ! cm_uuid_is_nil(&req->ar_uuid) ) {

			return CM_PNIO_FAULTY_Record__Block(5); /* ARUUID not NIL */
		}
		else {

			CM_GET_UUID_NTOH(&req->ar_uuid, req->target_ar_uuid_ptr, 0);

			if( req->index < 0xF000 ) { /* TargetARUUID shall be evaluated if needed by index */

				if( cm_uuid_is_nil(&req->ar_uuid) ) {

					req->argr = LSA_NULL;
				}
				else {

					req->argr = cm_sv_ar_lookup_by_aruuid(dev, &req->ar_uuid); /* NIL is not an error here */
				}
			}
			else { /* TargetARUUID shall be ignored */

				req->argr = LSA_NULL;
			}
		}
	}
	else { /* explicit-ar (read or write) */

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup_by_aruuid(dev, &req->ar_uuid);

		CM_ASSERT(cm_is_not_null(explicit_argr_ptr));

		if( cm_is_null(argr) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev(%u) ARUUID not found"
				, dev->device_nr
				);

			CM_ASSERT(cm_is_null(*explicit_argr_ptr));

			return CM_PNIO_FAULTY_Record__CMRPC_ARUUIDUnknown();
		}
		else if( rpc->args.sv.call->host_ip != argr->ar.host_ip ) {

			CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) host(%08X) not conn-host(%08X)"
				, argr->device_nr, argr->ar_nr
				, CM_NTOH32(rpc->args.sv.call->host_ip)
				, CM_NTOH32(argr->ar.host_ip)
				);

			return CM_PNIO_FAULTY_Record__CMRPC_StateConflict(); /* unexpected ip-addr */
		}
		else if( cm_is_not_null(argr->cm.sv.curr_rpc_ind) ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) already servicing a call (two clients on same host using the same ARUUID?)"
				, argr->device_nr, argr->ar_nr
				);

			return CM_PNIO_FAULTY_Record__CMRPC_StateConflict(); /* unexpected call */
		}
		else {

			req->argr = argr; /* found */
		}
	}

	/***/

	CM_SV_TRACE_09(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "is_read(%u) is_implicit(%u) seq(%u) ap/sl/su(%u/%u/%u) index(0x%x) length(%u+%u)"
		, req->is_read
		, req->is_read_implicit
		, req->seq_number
		, req->api
		, req->slot_nr
		, req->subslot_nr
		, req->index
		, CM_RECORD_OFFSET
		, req->record_data_length - CM_RECORD_OFFSET
		);

	CM_SV_TRACE_11(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, req->ar_uuid.time_low, req->ar_uuid.time_mid, req->ar_uuid.time_hi_and_version
		, req->ar_uuid.clock_seq_hi_and_reserved, req->ar_uuid.clock_seq_low
		, req->ar_uuid.node[0], req->ar_uuid.node[1], req->ar_uuid.node[2]
		, req->ar_uuid.node[3], req->ar_uuid.node[4], req->ar_uuid.node[5]
		);

	/***/

	if( ! req->is_read_implicit ) { /* explicit-ar */

		CM_AR_GRAPH_PTR_TYPE argr = req->argr; /* exists */

		/***/

		if( cm_ar_com_is_down(argr) ) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) already down (telling unknown aruuid)"
				, argr->device_nr, argr->ar_nr
				);

			return CM_PNIO_FAULTY_Record__CMRPC_ARUUIDUnknown(); /* already down */
		}

		/***/

		argr->cm.sv.curr_rpc_ind = rpc; /* pending */

		CM_ASSERT(cm_is_not_null(explicit_argr_ptr));
		CM_ASSERT(cm_is_null(*explicit_argr_ptr));

		*explicit_argr_ptr = argr;

		if( !req->is_read ) { /* TIA 1518690 */

			LSA_BOOL is_parked = cm_sv_rpc_park(argr, rpc, LSA_TRUE);

			if( is_parked ) {

				return CM_PNIO_ERR_NONE;
			}
		}

		/*
		 * check sequence number
		 */

		if( req->seq_number == argr->cm.sv.rec.seq_number_exp ) {

			/* advance to next expected sequence number */

			argr->cm.sv.rec.seq_number_pre = argr->cm.sv.rec.seq_number_exp;

			argr->cm.sv.rec.seq_number_exp += record_cnt; /* wrap around at 64K */

			argr->cm.sv.rec.is_rerun = LSA_FALSE;
		}
		else if( req->seq_number == argr->cm.sv.rec.seq_number_pre ) {

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) seq(%u) == pre(%u), assuming rerun"
				, argr->device_nr, argr->ar_nr
				, req->seq_number
				, argr->cm.sv.rec.seq_number_pre
				);

			argr->cm.sv.rec.is_rerun = LSA_TRUE;
		}
		else {

			CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) seq(%u) not exp(%u)/pre(%u), protocol error"
				, argr->device_nr, argr->ar_nr
				, req->seq_number
				, argr->cm.sv.rec.seq_number_exp
				, argr->cm.sv.rec.seq_number_pre
				);

			/*
			 * if we do not abort the ar here, it will be difficult to find out what is going wrong!
			 */

			cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PROT, 0);
			cm_ar_com_down(argr, CM_AR_REASON_PROT); /* protocol error */

			return CM_PNIO_FAULTY_Record__CMRPC_StateConflict();
		}

		/***/

		if( record_cnt > 1 ) { /* write-multiple */

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "write-multiple, record_cnt(%u)"
				, record_cnt
				);

			argr->cm.sv.rec.multi_offs = CM_RECORD_OFFSET; /* set up for first */
		}
		else { /* read or write "single" */

			argr->cm.sv.rec.multi_offs = 0;
		}

		argr->cm.sv.rec.coc_offs = 0;

		/***/

		if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_RETRIGGER_TIMER) != 0 ) {

			cm_sv_ar_cmi_timer_stop(argr); /* trigger: rpc-read-write.ind */

			/***/

			if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_DO_CONS_12UDP) != 0 ) {

				argr->cm.sv.ar_flags &= ~CM_SV_AR_FL_DO_CONS_12UDP;

				/* compare to cm_sv_rpc_control_indication() */

				if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_CONS_ACT) != 0 ) {

					CM_FATAL();
				}
				else {

					if( CM_SV_AR_IS_ADVANCED(argr) ) {

						argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONS_ACT;
						cm_arcq_request(argr, CM_ARCQ_OPC_CONS_ACTIVATE, 0);
					}
					else {

						argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_RETRIGGER_TIMER; /* consumer DHT takes over */

						argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_CONS_ACT;
						cm_arcq_request(argr, CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK, 0);
					}
				}
			}
		}
	}

	/***/

	if( req->is_read ) { /* read or read-implicit */

		CM_SV_SUBSLOT_PTR_TYPE su = LSA_NULL;

		pnio_err = cm_sv_record_check_range(dev, req, &su);

		if( pnio_err == CM_PNIO_ERR_NONE ) { /* TIA 1094128 */

			if( cm_is_not_null(su) ) {

				if( req->index == 0x80A0 /* PROFIenergy Record_0 */ ) {

					if( (su->im0_bits & CM_SV_IM0_BITS_PROFIENERGY) == 0 ) {

						CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_WARN
							, "dev(%u) PROFIenergy not supported"
							, dev->device_nr
							);
						return CM_PNIO_FAULTY_Record__InvalidIndex(10); /* submodule does not support PROFIenergy */
					}
				}

				/***/

				if( req->index == 0xAFF5/*I&M5*/ ) { /* TIA 1915400 */

					if(
						(req->cls & CM_REC_IDX_NORM) != 0 /* always true, see cm_record_index_classify() */
						&& (su->im0_bits & CM_SV_IM0_BITS_IM5) != 0 /* if false: cm_sv_record_marshal_IM5() handles the case */
						&& (su->im0_bits & CM_SV_IM0_BITS_R1_REMOTE) != 0
					) {

						/* reclassify, so that it is passed on to the user */
						req->cls &= ~CM_REC_IDX_NORM;
						req->cls |= CM_REC_IDX_USER;
					}
				}
			}
		}

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev(%u) cm_sv_record_check_range, pnio_err(0x%x)"
				, dev->device_nr
				, pnio_err
				);
		}
		else if( (req->cls & CM_REC_IDX_E000) != 0 && cm_is_null(req->argr) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_WARN
				, "index(0x%x) needs target_uuid"
				, req->index
				);
			pnio_err = CM_PNIO_FAULTY_Record__InvalidParameter(12); /* index needs target_uuid */
		}
		else if( (req->cls & CM_REC_IDX_READ) == 0 ) {

			CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) index(0x%x) is reserved or not readable"
				, dev->device_nr, req->index
				);

			if( (req->cls & CM_REC_IDX_WRITE) != 0 ) { /* AP01566285: access denied if writeonly record */

				pnio_err = CM_PNIO_FAULTY_Record__AccessDenied(10);
			}
			else {

				pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10);
			}
		}
		else if( (req->cls & CM_REC_IDX_NORM) != 0 ) {

			pnio_err = cm_sv_record_read_normative(dev, rpc, req, su);
		}
		else if( req->index == 0xE050/*ARFSUDataAdjust*/ ) {

			if(
				cm_is_null(req->argr) /* always false */
				|| CM_SV_AR_IS_ADVANCED(req->argr)
			) {

				pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* note: advanced ARs use the IODConnectReq::ARFSUBlock (AP01034170) */
			}
			else {

				pnio_err = cm_sv_ARFSUDataAdjust_read(req->argr, rpc, req);
			}
		}
		else if( req->is_read_implicit ) {

			pnio_err = cm_sv_record_read_implicit(dev, rpc, req);
		}
		else if( cm_is_null(req->argr) ) { /* always false */

			pnio_err = CM_PNIO_FAULTY_Record__StateConflict(5); /* unreachable */
		}
		else {

			pnio_err = cm_sv_record_rdwr_explicit(req->argr, rpc, req);
		}
	}
	else if( cm_is_null(req->argr) ) { /* always false */

		pnio_err = CM_PNIO_FAULTY_Record__StateConflict(5); /* unreachable */
	}
	else if( req->index == 0xE040 ) { /* write-multiple */

		CM_AR_GRAPH_PTR_TYPE argr = req->argr; /* see check above */

		if( argr->cm.sv.rec.multi_offs != CM_RECORD_OFFSET ) {

			CM_FATAL(); /* a bug */
		}
		else {

			argr->cm.sv.rec.multi_pending_cnt = 0;

			/***/

			if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_RECORD_RSP) != 0 ) {

				CM_FATAL();
			}
			else {

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_RECORD_RSP; /* set on start, reset on completion */

				/***/

				cm_sv_record_write_embedded_next(argr);
			}
		}

		pnio_err = CM_PNIO_ERR_NONE; /* running or completed */
	}
	else { /* write "single" */

		CM_AR_GRAPH_PTR_TYPE argr = req->argr; /* see check above */

		if( req->index == 0x80B0/*CombinedObjectContainer*/ ) { /* TIA 1092258 */

			pnio_err = cm_sv_pdev_CombinedObjectContainer_write(argr, rpc->args.sv.call->ndr_data, req);

			return pnio_err;
		}

		pnio_err = cm_sv_record_check_range_and_context(argr, req, LSA_FALSE);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_WARN
				, "dev/ar(%u/%u) cm_sv_record_check_range_and_context, pnio_err(0x%x)"
				, argr->device_nr, argr->ar_nr
				, pnio_err
				);
		}
		else {

			pnio_err = cm_sv_record_rdwr_explicit(argr, rpc, req);
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== RECORD INDICATION =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_record_indication(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	LSA_BOOL is_read,
	LSA_BOOL is_read_implicit
) {
	CM_SV_RECORD_PARAMS_TYPE req;

	CM_ASSERT(CM_RPC_GET_RESPONSE(rpc) == LSA_RSP_UNUSED/*pending*/); /* see caller */

	req.is_read = is_read;
	req.is_read_implicit = is_read_implicit;

	/***/

	if( req.is_read_implicit ) {

		CM_ASSERT(req.is_read == LSA_TRUE);

		if( cm_is_null(dev->implicit_ar.rpc) ) {
			CM_FATAL(); /* a bug, see caller */
		}

		/***/

		{
		LSA_UINT32 pnio_err;

		pnio_err = cm_sv_rpc_record_dispatch(dev, rpc, &req, LSA_NULL);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_READ);

			cm_sv_rpc_mark_as_record_failed(dev->implicit_ar.rpc, req.is_read, pnio_err, 0, 0);
		}
		}
	}
	else { /* read or write explicit */

		CM_AR_GRAPH_PTR_TYPE explicit_argr = LSA_NULL;
		LSA_UINT32 pnio_err;

		pnio_err = cm_sv_rpc_record_dispatch(dev, rpc, &req, &explicit_argr);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_READ);

			if( ! req.is_read ) { /* write */

				pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(pnio_err);
			}

			cm_sv_rpc_mark_as_record_failed(rpc, req.is_read, pnio_err, 0, 0);
		}

		/***/

		if( cm_is_null(explicit_argr) ) { /* no ar-graph */

			if( pnio_err == CM_PNIO_ERR_NONE ) {

				/* malformed header, see calls of cm_sv_rpc_mark_as_record_malformed() */

				CM_ASSERT(rpc->args.sv.call->ndr_len == CLRPC_PNIO_OFFSET); /* a weak check */
			}
			else {

				/* unknown ARUUID, must return a IODXxxResHeader in this case too */

				CM_ASSERT(rpc->args.sv.call->ndr_len == CM_RECORD_OFFSET); /* a weak check */
			}

			cm_rpc_request_lower(dev->channel, rpc, CLRPC_OPC_SV_CALL_RESPONSE, dev->device_nr, 0);
		}
		else {

			CM_ASSERT(cm_is_not_null(explicit_argr->cm.sv.curr_rpc_ind));

			cm_sv_rpc_check_explicit_ar(explicit_argr);
		}
	}
}

/*===========================================================================*/
/*=== UPDATE EMBEDDED HEADER ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_record_write_embedded_update(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_MEM_U8_PTR_TYPE header_ptr,
	LSA_UINT32 pnio_err,
	LSA_UINT16 ret_val_1,
	LSA_UINT16 ret_val_2
) {
	CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "multi_pending_cnt(%u) on entry"
		, argr->cm.sv.rec.multi_pending_cnt
		);

	/***/

	if( CM_GET16_NTOH(header_ptr, 0) != CM_BLOCK_TYPE_WRITE_REQ ) {
		CM_FATAL(); /* a bug */
	}

	cm_sv_record_update_header(
		header_ptr,
		LSA_FALSE/*write*/,
		0/*!*/,
		pnio_err,
		ret_val_1,
		ret_val_2
		);

	/***/

	if( argr->cm.sv.rec.multi_pending_cnt < 1 ) {
		CM_FATAL();
	}

	argr->cm.sv.rec.multi_pending_cnt -= 1;
}

/*===========================================================================*/
/*=== THIS EMBEDDED WRITE ===================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_record_write_embedded_this(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_MEM_U8_PTR_TYPE header_ptr,
	LSA_UINT32 record_data_length
) {
	CM_SV_RECORD_PARAMS_TYPE req;
	LSA_UINT32 pnio_err;

	/***/

	req.is_read = LSA_FALSE;
	req.is_read_implicit = LSA_FALSE;

	/***/

	if( CM_GET16_NTOH(header_ptr, CM_BLK_HEADER_type) != CM_BLOCK_TYPE_WRITE_REQ ) {
		CM_FATAL(); /* a bug */
	}

	{
	CM_UPPER_MEM_U8_PTR_TYPE data_ptr = header_ptr;

	req.seq_number = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_seq_number);
	req.api        = CM_GET32_NTOH(data_ptr, CM_BLK_RDWR_api);
	req.slot_nr    = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_slot_nr);
	req.subslot_nr = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_subslot_nr);
	req.index      = CM_GET16_NTOH(data_ptr, CM_BLK_RDWR_index);

	CM_ASSERT(record_data_length == CM_GET32_NTOH(data_ptr, CM_BLK_RDWR_record_data_length));
	}

	req.record_data_length = CM_RECORD_OFFSET + record_data_length; /* include the clrpc- and record-header */

	/***/

	req.cls = cm_record_index_classify(argr->ar_com.channel, req.api, req.index, argr->cm.sv.dev->vendor_id);

	req.argr = argr;

	/***/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REFACTOR: unify write and write-multiple to avoid code-duplication")
#endif /* CM_MESSAGE */

	if( req.index == 0x80B0/*CombinedObjectContainer*/ ) { /* TIA 1092258 */

		pnio_err = cm_sv_pdev_CombinedObjectContainer_write(argr, (header_ptr - CLRPC_PNIO_OFFSET), &req);

		return pnio_err;
	}

	pnio_err = cm_sv_record_check_range_and_context(argr, &req, LSA_FALSE);

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev/ar(%u/%u) cm_sv_record_check_range_and_context, pnio_err(0x%x)"
			, argr->device_nr, argr->ar_nr
			, pnio_err
			);
	}
	else if( (req.cls & CM_REC_IDX_USER) != 0 ) {

		LSA_BOOL fin;

		fin = cm_sv_ar_record_indicate(
			argr->cm.sv.dev, argr, LSA_FALSE,
			&req, (header_ptr - CLRPC_PNIO_OFFSET), /* pass-in the clrpc-buffer! */
			&pnio_err
			);

		if( fin ) {

			if( ! req.is_read && pnio_err != CM_PNIO_ERR_NONE ) { /* write.rsp(-) */

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) { /* connect-phase */

					if( (req.cls & CM_REC_IDX_E000) != 0 ) {

						/* AP00572036: CM has to set "a.r.p." on all submodules */

						CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_WARN
							, "prm-write index(0x%x) failed, setting all submodules to appl-ready-pending"
							, req.index
							);

						argr->cm.sv.ar_flags |= CM_SV_AR_FL_ARP_E000; /* see cm_sv_marshal_module_diff_block() */
					}
				}
			}

			if( pnio_err == CM_PNIO_ERR_NONE ) {

				cm_sv_record_write_embedded_update(argr, header_ptr, pnio_err, 0, 0);
			}
		}
	}
	else if( (req.cls & CM_REC_IDX_PDEV) != 0 ) {

		pnio_err = cm_sv_pdev_read_write(
			argr->cm.sv.dev
			, &req
			, header_ptr - CLRPC_PNIO_OFFSET /* pass-in the clrpc-buffer! */
			, argr->ar_nr
			);
	}
	else {

		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(10); /* unreachable */
	}

	/***/

	return pnio_err; /* note: CM_PNIO_ERR_NONE if pending */
}

/*===========================================================================*/
/*=== NEXT EMBEDDED WRITE ===================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_record_write_embedded_next(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT32 multi_pending_max;

	/***/

	CM_ASSERT(cm_is_not_null(argr->cm.sv.curr_rpc_ind));

	CM_ASSERT(((LSA_UINT32)argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_data & 3) == 0); /* see caller */

	CM_ASSERT(argr->cm.sv.rec.multi_offs != 0);

	CM_ASSERT((argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_RECORD_RSP) != 0); /* see caller */

	/***/

	if( cm_is_iosar_with_device_access(argr) ) {

		multi_pending_max = 1; /* no parallelism for easy-supervisor */
	}
	else {

		multi_pending_max = argr->cm.sv.dev->write_multiple_parallelism; /* AP00786836 */
	}

	/***/

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "multi_offs(%u) ndr_len(%u) multi_pending_cnt/max(%u/%u) on entry"
		, argr->cm.sv.rec.multi_offs
		, argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_len
		, argr->cm.sv.rec.multi_pending_cnt
		, multi_pending_max
		);

	/***/

	while( argr->cm.sv.rec.multi_offs < argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_len ) {

		CM_UPPER_MEM_U8_PTR_TYPE header_ptr;
		LSA_UINT32 record_data_length;

		/***/

		argr->cm.sv.rec.multi_offs = ((argr->cm.sv.rec.multi_offs + 3) & ~3); /* align next header on 32 bits */

		header_ptr = argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_data + argr->cm.sv.rec.multi_offs;

		if( argr->cm.sv.rec.multi_pending_cnt != 0 ) {

			LSA_UINT16 index = CM_GET16_NTOH(header_ptr, CM_BLK_RDWR_index);

			CM_ASSERT(argr->cm.sv.rec.coc_offs == 0);

			if( index == 0x80B0/*CombinedObjectContainer*/ ) {

				/* must start with nothing pending */
				/* otherwise pd.cnf cannot be related to the request, see cm_sv_pdev_write_done() */

				break; /* wait */
			}
		}

		record_data_length = CM_GET32_NTOH(header_ptr, CM_BLK_RDWR_record_data_length);

		argr->cm.sv.rec.multi_offs += CM_RECORD_HEADER_SIZE + record_data_length;

		/***/

		argr->cm.sv.rec.multi_pending_cnt += 1; /* decrement see cm_sv_record_write_embedded_update() */

		{
		LSA_UINT32 pnio_err;

		if( cm_ar_com_is_down(argr) ) { /* must not indicate after abort/offline */

			pnio_err = CM_PNIO_FAULTY_Record__StateConflict(5); /* see below */
		}
		else {

			pnio_err = cm_sv_record_write_embedded_this(argr, header_ptr, record_data_length);
		}

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_READ);

			pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(pnio_err);

			cm_sv_record_write_embedded_update(argr, header_ptr, pnio_err, 0, 0);
		}
		}

		/***/

		if( argr->cm.sv.rec.multi_pending_cnt >= multi_pending_max ) {

			break;
		}

		if( argr->cm.sv.rec.coc_offs != 0 ) {

			break; /* coc-iterator running */
		}
	}

	/***/

	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "multi_offs(%u) ndr_len(%u) multi_pending_cnt/max(%u/%u) after loop"
		, argr->cm.sv.rec.multi_offs
		, argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_len
		, argr->cm.sv.rec.multi_pending_cnt
		, multi_pending_max
		);

	if( argr->cm.sv.rec.multi_pending_cnt == 0 ) { /* complete */

		argr->cm.sv.rec.multi_offs = 0; /* sanity */

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_RECORD_RSP;

		/***/

		if( cm_ar_com_is_down(argr) ) {

			LSA_UINT32 pnio_err = CM_PNIO_FAULTY_Record__CMRPC_ARUUIDUnknown(); /* already down */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) already down (telling unknown aruuid)"
				, argr->device_nr, argr->ar_nr
				);

			/***/

			CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_READ);

			pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(pnio_err);

			/***/

			cm_sv_rpc_mark_as_record_failed(argr->cm.sv.curr_rpc_ind, LSA_FALSE/*write*/, pnio_err, 0, 0);

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("BEAUTIFY: refactor, return pnio_err here, mark in caller")
#endif /* CM_MESSAGE */

		}
		else { /* move headers to front */

			CM_UPPER_MEM_U8_PTR_TYPE dst = argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_data + CM_RECORD_OFFSET;
			LSA_UINT32 multi_offs = CM_RECORD_OFFSET;

			while( multi_offs < argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_len ) {

				CM_UPPER_MEM_U8_PTR_TYPE header_ptr;
				LSA_UINT32 record_data_length;

				/***/

				multi_offs = ((multi_offs + 3) & ~3); /* align next header on 32 bits */

				header_ptr = argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_data + multi_offs;

				record_data_length = CM_GET32_NTOH(header_ptr, CM_BLK_RDWR_record_data_length);

				multi_offs += CM_RECORD_HEADER_SIZE + record_data_length;

				/***/

				if( CM_GET16_NTOH(header_ptr, CM_BLK_HEADER_type) != CM_BLOCK_TYPE_WRITE_RSP ) {
					CM_FATAL();
				}

				CM_PUT32_HTON(header_ptr, CM_BLK_RDWR_record_data_length, 0/*!*/);

				/***/

				if( dst != header_ptr ) {

					CM_MEMMOVE(dst, header_ptr, CM_RECORD_HEADER_SIZE);
				}

				dst += CM_RECORD_HEADER_SIZE;
			}

			/***/

			{
			LSA_UINT32 ndr_length = CM_CAST_U32(dst - argr->cm.sv.curr_rpc_ind->args.sv.call->ndr_data);

			CM_ASSERT(
				ndr_length >= CM_RECORD_OFFSET + CM_RECORD_HEADER_SIZE
				&& ((ndr_length - CM_RECORD_OFFSET) % CM_RECORD_HEADER_SIZE) == 0
			);

			cm_sv_rpc_mark_as_record_succeeded(argr->cm.sv.curr_rpc_ind, LSA_FALSE/*write*/, ndr_length);
			}
		}
	}
}

/*===========================================================================*/
/*=== RESPONSE CHECK OF RS_GetEvent =========================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_record_response_RS_GetEvent_ok( /* TIA 1702335 */
	LSA_UINT32 pnio_err,
	LSA_UINT8 * ndr_data,
	LSA_UINT32 ndr_length
) {
	LSA_UINT8 * data;
	LSA_UINT32 size;
	LSA_UINT32 offs;
	LSA_UINT16 entries;
	LSA_UINT16 cnt;

	if( pnio_err != CM_PNIO_ERR_NONE ) {
		return LSA_FALSE;
	}

	if( ndr_length < CM_RECORD_OFFSET ) {
		return LSA_FALSE; /* unreachable */
	}

	data = ndr_data   + CM_RECORD_OFFSET;
	size = ndr_length - CM_RECORD_OFFSET;

	if( size == 0 ) {
		return LSA_TRUE; /* "empty" is ok */
	}

	/* block structure check
	 *
	 * RS_GetEvent  ::= BlockHeader, RS_EventInfo
	 * RS_EventInfo ::= NumberOfEntries, RS_EventBlock*
	 */

	if(
		size < 8 /* too short for BlockHeader and NumberOfEntries */
		|| CM_GET16_NTOH(data, 0) != 0x0901
		|| 4U + CM_GET16_NTOH(data, 2) != size
		|| CM_GET16_NTOH(data, 4) != 0x0100
	) {
		return LSA_FALSE;
	}

	entries = CM_GET16_NTOH(data, 6);

	offs = 8;

	if( entries == 0 ) {
		return LSA_FALSE; /* RS_EventBlock is not optional */
	}

	for( cnt = 0; cnt < entries; ++cnt ) {

		LSA_UINT16 blklen;

		/*
		 * RS_EventBlock ::= RS_BlockHeader, [Padding]* a, RS_EventData, [Padding]* a
		 * a The number of padding octets shall be adapted to make the block Unsigned32 aligned.
		 *
		 * RS_BlockHeader ::= RS_BlockType, RS_BlockLength, BlockVersionHigh, BlockVersionLow
		 *
		 * RS_EventData ::= RS_EventDataCommon, [RS_EventDataExtension]*, [Padding]* a
		 * a The number of padding octets shall be adapted to make the block Unsigned32 aligned.
		 *
		 * RS_EventDataCommon ::= RS_AddressInfo, RS_Specifier, RS_TimeStamp, RS_MinusError, RS_PlusError
		 * ... 26 + 2+12+2+2 = 44 bytes
		 *
		 * RS_AddressInfo ::= IM_UniqueIdentifier, API, SlotNumber, SubslotNumber, ChannelNumber
		 * ... 16+4+2+2+2 = 26 bytes
		 */

		if( offs + 8 > size ) {
			return LSA_FALSE; /* too short for RS_BlockHeader and first Padding */
		}

		blklen = CM_GET16_NTOH(data, offs + 2);

		if( offs + 4 + blklen > size ) {
			return LSA_FALSE;
		}

		if(
			blklen < 48 /* too short for BlockVersion and first Padding and RS_EventDataCommon */
			|| (blklen & 3) != 0 /* see Padding at the end */
			|| CM_GET16_NTOH(data, offs + 0) == 0 /* RS_BlockType 0 is reserved */
			|| CM_GET16_NTOH(data, offs + 4) != 0x0100
			|| CM_GET16_NTOH(data, offs + 6) != 0 /* first Padding */
		) {
			return LSA_FALSE;
		}

		if( blklen > 48 ) { /* optional [RS_EventDataExtension]* exists */

			LSA_UINT32 ext_offs = offs + 4 + 48;
			LSA_UINT32 ext_size = offs + 4 + blklen;

			/* RS_EventDataExtension ::= RS_ExtensionBlockType, RS_ExtensionBlockLength, Data*, [Padding]* a */

			while( ext_offs < ext_size ) {

				LSA_UINT8 ext_blklen;

				if( ext_offs + 2 > ext_size ) {
					return LSA_FALSE;
				}

				ext_blklen = CM_GET8_NTOH(data, ext_offs + 1);

				if(
					ext_blklen < 2
					|| ext_offs + (2 + ext_blklen) > ext_size
					|| ((2 + ext_blklen) & 3) != 0
					|| CM_GET8_NTOH(data, ext_offs + 0) == 0 /* RS_ExtensionBlockType 0 is reserved */
				) {
					return LSA_FALSE;
				}

				ext_offs += (2 + ext_blklen);
			}

			if( ext_offs != ext_size ) {
				return LSA_FALSE;
			}
		}

		offs += 4 + blklen;
	}

	if( offs != size ) {
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== RESPONSE CHECK OF AssetManagementData =================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_record_response_AssetManagementData_ec1( /* TIA 1722142 */
	LSA_UINT8 * ndr_data,
	LSA_UINT32 ndr_length
) {
	LSA_UINT8 * data;
	LSA_UINT32 size;
	LSA_UINT32 offs;
	LSA_UINT16 entries;
	LSA_UINT16 cnt;

	data = ndr_data   + CM_RECORD_OFFSET;
	size = ndr_length - CM_RECORD_OFFSET;

	if( size == 0) {
		return 0x00; /* empty records allowed since FeatureSpec v1.21b */
	}

	/*
	 * AssetManagementData block header check
	 */

	if(
		size < 8 /* too short for BlockHeader and NumberOfEntries */
		|| CM_GET16_NTOH(data, 0) != 0x0035
		|| 4U + CM_GET16_NTOH(data, 2) != size
		|| CM_GET16_NTOH(data, 4) != 0x0100
	) {
		return 0xAA;
	}

	/*
	 * iterate NumberOfEntries
	 */

	entries = CM_GET16_NTOH(data, 6);

	if (entries == 0)
	{
		return 0xB0; /* always at least one AssetManagementBlock -> invalid index, see US */
	}

	offs = 8; /* 1st AssetManagementBlock */

	for (cnt = 0; cnt<entries; cnt++)
	{
		if (size < offs + 8+16+16) /* too short for following checks, see below */
		{
			return 0xAA;
		}

		if (CM_GET16_NTOH(data, offs + 4) != 0x0100) /* BlockVersion */
		{
			return 0xAA;
		}

		if (CM_GET16_NTOH(data, offs + 6) != 0x0000) /* Padding */
		{
			return 0xAA;
		}

		/* IM_UniqueIdentifier (UUID) must not be zero */
		if (CM_GET32_NTOH(data, offs + 8) == 0x00000000
			&& CM_GET32_NTOH(data, offs + 8 + 4) == 0x00000000
			&& CM_GET32_NTOH(data, offs + 8 + 8) == 0x00000000
			&& CM_GET32_NTOH(data, offs + 8 + 12) == 0x00000000
		) {
			return 0xAA;
		}

		{ /* AM_Location (16 octets, UINT128, big endian), TIA 2774819 */
		LSA_UINT32 am_offs = offs + 8+16;
		LSA_UINT8  AM_Location_Structure = CM_GET8_NTOH(data, am_offs + 15); /* Bit 0 - 7: AM_Location.Structure */

		if(
			AM_Location_Structure != 0x01
			&& AM_Location_Structure != 0x02
		) {
			return 0xAA; /* neither "Twelve level tree" nor "Slot- and SubslotNumber" */
		}

		if(
			AM_Location_Structure == 0x02
			&& CM_GET8_NTOH(data, am_offs + 14) != 0x00 /* Bit 8 – 15: AM_Location.Reserved1 */
		) {
			return 0xAA;
		}
		}

		{
		LSA_UINT16 block_type = CM_GET16_NTOH(data, offs);
		LSA_UINT16 block_length = CM_GET16_NTOH(data, offs + 2);
		LSA_UINT16 expected_block_length;

		switch(block_type)
		{
		case 0x0036:
			expected_block_length = 324; /* AM_FullInformation */
			break;
		case 0x0037:
			expected_block_length = 256; /* AM_HardwareOnlyInformation */
			break;
		case 0x0038:
			expected_block_length = 260; /* AM_FirmwareOnlyInformation */
			break;
		default:
			return 0xAA; /* unknown block_type */
		}

		if (block_length != expected_block_length
				|| offs + expected_block_length + 4 > size)
		{
			return 0xAA;
		}

		offs += block_length + 4; /* start of next AssetManagementBlock */
		}
	}

	if (offs != size)
	{
		return 0xAA;
	}

	return 0x00;
}

/*===========================================================================*/
/*=== RESPONSE OF A RECORD-EVENT ============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_record_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_read
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;

	CM_SV_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) sk(0x%x), is_read(%u)"
		, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
		, param->session_key
		, is_read
		);

	/* from the cm-documentation:
	 *
	 * read.rsp:
	 *   data_length = CM_RECORD_OFFSET + number of bytes read.
	 *   if cm_pnio_err != CM_PNIO_ERR_NONE data_length has to be set to CM_RECORD_OFFSET.
	 *
	 * write.rsp:
	 *   data_length = CM_RECORD_OFFSET + number of bytes written.
	 *   if cm_pnio_err != CM_PNIO_ERR_NONE data_length has to be set to CM_RECORD_OFFSET.
	 */

	if( param->u.sv.ar_rec.data_length < CM_RECORD_OFFSET ) {

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "data_length(%u) < CM_RECORD_OFFSET(%u)"
			, param->u.sv.ar_rec.data_length
			, CM_RECORD_OFFSET
			);

		CM_FATAL(); /* not as documented, cannot report (possibly a buffer-overflow) */
		/*lint -unreachable */
		return;
	}

	if( param->u.sv.ar_rec.cm_pnio_err != CM_PNIO_ERR_NONE ) {

		if( param->u.sv.ar_rec.data_length != CM_RECORD_OFFSET ) {

			CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "pnio_err(0x%x) but data_length(%u) not CM_RECORD_OFFSET(%u)"
				, param->u.sv.ar_rec.cm_pnio_err
				, param->u.sv.ar_rec.data_length
				, CM_RECORD_OFFSET
				);

			CM_FATAL(); /* not as documented, cannot report */
			/*lint -unreachable */
			return;
		}
	}

	/***/

	if( ! is_read ) {

		param->u.sv.ar_rec.data_length = CM_RECORD_OFFSET; /* must ignore the return-value; see PNIO-Spec definition of "RecordDataLength" */
	}

	/***/

	if (param->u.sv.ar_rec.record_index == 0xF880/*AssetManagementData*/) { /* TIA 1722142 */

		if (param->u.sv.ar_rec.cm_pnio_err == CM_PNIO_ERR_NONE) {

			LSA_UINT16 ec1 = cm_sv_record_response_AssetManagementData_ec1(param->u.sv.ar_rec.data, param->u.sv.ar_rec.data_length);

			if (ec1 != 0) {

				CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR/*sic!*/
					, "dev/ar(%u/%u) AssetManagementData: syntax error in response from user"
					, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
				);

				/***/

				param->u.sv.ar_rec.data_length = CM_RECORD_OFFSET;

				param->u.sv.ar_rec.cm_pnio_err = CM_PNIO_FAULTY_Record_PNIORW(ec1, 0x00);
				param->u.sv.ar_rec.ret_val_1 = 0;
				param->u.sv.ar_rec.ret_val_2 = 0;
			}
		}
	}

	/***/

	if( param->session_key == 0 ) { /* implicit-ar */

		CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, param->device_nr);

		if( cm_is_null(dev) ) {
			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "could not cm_sv_dev_lookup"
				);
			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
			/*lint -unreachable */
			return;
		}

		if( ! is_read ) {
			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "is_read(%u) not true (there is no write-implicit)", is_read
				);
			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
			/*lint -unreachable */
			return;
		}

		if( cm_is_null(dev->implicit_ar.rpc) ) {
			CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "no current implicit-ar rpc"
				);
			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
			/*lint -unreachable */
			return;
		}

		/***/

		if( param->u.sv.ar_rec.cm_pnio_err == CM_PNIO_ERR_NONE ) {

			cm_sv_rpc_mark_as_record_succeeded(dev->implicit_ar.rpc, is_read, param->u.sv.ar_rec.data_length);
		}
		else {

			cm_sv_rpc_mark_as_record_failed(dev->implicit_ar.rpc, is_read
				, param->u.sv.ar_rec.cm_pnio_err
				, param->u.sv.ar_rec.ret_val_1
				, param->u.sv.ar_rec.ret_val_2
				);
		}

		/***/

		cm_sv_dev_reprovide_event(dev, rb);

		/* NOTE: provide first, then process next */

		if( cm_sv_rpc_check_implicit_ar(dev) ) {

			cm_sv_rpc_next_implicit_ar(dev);
		}
	}
	else { /* explicit-ar */

		CM_AR_GRAPH_PTR_TYPE argr;

		argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_RECORD_RSP);

		if( cm_is_null(argr) ) {

			CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
				, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
				, param->session_key
				);

			CM_FATAL(); /* bug or cm-user fiddled with the RQB */
		}
		else {

			CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;

			CM_COMMON_MEM_U8_PTR_TYPE ndr_data = param->u.sv.ar_rec.data;
			LSA_UINT32 ndr_length = param->u.sv.ar_rec.data_length;
			LSA_UINT32 pnio_err  = param->u.sv.ar_rec.cm_pnio_err;
			LSA_UINT16 ret_val_1 = param->u.sv.ar_rec.ret_val_1;
			LSA_UINT16 ret_val_2 = param->u.sv.ar_rec.ret_val_2;

			argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_RECORD_RSP;

			if( cm_is_null(argr->cm.sv.curr_rpc_ind) ) {
				CM_FATAL();
			}

			/***/

			if( ! is_read && pnio_err != CM_PNIO_ERR_NONE ) { /* write.rsp(-) */

				LSA_UINT32 cls = cm_record_index_classify(channel, param->u.sv.ar_rec.api, param->u.sv.ar_rec.record_index, dev->vendor_id);

				if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) { /* connect-phase */

					if( (cls & CM_REC_IDX_E000) != 0 ) {

						/* AP00572036: CM has to set "a.r.p." on all submodules */

						CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "prm-write index(0x%x) failed, setting all submodules to appl-ready-pending"
							, param->u.sv.ar_rec.record_index
							);

						argr->cm.sv.ar_flags |= CM_SV_AR_FL_ARP_E000; /* see cm_sv_marshal_module_diff_block() */
					}
				}

				/***/

				if( param->u.sv.ar_rec.record_index == 0xE050/*ARFSUDataAdjust*/ ) {

					/* remove it if the user said "not ok", see comment in cm_sv_ARFSUDataAdjust_store() */

					cm_sv_ARFSUDataAdjust_remove(argr);
				}

				/***/

				if( argr->cm.sv.rec.coc_offs != 0 ) { /* TIA 1582512 */

					/* write.rsp(-) must lead to PRM-Diag only if written via COC (and has no effect otherwise) */

					if( CM_SV_SUBSLOT_IS_PDEV(param->u.sv.ar_rec.subslot_nr) ) {

						cm_sv_pdev_write_failed(argr, cls, param->u.sv.ar_rec.slot_nr, param->u.sv.ar_rec.subslot_nr, param->u.sv.ar_rec.record_index, 0);
					}
				}
			}

			/***/

			if( param->u.sv.ar_rec.record_index == 0xE060/*RS_GetEvent*/ ) { /* TIA 1702335 */

				if( ! cm_sv_record_response_RS_GetEvent_ok(pnio_err, ndr_data, ndr_length) ) {

					CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "dev/ar(%u/%u) RS_GetEvent: syntax error in response from user, aborting the AR"
						, argr->device_nr, argr->ar_nr
						);

					cm_ar_com_down(argr, CM_AR_REASON_READ);

					/***/

					ndr_length = CM_RECORD_OFFSET;

					pnio_err = CM_PNIO_FAULTY_Record__InvalidParameter(11);
					ret_val_1 = 0;
					ret_val_2 = 0;
				}
			}

			if( param->u.sv.ar_rec.record_index == 0xE061/*RS_AckEvent*/ ) { /* TIA 1702335 */

				if( pnio_err != CM_PNIO_ERR_NONE ) {

					CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "dev/ar(%u/%u) RS_AckEvent: pnio_err(0x%08x) from user != OK, aborting the AR"
						, argr->device_nr, argr->ar_nr, pnio_err
						);

					cm_ar_com_down(argr, CM_AR_REASON_WRITE);

					/***/

					ndr_length = CM_RECORD_OFFSET;

					pnio_err = CM_PNIO_FAULTY_Record__InvalidParameter(11);
					ret_val_1 = 0;
					ret_val_2 = 0;
				}
			}

			/***/

			cm_sv_dev_reprovide_event(dev, rb); /* NOTE: reprovide first, then process next */

			/***/

			if( argr->cm.sv.rec.coc_offs != 0 ) { /* TIA 1582512 */

				 /* note: compare to cm_sv_pdev_write_done() */

				if( is_read ) {
					CM_FATAL(); /* a bug */
				}

				if( pnio_err != CM_PNIO_ERR_NONE ) {

					/* note: all PRM-records are later discarded (see cm_sv_pdev_write_failed() above) */

					pnio_err = CM_PNIO_ERR_NONE; /* ignore, cannot report cnf(-) for a COContainerBlock */
				}

				if( argr->cm.sv.rec.coc_offs == argr->cm.sv.rec.coc_size ) { /* complete */

					argr->cm.sv.rec.coc_offs = 0;
				}
				else {

					argr->cm.sv.rec.coc_offs = ((argr->cm.sv.rec.coc_offs + 3) & ~3); /* align */

					if( ! cm_sv_pdev_CombinedObjectContainer_next(argr) ) {

						/* note: all PRM-records are later discarded */

						argr->cm.sv.rec.coc_offs = 0; /* abort */
					}
				}

				if( argr->cm.sv.rec.coc_offs != 0 ) {

					if( argr->cm.sv.rec.multi_offs != 0 ) {
						argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_RECORD_RSP; /* see cm_sv_record_write_embedded_next() */
					}

					return; /* pending */
				}

				ndr_data = argr->cm.sv.rec.coc_data; /* see write-multiple */

				pnio_err = CM_PNIO_ERR_NONE;
			}

			/***/

			if( argr->cm.sv.rec.multi_offs == 0 ) { /* single read or write */

				if( pnio_err != CM_PNIO_ERR_NONE ) {

					cm_sv_rpc_mark_as_record_failed(argr->cm.sv.curr_rpc_ind, is_read, pnio_err, ret_val_1, ret_val_2);
				}
				else {

					cm_sv_rpc_mark_as_record_succeeded(argr->cm.sv.curr_rpc_ind, is_read, ndr_length);
				}
			}
			else { /* write-multiple */

				if( is_read ) {
					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
						, "is_read(%u) not false (there is no read-multiple)", is_read
						);
					CM_FATAL(); /* bug or cm-user fiddled with the RQB */
					/*lint -unreachable */
					return;
				}

				/***/

				cm_sv_record_write_embedded_update(
					argr,
					ndr_data + CLRPC_PNIO_OFFSET,
					pnio_err, ret_val_1, ret_val_2
					);

				/***/

				argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_RECORD_RSP; /* tricky */

				cm_sv_record_write_embedded_next(argr);
			}

			/***/

			cm_sv_rpc_check_explicit_ar(argr);
		}
	}
}

/*===========================================================================*/
/*=== AR PDEVPRM-READ/WRITE =================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_pdevprm_rdwr(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_BOOL is_read
) {
	CM_UPPER_SV_AR_PDEVPRM_RDWR_PTR_TYPE param = rb->args.sv.ar_pdevprm_rdwr;
	CM_UPPER_RQB_PTR_TYPE pd = LSA_NULL;
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 rsp;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_PDEVPRM_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( cm_is_not_null(argr->cm.sv.curr_ar_pdevprm) ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) another pdevprm-request still running, opc(%u)"
			, argr->device_nr, argr->ar_nr
			, CM_RQB_GET_OPCODE(argr->cm.sv.curr_ar_pdevprm)
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		LSA_UINT32 cls = cm_record_index_classify(channel, 0/*!*/, param->rec.record_index, argr->cm.sv.dev->vendor_id);

		if( (cls & (CM_REC_IDX_INTERFACE | CM_REC_IDX_PORT)) == 0 ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev/ar(%u/%u) rec.index(0x%x) not interface/port"
				, argr->device_nr, argr->ar_nr
				, param->rec.record_index
				);

			rsp = CM_ERR_PARAM;
		}
		else if( cm_is_null(param->rec.record_data) && (is_read || param->rec.record_data_length != 0) ) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev/ar(%u/%u) rec.record_data is null"
				, argr->device_nr, argr->ar_nr
				);

			rsp = CM_ERR_PARAM;
		}
		else if( ! is_read && (param->rec.mk_remanent != CM_PD_MK_REMANENT_NO && param->rec.mk_remanent != CM_PD_MK_REMANENT_YES) ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "mk_remanent(%u) is out of range"
				, param->rec.mk_remanent
				);

			rsp = CM_ERR_PARAM;
		}
		else if( ! is_read && (param->rec.is_local != CM_PD_IS_LOCAL_NO && param->rec.is_local != CM_PD_IS_LOCAL_YES) ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "is_local(%u) is out of range"
				, param->rec.is_local
				);

			rsp = CM_ERR_PARAM;
		}
		else {

			/* note: more checks are done by CM-PD (mapping, readable, writeable) */
			/* note: the AR is the pdev-owner, see where CM_SV_AR_CNFL_WF_PDEVPRM_RSP is set */
			/* note: "plugged" is not checked (by design) */
			/* note: "contained in AR" is not checked (by design) */

			CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
				, "dev/ar(%u/%u) ap/sl/su(0/%u/%u) index(0x%x) is_read(%u)"
				, argr->device_nr, argr->ar_nr
				, param->rec.slot_nr, param->rec.subslot_nr
				, param->rec.record_index
				, is_read
				);

			if( cm_ar_com_is_down(argr) ) {

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev/ar(%u/%u) already down"
					, argr->device_nr, argr->ar_nr
					);

				rsp = CM_OK_CANCELLED; /* late (this AR will not parameterize the PDEV) */
			}
			else if( ! cm_sv_ar_is_pdev_owner(argr) ) {

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
					, "dev/ar(%u/%u) not pdev-owner"
					, argr->device_nr, argr->ar_nr
					);

				rsp = CM_OK_CANCELLED; /* unreachable (until "takeover") */
			}
			else {

				pd = argr->cm.sv.pdevprm_rdwr_rqb;

				if( cm_is_null(pd) || CM_RQB_GET_OPCODE(pd) != 0/*unused*/ ) {

					rsp = CM_OK_CANCELLED; /* to avoid a warning */

					CM_FATAL(); /* a bug */
				}
				else {

					if( is_read ) {

						if( (cls & CM_REC_IDX_NONPRM) == 0 ) { /* modify classification for "prm" indices: read record from B-buffer */

							cls = ((cls & ~CM_REC_IDX_CMPD_MASK) | CM_REC_IDX_CMPD_BBUF); /* see cm_pd_prm_sc_read() */
						}
					}
					else { /* write */

#if CM_CFG_USE_MRP
						if( argr->cm.sv.set_mrp_off && (cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_MRP ) {

							/* note: User must care for "MRP_Off" record when starting fiddling with MRP-records (TIA 1323566) */
							argr->cm.sv.set_mrp_off = LSA_FALSE;
						}
#endif

						/***/

						if( param->rec.record_data_length == 0 ) { /* writing 0 bytes means "delete" */

							if( (cls & CM_REC_IDX_NONPRM) == 0 ) { /* modify classification for "prm" indices: delete record from B-buffer */

								cls = ((cls & ~CM_REC_IDX_CMPD_MASK) | CM_REC_IDX_CMPD_BBUF); /* see caller of cm_pd_reclist_delete() */
							}
						}
					}

					/***/

					{
					CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;

					prm_rdwr->slot_nr            = param->rec.slot_nr;
					prm_rdwr->subslot_nr         = param->rec.subslot_nr;
					prm_rdwr->record_index       = param->rec.record_index;
					prm_rdwr->record_data_length = param->rec.record_data_length;
					prm_rdwr->record_data        = param->rec.record_data;

					prm_rdwr->mk_remanent = param->rec.mk_remanent;
					prm_rdwr->is_local    = param->rec.is_local;

					prm_rdwr->cm_pnio_err = 0; /* sanity, not used by pdevprm-read/write */
					prm_rdwr->ret_val_1   = 0; /* sanity, not used by pdevprm-read/write */
					prm_rdwr->ret_val_2   = 0; /* sanity, not used by pdevprm-read/write */

					prm_rdwr->cls = cls;
					}

					argr->cm.sv.curr_ar_pdevprm = rb; /* pending */

					rsp = CM_OK_PENDING;
				}
			}
		}
	}

	if( rsp != CM_OK_PENDING || cm_is_null(pd) || cm_is_null(argr) ) {

		cm_callback(channel, rsp, &rb);
	}
	else {

		LSA_OPCODE_TYPE opcode = (LSA_OPCODE_TYPE)(is_read ? CM_OPC_PD_PRM_READ_SERVER : CM_OPC_PD_PRM_WRITE_SERVER);

		cm_sv_pdev_request(channel, opcode, argr->device_nr, argr->ar_nr, pd);
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
