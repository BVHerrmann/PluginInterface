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
/*  F i l e               &F: cm_sv49.c                                 :F&  */
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
/*  - ARFSUDataAdjust                                                        */
/*  - interface to CM-PD                                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7490
#define CM_MODULE_ID       7490

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== ARFSUDataAdjust =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ARFSUDataAdjust_init(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	/* AP00511230:
	 *   ARFSUDataAdjust shall be stored in the AR
	 */

	argr->cm.sv.rec.ARFSUDataAdjust_ptr = LSA_NULL;
	argr->cm.sv.rec.ARFSUDataAdjust_len = 0;
}

/*===========================================================================*/

LSA_VOID
cm_sv_ARFSUDataAdjust_remove(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	if( cm_is_not_null(argr->cm.sv.rec.ARFSUDataAdjust_ptr) ) {

		{
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM(&ret_val, argr->cm.sv.rec.ARFSUDataAdjust_ptr);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		}

		argr->cm.sv.rec.ARFSUDataAdjust_ptr = LSA_NULL;
		argr->cm.sv.rec.ARFSUDataAdjust_len = 0;
	}
}

/*===========================================================================*/

LSA_BOOL
cm_sv_ARFSUDataAdjust_check(
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_BOOL is_ARFSUBlock /* AP01034170 */
) {
	LSA_UINT32 offs;
	LSA_UINT16 block_type;
	LSA_UINT16 block_length;
	LSA_UINT32 block_count;

	/*
	 * ARFSUDataAdjust ::=
	 *   BlockHeader, [Padding*] a, { [FSParameterBlock], [FastStartUpBlock] } b
	 *
	 * a The number of padding octets shall be adapted to make the block Unsigned32 aligned.
	 * b At least one optional block shall be existing.
	 *
	 *
	 * FSParameterBlock ::=
	 *   BlockHeader, [Padding*] a, FSParameterMode, FSParameterUUID, [Padding*] a
	 *
	 * FastStartUpBlock ::=
	 *   BlockHeader, [Padding*] a, Data*, [Padding*] a
	 */

	offs = (is_ARFSUBlock ? 0 : CM_RECORD_OFFSET);

	if( offs + 2+2 > size ) {

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "size(%u) too short"
			, size
			);

		return LSA_FALSE;
	}

	block_type   = CM_GET16_NTOH(data, offs + 0);
	block_length = CM_GET16_NTOH(data, offs + 2);

	if( offs + 2+2 + block_length != size ) {

		CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP
			, "block_type(0x%x), block_length(%u) not consistent with size(%u)"
			, block_type, block_length, size
			);

		return LSA_FALSE;
	}

	if(
		block_type != (is_ARFSUBlock ? CM_BLOCK_TYPE_ARFSUBLOCK : CM_BLOCK_TYPE_ARFSUDATAADJUST)
		|| block_length < 1+1/*version*/ + 2/*padding*/
	) {

		CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
			, "block_type(0x%x) wrong or block_length(%u) too short"
			, block_type, block_length
			);

		return LSA_FALSE;
	}

	offs += 2+2 + 1+1/*version*/ + 2/*padding*/;

	/***/

	block_count = 0;

	while( offs < size ) {

		/*
		 * note: it was decided (24.9.2007) that the block_type is only range-checked here
		 *       thus more optional blocks may be defined without having to change this code
		 */

		if( offs + 2+2 > size ) {

			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
				, "too short for another optional block"
				);

			return LSA_FALSE;
		}

		block_type    = CM_GET16_NTOH(data, offs + 0);
		block_length  = CM_GET16_NTOH(data, offs + 2);

		if( ! (
			(block_type >= 0x0600 && block_type <= 0x060F) /* the range assigned to fast startup */
			&& block_type != CM_BLOCK_TYPE_ARFSUDATAADJUST /* but not the "outer" block */
		) ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "optional block_type(0x%x) wrong"
				, block_type
				);

			return LSA_FALSE;
		}

		if( offs + 2+2 + block_length > size ) {

			CM_SV_TRACE_03(0, LSA_TRACE_LEVEL_UNEXP
				, "optional block_type(0x%x), block_length(%u) not consistent with size(%u)"
				, block_type, block_length, size
				);

			return LSA_FALSE;
		}

		if(
			block_length < 1+1/*version*/
			|| (block_length % 4) != 0
		) {

			CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "optional block_type(0x%x), block_length(%u) too short or not 0 mod 4"
				, block_type, block_length
				);

			return LSA_FALSE;
		}

		/***/

		offs += 2+2 + block_length;

		block_count += 1;
	}

	if( block_count < 1 ) {

		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "not 'at least one optional block'"
			);

		return LSA_FALSE;
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/

LSA_BOOL
cm_sv_ARFSUDataAdjust_store(
	CM_AR_GRAPH_PTR_TYPE argr,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_BOOL is_ARFSUBlock /* AP01034170 */
) {
	LSA_BOOL ok;

	/* AP00511230:
	 *   ARFSUDataAdjust shall be written only in the connect-phase
	 *   - RQ-comment: checking the phase is not necessary
	 *   - we make a copy of the record here because the data_length in the record.rsp may have been changed by the user
	 *   - we invalidate it later in cm_sv_opc_ar_record_response() if the user said "not ok"
	 */

	cm_sv_ARFSUDataAdjust_remove(argr); /* remove if exists (e.g. rerun) */

	/***/

	if( size > 0xFFFF ) {

		/* note: data_length <= ndr_len <= alloc_len < 64K */

		ok = LSA_FALSE; /* actually unreachable */
	}
	else {

		if( ! is_ARFSUBlock ) {

			data += CM_RECORD_OFFSET;
			size -= CM_RECORD_OFFSET;
		}

		CM_ALLOC_LOCAL_MEM(&argr->cm.sv.rec.ARFSUDataAdjust_ptr, size);

		if( cm_is_null(argr->cm.sv.rec.ARFSUDataAdjust_ptr) ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) could not alloc(%u) ARFSUDataAdjust"
				, argr->device_nr, argr->ar_nr, size
				);

			CM_ASSERT(argr->cm.sv.rec.ARFSUDataAdjust_len == 0);

			ok = LSA_FALSE;
		}
		else {

			CM_MEMCPY(argr->cm.sv.rec.ARFSUDataAdjust_ptr, data, size);

			argr->cm.sv.rec.ARFSUDataAdjust_len = CM_CAST_U16(size);

			ok = LSA_TRUE;
		}
	}

	return ok;
}

/*===========================================================================*/

LSA_UINT32
cm_sv_ARFSUDataAdjust_read(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req
) {
	LSA_UINT32 pnio_err;
	LSA_UINT32 offs;

	/* AP00511230:
	 *   reading ARFSUDataAdjust shall be short-cut by CM (no user-record.ind)
	 *   returning "0 Bytes + OK" if it was not written (analogous to PDPortDataAdjust)
	 */

	offs = CM_RECORD_OFFSET;

	if( offs + argr->cm.sv.rec.ARFSUDataAdjust_len > req->record_data_length/*!*/ ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "buffer too small");

		pnio_err = cm_pd_response_to_pnio_err(CM_ERR_PRM_DATA, req->is_read); /* data-length too short */
	}
	else {

		if( cm_is_not_null(argr->cm.sv.rec.ARFSUDataAdjust_ptr) ) {

			CLRPC_UPPER_MEM_U8_PTR_TYPE data = rpc->args.sv.call->ndr_data;

			CM_MEMCPY(data + offs, argr->cm.sv.rec.ARFSUDataAdjust_ptr, argr->cm.sv.rec.ARFSUDataAdjust_len);

			offs += argr->cm.sv.rec.ARFSUDataAdjust_len;
		}
		else { /* no record */

			CM_ASSERT(offs == CM_RECORD_OFFSET);
		}

		/***/

		cm_sv_rpc_mark_as_record_succeeded(rpc, req->is_read, offs);

		pnio_err = CM_PNIO_ERR_NONE;
	}

	return pnio_err;
}

/*===========================================================================*/
/*=== I&M1-4 CHECKS =========================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_im_datepart_valid(
	LSA_UINT8 const * dp,
	LSA_UINT16 length, /* 1-4 */
	LSA_UINT16 minval,
	LSA_UINT16 maxval
) {
	LSA_UINT32 val = 0;
	LSA_UINT16 idx;

	for(idx = 0; idx < length; idx++)
	{
		LSA_UINT8 ch = dp[idx];

		if( ch < 0x30 || ch > 0x39 )
		{
			return LSA_FALSE; /* not within 0-9 */
		}

		/* calc date part value */
		val = (val * 10) + (ch - 0x30); /* 0x30 = '0' */
	}

	if (val < minval || val > maxval)
	{
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_im_date_valid(
	LSA_UINT8 const * dp
) {
	LSA_UINT8 idx;

	/* PNIO-Spec, checking rules changed (AP01520761)
	 *
	 * IM_Date ::=
	 *   all blanks
	 *   ^ "YYYY–MM–DD HH:MM"
	 *   ^ "YYYY–MM–DD      "
	 */

	if( dp[0] == 0x20 ) {

		idx = 1; /* rest must be blanks (case: all blanks) */
	}
	else {

		if( ! cm_sv_im_datepart_valid(dp + 0, 4, 0, 9999) ) {
			return LSA_FALSE; /* year out of range */
		}

		if( dp[4] != 0x2D ) {
			return LSA_FALSE; /* not "-" */
		}

		if( ! cm_sv_im_datepart_valid(dp + 5, 2, 1, 12) ) {
			return LSA_FALSE; /* month out of range */
		}

		if( dp[7] != 0x2D ) {
			return LSA_FALSE; /* not "-" */
		}

		if( ! cm_sv_im_datepart_valid(dp + 8, 2, 1, 31) ) {
			return LSA_FALSE; /* day out of range */
		}

		/***/

		if( dp[10] != 0x20 ) {
			return LSA_FALSE; /* not " " */
		}

		/***/

		if( dp[11] == 0x20 ) {

			idx = 12; /* rest must be blanks (case: YYYY–MM–DD without time) */
		}
		else {

			if( ! cm_sv_im_datepart_valid(dp + 11, 2, 0, 23) ) {
				return LSA_FALSE; /* hour out of range */
			}

			if( dp[13] != 0x3A ) {
				return LSA_FALSE; /* not ":" */
			}

			if( ! cm_sv_im_datepart_valid(dp + 14, 2, 0, 59) ) {
				return LSA_FALSE; /* minutes out of range */
			}

			idx = 16;
		}
	}

	/***/

	for( ; idx < 16; ++idx ) {

		if( dp[idx] != 0x20 ) {
			return LSA_FALSE; /* not " " */
		}
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_im_visiblestring_valid(
	LSA_UINT8 const * dp,
	LSA_UINT16 len
) {
	LSA_UINT16 idx;

	for(idx = 0; idx < len; idx++)
	{
		LSA_UINT8 ch = dp[idx];

		/* AP01534257: "FDIS 61158-6-10_v23ed2_FINAL__CBPG6_TO_PNO_CO__Rev19.docm" clarified the meaning of "visible"
		 *
		 * Chapter "Notation for VisibleString type"
		 *   ISO/IEC 646 - International Reference Version without the "del" (coding 0x7F) character (i.e., 0x00 - 0x7E)
		 *
		 * Chapter "Coding section related to I&M Records"
		 *   The engineering shall only use the visible characters for VisibleString ... (i.e., 0x20 - 0x7E)
		 */

		if (ch > 0x7E)
		{
			return LSA_FALSE;
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_UINT32
cm_sv_IM1_check(
	LSA_UINT8 * data,
	LSA_UINT32 length
) {
	LSA_UINT16 blocktype;
	LSA_UINT16 blocklen;
	LSA_UINT16 blockversion;
	LSA_UINT16 offset;

	/* I&M1 ::= BlockHeader, IM_Tag_Function(VisibleString[32]), IM_Tag_Location(VisibleString[22]) */

	if (length != 2+2 + 1+1 + 32 + 22) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blocktype = CM_GET16_NTOH(data, 0);
	if (blocktype != CM_BLOCK_TYPE_I_M_1) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	blocklen = CM_GET16_NTOH(data, 2);
	if (blocklen != 1+1 + 32 + 22) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blockversion = CM_GET16_NTOH(data, 2+2);
	if (blockversion != 0x0100) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	offset = 2+2 + 1+1;

	if (!cm_sv_im_visiblestring_valid(data + offset, 32)) {
		return CM_PNIO_FAULTY_Record__InvalidType(0); /* invalid IM_Tag_Function */
	}

	offset += 32;

	if (!cm_sv_im_visiblestring_valid(data + offset, 22)) {
		return CM_PNIO_FAULTY_Record__InvalidType(0); /* invalid IM_Tag_Location */
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_UINT32
cm_sv_IM2_check(
	LSA_UINT8 * data,
	LSA_UINT32 length
) {
	LSA_UINT16 blocktype;
	LSA_UINT16 blocklen;
	LSA_UINT16 blockversion;
	LSA_UINT16 offset;

	/* I&M2 ::= BlockHeader, IM_Date */

	if (length != 2+2 + 1+1 + 16) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blocktype = CM_GET16_NTOH(data, 0);
	if (blocktype != CM_BLOCK_TYPE_I_M_2) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	blocklen = CM_GET16_NTOH(data, 2);
	if (blocklen != 1+1 + 16) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blockversion = CM_GET16_NTOH(data, 2+2);
	if (blockversion != 0x0100) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	offset = 2+2 + 1+1;

	if (!cm_sv_im_date_valid(data + offset)) {
		return CM_PNIO_FAULTY_Record__InvalidType(0); /* invalid IM_Date */
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_UINT32
cm_sv_IM3_check(
	LSA_UINT8 * data,
	LSA_UINT32 length
) {
	LSA_UINT16 blocktype;
	LSA_UINT16 blocklen;
	LSA_UINT16 blockversion;
	LSA_UINT16 offset;

	/* I&M3 ::= BlockHeader, IM_Descriptor(VisibleString[54]) */

	if (length != 2+2 + 1+1 + 54) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blocktype = CM_GET16_NTOH(data, 0);
	if (blocktype != CM_BLOCK_TYPE_I_M_3) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	blocklen = CM_GET16_NTOH(data, 2);
	if (blocklen != 1+1 + 54) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blockversion = CM_GET16_NTOH(data, 2+2);
	if (blockversion != 0x0100) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	offset = 2+2 + 1+1;

	if (!cm_sv_im_visiblestring_valid(data + offset, 54)) {
		return CM_PNIO_FAULTY_Record__InvalidType(0); /* invalid IM_Descriptor */
	}

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

static LSA_UINT32
cm_sv_IM4_check(
	LSA_UINT8 * data,
	LSA_UINT32 length
) {
	LSA_UINT16 blocktype;
	LSA_UINT16 blocklen;
	LSA_UINT16 blockversion;

	/* I&M4 ::= BlockHeader, IM_Signature(OctetString[54]) */

	if (length != 2+2 + 1+1 + 54) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blocktype = CM_GET16_NTOH(data, 0);
	if (blocktype != CM_BLOCK_TYPE_I_M_4) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	blocklen = CM_GET16_NTOH(data, 2);
	if (blocklen != 1+1 + 54) {
		return CM_PNIO_FAULTY_Record__WriteLengthError(0);
	}

	blockversion = CM_GET16_NTOH(data, 2+2);
	if (blockversion != 0x0100) {
		return CM_PNIO_FAULTY_Record__InvalidType(0);
	}

	/* no checks for OctetString */

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/

LSA_UINT32
cm_sv_IM1234_check( /* AP01338246 */
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT16 index
) {
	LSA_UINT32 pnio_err;

	data += CM_RECORD_OFFSET;
	size -= CM_RECORD_OFFSET;

	switch( index ) {

	case 0xAFF1: /* I&M1 */
		pnio_err = cm_sv_IM1_check(data, size);
		break;

	case 0xAFF2: /* I&M2 */
		pnio_err = cm_sv_IM2_check(data, size);
		break;

	case 0xAFF3: /* I&M3 */
		pnio_err = cm_sv_IM3_check(data, size);
		break;

	case 0xAFF4: /* I&M4 */
		pnio_err = cm_sv_IM4_check(data, size);
		break;

	default:
		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0); /* unreachable */
		break;
	}

	return pnio_err;
}

/*===========================================================================*/
/*=== COMBINED OBJECT CONTAINER (TIA 1092258) ===============================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_pdev_CombinedObjectContainer_next(
	CM_AR_GRAPH_PTR_TYPE argr
) {
	CLRPC_UPPER_MEM_U8_PTR_TYPE data = argr->cm.sv.rec.coc_data;

	if( cm_ar_com_is_down(argr) ) { /* must not indicate after abort/offline */

		return LSA_FALSE;
	}
	else if( argr->cm.sv.rec.coc_offs >= argr->cm.sv.rec.coc_size ) {

		CM_FATAL(); /* a bug */
	}
	else {

		LSA_UINT32 offs = argr->cm.sv.rec.coc_offs;

		if( CM_GET16_NTOH(data, offs) != CM_BLOCK_TYPE_COCONTAINERCONTENT ) {

			CM_FATAL(); /* a bug */
		}
		else {

			LSA_UINT32 coclen = 2+2 + CM_GET16_NTOH(data, offs + 2); /* COContainerContent */

			argr->cm.sv.rec.coc_offs += coclen; /* prepare for next */

			if( argr->cm.sv.rec.coc_offs > argr->cm.sv.rec.coc_size ) {

				CM_FATAL(); /* a bug */
			}
			else {

				CM_SV_RECORD_PARAMS_TYPE req;
				CLRPC_UPPER_MEM_U8_PTR_TYPE ndr_data;
				LSA_UINT32 pnio_err;

				req.is_read = LSA_FALSE;
				req.is_read_implicit = LSA_FALSE;

				req.api        = CM_GET32_NTOH(data, offs +  8);
				req.slot_nr    = CM_GET16_NTOH(data, offs + 12);
				req.subslot_nr = CM_GET16_NTOH(data, offs + 14);
				req.index      = CM_GET16_NTOH(data, offs + 18);

				req.seq_number  = CM_GET16_NTOH(data, CLRPC_PNIO_OFFSET + CM_BLK_RDWR_seq_number); /* from container (TIA 1582512) */

				ndr_data               = data + ((offs + 20) - CM_RECORD_OFFSET);
				req.record_data_length = (coclen - 20) + CM_RECORD_OFFSET;

				req.cls  = cm_record_index_classify(argr->ar_com.channel, req.api, req.index, argr->cm.sv.dev->vendor_id);
				req.argr = argr;

				if( (req.cls & CM_REC_IDX_USER) != 0 ) { /* a COC may contain "user" records (TIA 1582512) */

					LSA_BOOL fin;

					argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_RECORD_RSP; /* note: already set if write-multiple */

					fin = cm_sv_ar_record_indicate(
						argr->cm.sv.dev, argr, LSA_TRUE,
						&req, ndr_data, /* pass-in the clrpc-buffer! */
						&pnio_err
					);

					if( fin ) {

						if( pnio_err != CM_PNIO_ERR_NONE ) {

							/* don't care (fin already means error) */
						}

						if( CM_SV_SUBSLOT_IS_PDEV(req.subslot_nr) ) {

							cm_sv_pdev_write_failed(argr, req.cls, req.slot_nr, req.subslot_nr, req.index, 0);
						}

						if( argr->cm.sv.rec.multi_offs == 0 ) { /* single write */

							argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_RECORD_RSP;
						}

						return LSA_FALSE;
					}
				}
				else if( (req.cls & CM_REC_IDX_PDEV) != 0 ) {

					pnio_err = cm_sv_pdev_read_write(argr->cm.sv.dev, &req, ndr_data, argr->ar_nr);

					if( pnio_err != CM_PNIO_ERR_NONE ) {

						return LSA_FALSE;
					}
				}
				else {

					CM_FATAL(); /* a bug in cm_sv_pdev_CombinedObjectContainer_check() */
				}
			}
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_UINT32
cm_sv_pdev_CombinedObjectContainer_check(
	CM_CHANNEL_PTR_TYPE channel,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 coc_api,     /* note: always 0 */
	LSA_UINT16 coc_slot,    /* note: ports may be plugged into different slots (modular switch) */
	LSA_UINT16 coc_subslot, /* note: plugged and 0x8i00 (access point = pdev-interface) */
	LSA_UINT16 vendor_id,
	LSA_BOOL * is_access_denied,
	LSA_BOOL is_local_pdev
) {
	/*
	 * CombinedObjectContainer ::= ( COContainerContent, [Padding]* <a> )*
	 *   <a> The number of padding octets (value=0) shall be 0,1,2,3 to have 32 bit alignment to the next COContainerContent
	 *
	 * COContainerContent ::=
	 *   [ 0] BlockHeader, Padding, Padding,
	 *   [ 8] API, Slot, Subslot,
	 *   [16] Padding, Padding, Index,
	 *   [20] COContainerBlock
	 *
	 * COContainerBlock ::= RecordDataWrite <a>
	 *   <a> Without CombinedObjectContainer
	 */

	LSA_UINT32 offs = CM_RECORD_OFFSET;

	*is_access_denied = LSA_FALSE;

	if( offs >= size ) {

		return offs; /* too few data (empty) */
	}

	 while( offs < size ) {

		LSA_UINT16 blen;

		if( offs + 2+2 > size ) {

			return offs; /* too few data (for block-type and block-length) */
		}

		if( CM_GET16_NTOH(data, offs) != CM_BLOCK_TYPE_COCONTAINERCONTENT ) {

			return offs; /* wrong block-type (of COContainerContent) */
		}

		blen = CM_GET16_NTOH(data, offs + 2); /* size of COContainerBlock */

		if( offs + 2+2 + blen > size ) {

			return offs + 2; /* block-length vs. size */
		}

		if( 1+1 + 1+1 + 4+2+2 + 1+1 + 2 + 1 > blen ) {

			return offs + 2; /* too few data (for a COContainerContent with non-empty RecordDataWrite) */
		}
		else {

			LSA_UINT16 slot;
			LSA_UINT16 subslot;
			LSA_UINT16 index;
			LSA_UINT32 cls;

			if( CM_GET8_NTOH(data, offs + 4) != 0x01 ) {

				return offs + 4; /* wrong block-version-high (of COContainerContent) */
			}

			if( CM_GET8_NTOH(data, offs + 5) != 0x00 ) {

				return offs + 5; /* wrong block-version-low (of COContainerContent) */
			}

			if( CM_GET16_NTOH(data, offs + 6) != 0 ) {

				return offs + 6; /* padding after block-header not zero */
			}

			if( CM_GET32_NTOH(data, offs +  8) != coc_api ) {

				*is_access_denied = LSA_TRUE; /* not part of the COC group (api does not match the COC::api) */
				return offs + 8;
			}

			slot = CM_GET16_NTOH(data, offs + 12);

			if( slot != coc_slot ) {

				/* ok (usecase: modular switch) */
			}

			subslot = CM_GET16_NTOH(data, offs + 14);

			if( (subslot & 0xFF00) != (coc_subslot & 0xFF00) ) {

				*is_access_denied = LSA_TRUE; /* not part of the COC group (8i/9j part does not match the COC::subslot) */

				return offs + 14;
			}

			if( is_local_pdev ) {

				/* note: only check for "mapped in CM-PD" not for "plugged in CM-SV" */

				if( ! cm_pdsv_mapping_ok(channel, slot, subslot) ) {

					*is_access_denied = LSA_TRUE; /* not part of the COC group (not mapped) */

					return offs + 14;
				}
			}

			if( CM_GET16_NTOH(data, offs + 16) != 0 ) {

				return offs + 16; /* padding before index not zero */
			}

			index = CM_GET16_NTOH(data, offs + 18);

			cls = cm_record_index_classify(channel, coc_api, index, vendor_id);

			if(
				index == 0x80B0/*CombinedObjectContainer*/
				|| (cls & (CM_REC_IDX_8000 | CM_REC_IDX_0000)) == 0 /* note: this includes the Simatic indizes 0xBxxx */
				|| (cls & CM_REC_IDX_WRITE) == 0
				|| (cls & (CM_REC_IDX_PDEV | CM_REC_IDX_USER)) == 0
				/* note: no other checks here! */
			) {

				return offs + 18; /* not a record for the submodules of the COC group */
			}
		}

		offs += 2+2 + blen;

		if( offs < size ) { /* another COContainerContent follows */

			while( (offs & 3) != 0 ) { /* alignment */

				if( offs + 1 > size ) {

					return offs;
				}

				if( CM_GET8_NTOH(data, offs) != 0 ) {

					return offs; /* padding between COContainerContent not zero */
				}

				offs += 1;
			}

			if( offs >= size ) {

				return offs; /* too few data (nothing after alignment-padding) */
			}
		}
	}

	if( offs != size ) {

		return offs; /* unreachable */
	}

	return 0; /* ok */
}

/*===========================================================================*/

LSA_UINT32
cm_sv_pdev_CombinedObjectContainer_write(
	CM_AR_GRAPH_PTR_TYPE argr,
	CLRPC_UPPER_MEM_U8_PTR_TYPE ndr_data,
	CM_SV_RECORD_PARAMS_PTR_TYPE req
) {
	LSA_UINT32 pnio_err;

	/* note: checking context on start of COC iteration is sufficient because no error reporting for embedded records */

	pnio_err = cm_sv_record_check_range_and_context(argr, req, LSA_TRUE);

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_WARN
			, "dev/ar(%u/%u) cm_sv_record_check_range_and_context, pnio_err(0x%x)"
			, argr->device_nr, argr->ar_nr
			, pnio_err
			);
	}
	else {

		LSA_BOOL is_access_denied = LSA_FALSE;
		LSA_BOOL is_local_pdev = LSA_FALSE;
		LSA_UINT32 offs;

		if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, req->subslot_nr) ) {

			is_local_pdev = LSA_TRUE;
		}

		offs = cm_sv_pdev_CombinedObjectContainer_check(argr->ar_com.channel, ndr_data, req->record_data_length, req->api, req->slot_nr, req->subslot_nr, argr->cm.sv.dev->vendor_id, &is_access_denied, is_local_pdev);

		if( offs != 0 ) {

			CM_SV_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) COC: check failed, offset(%u)"
				, argr->device_nr, argr->ar_nr
				, req->api, req->slot_nr, req->subslot_nr
				, offs
				);

			cm_sv_pdev_write_failed(argr, req->cls, req->slot_nr, req->subslot_nr, req->index, (offs - CM_RECORD_OFFSET));

			if( is_access_denied ) {

				pnio_err = CM_PNIO_FAULTY_Record__AccessDenied(0); /* any subslot not part of the COC group */
			}
			else {

				pnio_err = cm_pd_response_to_pnio_err(CM_ERR_PRM_BLOCK, LSA_FALSE);
			}
		}
		else {

			if( argr->cm.sv.rec.coc_offs != 0 ) {
				CM_FATAL(); /* a bug (iterator not idle) */
			}

			argr->cm.sv.rec.coc_offs = CM_RECORD_OFFSET;
			argr->cm.sv.rec.coc_data = ndr_data;
			argr->cm.sv.rec.coc_size = req->record_data_length;

			if( ! cm_sv_pdev_CombinedObjectContainer_next(argr) ) {

				argr->cm.sv.rec.coc_offs = 0; /* abort */

				pnio_err = cm_pd_response_to_pnio_err(CM_ERR_PRM_BLOCK, LSA_FALSE); /* tell something wrong with the block-data */
			}
			else {

				pnio_err = CM_PNIO_ERR_NONE; /* pending */
			}
		}
	}

	return pnio_err;
}

/*===========================================================================*/
/*=== CM-PD DIAG CHANGED ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pdev_diag_changed(
	CM_CHANNEL_PTR_TYPE channel /* the pd-channel */
) {

	CM_ASSERT(channel->path_type == CM_PATH_TYPE_PD);

	channel = cm_peer_channel_get(channel, CM_PATH_TYPE_SERVER);

	/***/

	if( cm_is_null(channel) ) {

		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_CHAT
			, "no server-channel, ignored"
			);

		/* ignore, server not yet opened or already closed */
	}
	else if( channel->state != CM_SV_CREATED ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "channel-state(%u) not created, ignored"
			, channel->state
			);

		/* ignore, server not yet created or already deleted */
	}
	else if( channel->usr.sv.pdev_device_nr == 0 ) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "no pdev-device, ignored"
			);

		/* ignore, device with pdev not yet added or already removed */
	}
	else {

		CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_get_added(channel, channel->usr.sv.pdev_device_nr);

		if( cm_is_not_null(dev) ) {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "dev(%u)"
				, dev->device_nr
				);

			if( dev->pdev_owner_ar_nr != 0 ) {

				CM_AR_GRAPH_PTR_TYPE owner_argr = cm_sv_ar_lookup2(dev, dev->pdev_owner_ar_nr);

				if( cm_is_not_null(owner_argr) ) { /* always true */

					if( cm_is_not_null(owner_argr->cm.sv.arset) ) {

						CM_SV_ARSET_PTR_TYPE arset = owner_argr->cm.sv.arset;

						{
						LSA_UINT16 idx;

						for( idx = 0; idx < CM_SV_ARSET_MAX_ARS; ++idx ) {

							CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[idx];

							if( cm_is_not_null(temp_argr) ) {

								cm_sv_ar_problem_indicator_eval(temp_argr); /* trigger: pdev-diag changed (AR-set) */
							}
						}
						}
					}
					else {

						cm_sv_ar_problem_indicator_eval(owner_argr); /* trigger: pdev-diag changed */
					}
				}
			}

			cm_sv_dev_led_info_eval(dev); /* trigger: pdev-diag changed */

			/***/

			if( dev->R1_enabled ) {

				//R1 VV: send only diff-messages (but: must change CM-PD to support this)
				cm_sv_r1b_diag_replace_pdev(dev); /* trigger: cm_sv_pdev_diag_changed() */
			}
		}
	}
}

/*===========================================================================*/
/*=== CM-PD READ/WRITE ======================================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_pdev_read_write(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_RECORD_PARAMS_CONST_PTR_TYPE req,
	CM_UPPER_MEM_U8_PTR_TYPE ndr_data, /* the clrpc-buffer! */
	LSA_UINT16 ar_nr
) {
	CM_CHANNEL_PTR_TYPE channel = dev->channel;
	LSA_UINT32 pnio_err;

	CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) is_read(%u) subslot(0x%x) index(0x%x) cls(0x%x)"
		, dev->device_nr, ar_nr
		, req->is_read, req->subslot_nr, req->index, req->cls
		);

	if( ! CM_SV_SUBSLOT_IS_PDEV(req->subslot_nr) ) {

		/* AP00707117 (wrong error code)
		 * all pdev-indices are just forwarded to the pdev.
		 * without this if() a request to a non-pdev-submodule results
		 * in a CM_ERR_NOT_ACCESSIBLE which will be mapped to "invalid subslot".
		 * this is ok for the CM-PD but wrong for the CM-SV.
		 */

		CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) pdev-index(0x%x) but subslot(0x%x) is not pdev-submodule"
			, dev->device_nr, ar_nr
			, req->index, req->subslot_nr
			);

		pnio_err = CM_PNIO_FAULTY_Record__InvalidIndex(0); /* see cm_pd_response_to_pnio_err(), case CM_ERR_PRM_INDEX */
	}
	else if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, req->subslot_nr) ) { /* local pdev-submodule */

		CM_UPPER_RQB_PTR_TYPE pd = cm_upper_alloc_rqb(channel); /* note: no pre-alloc because of DeviceAdd::write_multiple_parallelism */

		if( cm_is_null(pd) ) {

			/* refactor:
			 * - pre-alloc is not as easy as it may seem
			 * - caller must supply the RQB (implicit-AR, explicit-AR, iterators)
			 * - write-multiple and DeviceAdd::write_multiple_parallelism: pause after one, or have a list of RQBs
			 * - see cm_sv_pdev_write_failed() below
			 */

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

			cm_sv_pdev_request(channel, opcode, dev->device_nr, ar_nr, pd);

			pnio_err = CM_PNIO_ERR_NONE; /* pending */
		}

		/***/

		if( ! req->is_read && pnio_err != CM_PNIO_ERR_NONE ) {

			if( cm_is_not_null(req->argr) ) { /* always true */

				cm_sv_pdev_write_failed(req->argr, req->cls, req->slot_nr, req->subslot_nr, req->index, 0); /* see above: alloc failed */
			}
		}
	}
	else { /* remote pdev-submodule (only if R1_enabled) */

		if ( ! dev->R1_enabled ) {

			pnio_err = CM_PNIO_FAULTY_Record__AccessDenied(255); /* unreachable */
		}
		else if( dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_UNKNOWN || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN ) {

			pnio_err = CM_PNIO_FAULTY_Record__AccessDenied(255); /* R1 bridge unknown or broken */
		}
		else { //R1 marker (read/write remote PDEV)

			if( ! req->is_read && req->record_data_length > CM_RECORD_OFFSET + 0xFFFF ) {

				pnio_err = CM_PNIO_FAULTY_Record__WriteLengthError(11); /* unreachable */
			}
			else {

				pnio_err = cm_sv_r1b_pdev_A_read_write(dev, req, ndr_data, ar_nr); /* note: does enqueue and wait-for-established */
			}
		}
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== CM-PD READ-IMPLICIT DONE ==============================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_pdev_read_implicit_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	LSA_UINT32 pnio_err;

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u)", dev->device_nr
		);

	/***/

	if( dev->implicit_ar.iter.pdev.state != CM_PD_ITER_PDEV_IDLE ) { /* sic! using the CM-PD enumeration */

		pnio_err = cm_sv_record_read_pdev_next(dev, LSA_NULL/*implicit-ar*/, pd);
	}
	else if( CM_RQB_GET_RESPONSE(pd) != CM_OK ) {

		/* AP00352895: must differentiate, not just returning CM_PNIO_FAULTY_Record__ReadError(10) */

		pnio_err = cm_pd_response_to_pnio_err(CM_RQB_GET_RESPONSE(pd), LSA_TRUE);
	}
	else { /* success */

		LSA_UINT32 ndr_len = CM_RECORD_OFFSET + pd->args.pd.prm_rdwr->record_data_length;

		cm_sv_rpc_mark_as_record_succeeded(dev->implicit_ar.rpc, LSA_TRUE/*read*/, ndr_len);

		pnio_err = CM_PNIO_ERR_NONE; /* completed */
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== CM-PD READ DONE =======================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_pdev_read_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	LSA_UINT32 pnio_err;

	if( argr->cm.sv.rec.iter_ptr->pdev.state != CM_PD_ITER_PDEV_IDLE ) { /* sic! using the CM-PD enumeration */

		pnio_err = cm_sv_record_read_pdev_next(dev, argr, pd);
	}
	else if( CM_RQB_GET_RESPONSE(pd) != CM_OK ) {

		/* AP00352895: must differentiate, not just returning CM_PNIO_FAULTY_Record__ReadError(10) */

		pnio_err = cm_pd_response_to_pnio_err(CM_RQB_GET_RESPONSE(pd), LSA_TRUE);
	}
	else { /* success */

		/* note: report result even if cm_ar_com_is_down() in the meantime */

		LSA_UINT32 ndr_len = CM_RECORD_OFFSET + pd->args.pd.prm_rdwr->record_data_length;

		cm_sv_rpc_mark_as_record_succeeded(argr->cm.sv.curr_rpc_ind, LSA_TRUE/*read*/, ndr_len);

		pnio_err = CM_PNIO_ERR_NONE; /* completed */
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== CM-PD WRITE DONE ======================================================*/
/*===========================================================================*/

static LSA_UINT32
cm_sv_pdev_write_done(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_RQB_CONST_PTR_TYPE pd
) {
	LSA_UINT32 pnio_err;

	if( argr->cm.sv.rec.iter_ptr->pdev.state != CM_PD_ITER_PDEV_IDLE ) { /* sic! using the CM-PD enumeration */

		CM_FATAL(); /* a bug */
		return 0;
	}
	else if( CM_RQB_GET_RESPONSE(pd) != CM_OK ) {

		/* AP00352895: must differentiate, not just returning CM_PNIO_FAULTY_Record__WriteError(10) */

		pnio_err = cm_pd_response_to_pnio_err(CM_RQB_GET_RESPONSE(pd), LSA_FALSE);
	}
	else { /* success */

		/* note: report result even if cm_ar_com_is_down() in the meantime */

		pnio_err = CM_PNIO_ERR_NONE; /* ok */
	}

	/***/

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_READ);

		pnio_err = CM_PNIO_FAULTY_Record_ChangeErrorCodeToWrite(pnio_err);

		/* note: no cm_sv_pdev_write_failed() here, CM-PD already handled it */
	}

	/***/

	if( argr->cm.sv.rec.coc_offs != 0 ) {

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			/* note: all PRM-records are later discarded (see CM-PD, PRM-Diag) */

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

			return CM_PNIO_ERR_NONE; /* pending */
		}

		pd->args.pd.prm_rdwr->record_data = argr->cm.sv.rec.coc_data + CM_RECORD_OFFSET; /* see write-multiple */

		pnio_err = CM_PNIO_ERR_NONE;
	}

	/***/

	if( argr->cm.sv.rec.multi_offs == 0 ) { /* single write */

		if( pnio_err == CM_PNIO_ERR_NONE ) {

			cm_sv_rpc_mark_as_record_succeeded(argr->cm.sv.curr_rpc_ind, LSA_FALSE/*write*/, CM_RECORD_OFFSET);
		}
	}
	else { /* write-multiple */

		cm_sv_record_write_embedded_update(
			argr,
			pd->args.pd.prm_rdwr->record_data - CM_RECORD_HEADER_SIZE,
			pnio_err, 0, 0
			);

		/***/

		cm_sv_record_write_embedded_next(argr);

		pnio_err = CM_PNIO_ERR_NONE; /* handled */
	}

	/***/

	return pnio_err;
}

/*===========================================================================*/
/*=== PDEV-WRITE FAILED =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pdev_write_failed( /* AP00581494 */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 cls,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr, /* refactor: check if "ar_sub" is available in callers */
	LSA_UINT32 index,
	LSA_UINT32 offset
) {
	/* note: only for pdev-submodules - check in callers */

	if( (cls & CM_REC_IDX_NONPRM) == 0 ) { /* not non-prm (i.e., prm) */

		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

		cm_ar_graph_find_api_slot_subslot(argr, 0, slot_nr, subslot_nr, LSA_NULL, LSA_NULL, &ar_sub);

		if( cm_is_null(ar_sub) ) {

			CM_FATAL();
		}
		else {

			if( ar_sub->sv.err_flags == 0 ) { /* PNIO-Spec, Part 5: report first error */

				ar_sub->sv.err_flags  = CM_PD_PRM_END_WRITE;
				ar_sub->sv.err_index  = index;
				ar_sub->sv.err_offset = offset;
			}
		}
	}
}

/*===========================================================================*/
/*=== CM-PD RECORD DONE =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pdev_record_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE argr, /* LSA_NULL if implicit-ar */
	LSA_BOOL is_read,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	if( cm_is_null(argr) ) { /* implicit-ar */

		LSA_UINT32 pnio_err;

		CM_ASSERT(is_read == LSA_TRUE);

		pnio_err = cm_sv_pdev_read_implicit_done(dev, pd);

		if( pnio_err != CM_PNIO_ERR_NONE ) {

			cm_sv_rpc_mark_as_record_failed(dev->implicit_ar.rpc, LSA_TRUE/*read*/, pnio_err, 0, 0);
		}

		/***/

		if( cm_sv_rpc_check_implicit_ar(dev) ) {

			cm_sv_rpc_next_implicit_ar(dev);
		}

		/***/

		cm_upper_free_rqb(dev->channel, pd);
	}
	else { /* explicit-ar */

		if( cm_is_not_null(argr->cm.sv.curr_ar_pdevprm) ) { /* pdevprm-read/write */

			if(
				cm_is_null(argr->cm.sv.curr_rpc_ind) /* note: prm-end is the current rpc here */
				|| (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_PDEVPRM_RSP) == 0
			) {

				CM_FATAL();
			}
			else {

				CM_UPPER_RQB_PTR_TYPE upper = argr->cm.sv.curr_ar_pdevprm;
				argr->cm.sv.curr_ar_pdevprm = LSA_NULL; /* unlink */

				if( is_read ) {
					upper->args.sv.ar_pdevprm_rdwr->rec.record_data_length = pd->args.pd.prm_rdwr->record_data_length;
				}

				/* note: rec.cm_pnio_err, rec.ret_val_1, rec.ret_val_2 are not used by pdevprm-read/write */

				cm_callback(argr->ar_com.channel, CM_RQB_GET_RESPONSE(pd), &upper);

				CM_RQB_SET_OPCODE(pd, 0/*unused*/);

				/***/

				if( cm_ar_com_is_down(argr) ) {

					cm_sv_ar_disconnect_indication_join(argr); /* trigger: curr_ar_pdevprm == LSA_NULL (but: curr_rpc_ind != LSA_NULL here (prm-end)) */
				}
			}
		}
		else { /* remote read or write */

			LSA_UINT32 pnio_err;

			if( is_read ) {

				pnio_err = cm_sv_pdev_read_done(dev, argr, pd);
			}
			else { /* write */

				pnio_err = cm_sv_pdev_write_done(argr, pd);

				if( pnio_err != CM_PNIO_ERR_NONE ) {

					CM_ASSERT(CM_PNIO_ERR_FETCH_CODE(pnio_err) == CM_PNIO_ERR_CODE_WRITE);
				}
			}

			/***/

			if( pnio_err != CM_PNIO_ERR_NONE ) {

				cm_sv_rpc_mark_as_record_failed(argr->cm.sv.curr_rpc_ind, is_read, pnio_err, 0, 0);
			}

			/***/

			cm_sv_rpc_check_explicit_ar(argr);

			/***/

			cm_upper_free_rqb(dev->channel, pd);
		}
	}
}

/*===========================================================================*/
/*=== CM-PD REQUEST =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pdev_request(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_CHANNEL_PTR_TYPE pd_channel;

	CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) opc(%u)"
		, device_nr, ar_nr
		, opcode
		);

	pd_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_PD);

	if( cm_is_null(pd_channel) ) {
		CM_FATAL(); /* a bug */
	}

	CM_EDD_SET_USER_ID_HANDLE_DEV_AR (rb, CM_INVALID_HANDLE, device_nr, ar_nr); /* note: there is no CM_RQB_SET_USER_ID_HANDLE_DEV_AR */

	CM_RQB_SET_HANDLE(rb, pd_channel->my_handle);

	CM_RQB_SET_OPCODE(rb, opcode);

	CM_REQUEST_LOCAL(rb);
}

/*===========================================================================*/
/*=== CM-PD REQUEST DONE ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pdev_request_done(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	LSA_UINT16 device_nr = CM_EDD_GET_USER_ID_DEV(rb);
	LSA_UINT16 ar_nr     = CM_EDD_GET_USER_ID_AR(rb);

	CM_SV_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) opc(%u) rsp(%u)"
		, device_nr, ar_nr
		, CM_RQB_GET_OPCODE(rb), CM_RQB_GET_RESPONSE(rb)
		);

	if( cm_is_null(channel) || channel->state != CM_SV_CREATED ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

		if( cm_is_null(dev) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			if( ar_nr == 0 ) { /* implicit-ar */

				if( CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PRM_READ_SERVER ) {

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("warning: switch impl/expl AR differently. implementation of 'done' is very different too")
#endif

					cm_sv_pdev_record_done(dev, LSA_NULL, LSA_TRUE/*read*/, rb);
				}
				else {

					CM_FATAL1(CM_RQB_GET_OPCODE(rb)); /* a bug */
				}
			}
			else if( ar_nr == CM_SV_R1B_PDEV_B_AR_NR ) {

				cm_sv_r1b_pdev_B_done(dev, rb);
			}
			else { /* explicit-ar */

				CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

				if( cm_is_null(argr) ) {

					CM_FATAL(); /* a bug */
				}
				else {

					switch( CM_RQB_GET_OPCODE(rb) ) {

					case CM_OPC_PD_PRM_BEGIN_SERVER:
						cm_sv_pd_prm_begin_done(argr, rb);
						break;

					case CM_OPC_PD_PRM_READ_SERVER:
						cm_sv_pdev_record_done(dev, argr, LSA_TRUE/*read*/, rb);
						break;

					case CM_OPC_PD_PRM_WRITE_SERVER:
						cm_sv_pdev_record_done(dev, argr, LSA_FALSE/*write*/, rb);
						break;

					case CM_OPC_PD_PRM_END_SERVER:
						cm_sv_pd_prm_end_confirmation(argr, rb);
						break;

					case CM_OPC_PD_PRM_RELINQUISH_SERVER:
						cm_sv_pd_prm_relinquish_done(argr, rb);
						break;

					default:
						CM_FATAL1(CM_RQB_GET_OPCODE(rb)); /* a bug */
						return;
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== CM-PD CALLBACK ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_pdev_callback(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_UINT16 rsp
) {
	CM_CHANNEL_PTR_TYPE pd_channel;

	CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rsp(%u)"
		, rsp
		);

	pd_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_PD);

	if( cm_is_null(pd_channel) ) {
		CM_FATAL(); /* a bug */
	}

	CM_RQB_SET_RESPONSE(rb, rsp);

	CM_RQB_SET_HANDLE(rb, pd_channel->my_handle);

	CM_REQUEST_LOCAL(rb);
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
