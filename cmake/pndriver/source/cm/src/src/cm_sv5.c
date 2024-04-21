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
/*  F i l e               &F: cm_sv5.c                                  :F&  */
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
/*  - build ar-graph from connect-parameters                                 */
/*  - build connect-response from ar-graph                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7500
#define CM_MODULE_ID       7500

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/


/*===========================================================================*/
/*=== BUILD ARVENDOR INFO (UNMARSHAL) =======================================*/
/*===========================================================================*/

#if 0 /* AP01556507: disable ARVendorBlock (V5.2i4) */

static LSA_UINT16
cm_sv_build_arvendor_info( /* AP01354956 */
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr
) {
	LSA_UINT16 block_field;

	/* note: CM_AR_PROP_STARTUP_MODE_ADVANCED is checked in caller */

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	if( CM_REQ_ARVENDOR_SIZE > block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/blocktype mismatch"
			);
		return 1; /* block_length wrong */
	}

	if( (block_size & 3) != 0 ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length mod 4 != 0"
			);
		return 1; /* block_length wrong */
	}

	/***/

	block_field = cm_ar_graph_insert_arvendor(argr
		, CM_GET32_NTOH(data_ptr, CM_REQ_ARVENDOR_API)
		, CM_GET16_NTOH(data_ptr, CM_REQ_ARVENDOR_APStructureIdentifier)
		, CM_CAST_U16(block_size), data_ptr /* the whole block! */
		, CM_SV_ARVENDORBLOCK_RES_MAXSIZE
		);

	if( block_field != 0 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "could not cm_ar_graph_insert_arvendor, field(%u)"
			, block_field
			);
		return block_field; /* invalid ARVendorBlockReq */
	}

	/***/

	return 0; /* ok */
}

#endif

/*===========================================================================*/
/*=== BUILD SR-INFO (UNMARSHAL) =============================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_build_sr_info(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr
) {
	LSA_UINT16 block_field;

	/* note: CM_AR_TYPE_SINGLE_SYSRED (and CM_AR_PROP_STARTUP_MODE_ADVANCED) is checked in caller */

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	if( CM_REQ_SRINFO_SIZE != block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/blocktype mismatch"
			);
		return 1; /* block_length wrong */
	}

	/***/

	block_field = cm_ar_graph_insert_sr_info(argr
		, CM_GET16_NTOH(data_ptr, CM_REQ_SRINFO_RedundancyDataHoldFactor)
		, CM_GET32_NTOH(data_ptr, CM_REQ_SRINFO_SRProperties)
		, CM_CAST_U16(block_size)
		);

	if( block_field != 0 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "could not cm_ar_graph_insert_sr_info, field(%u)"
			, block_field
			);
		return block_field; /* invalid sr-info */
	}

	/***/

	if( cm_is_not_null (argr->sr_info)) {

		CM_MEMCPY(argr->sr_info + 1, data_ptr, block_size); /* see allocation in cm_ar_graph_insert_sr_info() */
	}

	/***/

	return 0; /* ok */
}

/*===========================================================================*/
/*=== BUILD IR-INFO (UNMARSHAL) =============================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_build_ir_info(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr
) {
	CM_COMMON_MEM_U8_PTR_TYPE block_ptr = data_ptr; /* see memcpy below */
	LSA_UINT32 number_of_iocrs;
	CLRPC_UUID_TYPE ir_data_uuid;
	LSA_UINT16 block_field;

	/* note: "IRInfoBlock already seen" is checked in caller */
	/* note: CM_AR_PROP_STARTUP_MODE_ADVANCED is checked in caller */

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	if( CM_REQ_IRINFO_SIZE > block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/blocktype mismatch"
			);
		return 1; /* block_length wrong */
	}

	/* padding1 ... not checked */

	CM_GET_UUID_NTOH(&ir_data_uuid, data_ptr, CM_REQ_IRINFO_ir_data_uuid);

	/* padding2 ... not checked */

	number_of_iocrs = CM_GET16_NTOH(data_ptr, CM_REQ_IRINFO_number_of_iocrs);

	data_ptr += CM_REQ_IRINFO_SIZE;

	/***/

	if( CM_REQ_IRINFO_SIZE + number_of_iocrs * CM_REQ_IRINFOSUB_SIZE != block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/blocktype mismatch"
			);
		return 1; /* block_length wrong */
	}

	if (!(number_of_iocrs == 0 || number_of_iocrs == 2))
	{
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "NumberOfIOCRs not 0 or 2"
			);
		return 7;
	}

	block_field = cm_ar_graph_insert_ir_info(argr, &ir_data_uuid, CM_CAST_U16(block_size));

	if (block_field != 0)
	{
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cm_ar_graph_insert_ir_info returned %d", block_field
			);
		return block_field;
	}

	/***/

	if( cm_is_not_null(argr->ir_info) ) /* always true */
	{
		LSA_UINT32 iocr_counter;

		for (iocr_counter = 0; iocr_counter < number_of_iocrs; iocr_counter++)
		{
			block_field = cm_ar_graph_insert_ir_info_sub (argr
				, CM_GET16_NTOH(data_ptr, CM_REQ_IRINFOSUB_iocr_reference)
				, CM_GET16_NTOH(data_ptr, CM_REQ_IRINFOSUB_subframe_offset)
				, CM_GET32_NTOH(data_ptr, CM_REQ_IRINFOSUB_subframe_data)
				);

			if (block_field != 0)
			{
				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cm_ar_graph_insert_ir_info_sub returned %d", block_field
					);
				return block_field;
			}

			data_ptr += CM_REQ_IRINFOSUB_SIZE;
		}

		/***/

		CM_MEMCPY(argr->ir_info + 1, block_ptr, block_size); /* see allocation in cm_ar_graph_insert_ir_info() */
	}

	/***/

	return 0; /* ok */
}

/*===========================================================================*/
/*=== BUILD EXPECTED-SUBMODULE (UNMARSHAL) ==================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_build_expsubmod(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE part1
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	LSA_UINT32 size = 0;

	/***/

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(part1, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	{
	LSA_UINT16 api_cnt;
	CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

	size += CM_REQ_API_SIZE;

	if( size > block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/blocktype mismatch"
			);
		return 1; /* block_length too short */
	}

	/***/

	api_cnt = CM_GET16_NTOH(part1, CM_REQ_API_number_of_related_apis);

	if( api_cnt == 0 ) { /* must test here, not in cm_ar_graph_check_consistency() */
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "no related apis"
			);
		return 4; /* no related apis (but expsubmod-block) */
	}

	ar_mod = LSA_NULL; /* see cm_ar_graph_insert_ap_module() below */

	for( ; api_cnt != 0; --api_cnt ) {

		CM_COMMON_MEM_U8_PTR_TYPE part;
		LSA_UINT16 sub_cnt;
		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
		LSA_UINT16 block_field;

		part = part1 + size; /* part 2 */

		size += CM_REQ_MOD_SIZE;

		if( size > block_size ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "length/blocktype mismatch"
				);
			return 1; /* number_of_related_apis too big / block_length too short */
		}

		/***/

		sub_cnt = CM_GET16_NTOH(part, CM_REQ_MOD_number_of_submodules);

		if( sub_cnt == 0 ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "no submodules"
				);
			return 9; /* no submodules (but expsubmod-block) */
		}

		if( sub_cnt > (dev->max_nr_of_subslots - argr->cm.sv.nr_of_submodules) ) {
			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "too many submodules, max(%u)", dev->max_nr_of_subslots
				);
			return 9; /* too many submodules (tell something wrong with field number_of_submodules) */
		}

		/***/

		block_field = cm_ar_graph_insert_ap_module(
			argr,
			CM_GET32_NTOH(part, CM_REQ_MOD_api),
			CM_GET16_NTOH(part, CM_REQ_MOD_slot_nr),
			CM_GET32_NTOH(part, CM_REQ_MOD_mod_ident),
			CM_GET16_NTOH(part, CM_REQ_MOD_mod_properties),
			cm_is_null(ar_mod) ? LSA_NULL : &ar_mod->slot_nr,
			&ar_ap,
			&ar_mod
			);

		if( block_field != 0 ) {
			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cannot cm_ar_graph_insert_ap, field(%u)"
				, block_field
				);
			return block_field; /* invalid ap/mod pair */
		}

		if( cm_is_null(ar_ap) || cm_is_null(ar_mod) ) {
			CM_FATAL();
		}

		/***/

		if( ! cm_sv_slot_range_ok(dev, ar_mod->slot_nr) ) {
			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "slot(%u) out of range"
				, ar_mod->slot_nr
				);
			return 6; /* slot out of range (local) */
		}

		/***/

		for( ; sub_cnt != 0; --sub_cnt ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			part = part1 + size; /* part 3 */

			size += CM_REQ_SUB_SIZE;

			if( size > block_size ) {
				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "length/blocktype mismatch"
					);
				return 1; /* number_of_submodules too big / block_length too short */
			}

			/***/

			block_field = cm_ar_graph_insert_submodule(
				argr,
				ar_mod,
				CM_GET16_NTOH(part, CM_REQ_SUB_subslot_nr),
				CM_GET32_NTOH(part, CM_REQ_SUB_sub_ident),
				CM_GET16_NTOH(part, CM_REQ_SUB_sub_properties),
				argr->ar_com.channel->usr.sv.IOXS_Required,
				&ar_sub
				);

			if( block_field != 0 ) {
				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cannot cm_ar_graph_insert_submodule, field(%u)"
					, block_field
					);
				return block_field; /* invalid submodule */
			}

			if( cm_is_null(ar_sub) ) {
				CM_FATAL();
			}

			/***/

			if( ! cm_sv_subslot_range_ok(dev, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr) ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "subslot(%u) out of range"
					, ar_sub->subslot_nr
					);
				return 10; /* subslot out of range (local) */
			}

			/***/

			argr->cm.sv.nr_of_submodules += 1;

			if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, ar_sub->subslot_nr) ) { /* note: implies CM_SV_DEV_IS_PDEV_DEVICE() and cm_pdsv_mapping_ok() */

				argr->cm.sv.nr_of_pdev_submodules += 1; /* another pdev-submodule */

				if( CM_SV_SUBSLOT_IS_PDEV_INTERFACE(ar_sub->subslot_nr) ) {

					argr->cm.sv.pd_flags |= CM_SV_AR_PDFL_LOCAL_PDEV_INTERFACE;

					argr->cm.sv.local_pdev_if = ar_sub;
				}
			}

			if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(argr->cm.sv.dev, ar_sub->subslot_nr) ) { /* note: cm_sv_subslot_range_ok() if R1_enabled */

				if( argr->cm.sv.r1_flags == 0 ) { /* only R1-ARs have a remote PDEV */

					/* case: right head got name of left head; then single-sided S1-AR comes (TIA 1778174) */

					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "subslot(%u) out of range (non-R1-AR with pdev-submodule of remote PDEV)"
						, ar_sub->subslot_nr
						);
					return 10; /* subslot_nr out of range (R1 device) */
				}

				if( argr->cm.sv.nr_of_remote_pdev_submodules >= 64 ) { /* see "remote_prm_end_args" */

					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "subslot(%u) out of range (R1, more than 64 pdev-submodules)"
						, ar_sub->subslot_nr
						);
					return 10; /* subslot_nr out of range (R1) */
				}

				argr->cm.sv.nr_of_remote_pdev_submodules += 1; /* another remote pdev-submodule */

				if( CM_SV_SUBSLOT_IS_PDEV_INTERFACE(ar_sub->subslot_nr) ) {

					argr->cm.sv.pd_flags |= CM_SV_AR_PDFL_REMOTE_PDEV_INTERFACE;

					argr->cm.sv.remote_pdev_if = ar_sub;
				}
			}

			/***/

			{
			LSA_UINT16 dsc_cnt;

			dsc_cnt = 1; /* all have at least 1 data-descriptions */

			if( (ar_sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) == CM_SUB_PROP_TYPE_INPUT_OUTPUT ) {
				dsc_cnt += 1; /* in-out has 2 data-descriptions */
			}

			for( ; dsc_cnt != 0; --dsc_cnt ) {

				part = part1 + size; /* part 4 */

				size += CM_REQ_DSC_SIZE;

				if( size > block_size ) {
					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "length/blocktype mismatch"
						);
					return 1; /* sub_properties wrong / block_length too short */
				}

				block_field = cm_ar_graph_set_data_descr(
									ar_sub,
									2/*use-description*/,
									CM_GET16_NTOH(part, CM_REQ_DSC_data_description),
									CM_GET16_NTOH(part, CM_REQ_DSC_data_length),
									CM_GET8_NTOH(part,  CM_REQ_DSC_iops_length),
									CM_GET8_NTOH(part,  CM_REQ_DSC_iocs_length)
									);

				if( block_field != 0 ) {
					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "cannot set description"
						);
					return block_field; /* invalid description */
				}
			}
			}
		}
	}
	}

	if( size != block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "parse incomplete"
			);
		return 1; /* block_length too long */
	}

	/***/

	return 0; /* ok */
}

/*===========================================================================*/
/*=== BUILD ALRAM-CR (UNMARSHAL) ============================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_build_alcr(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr
) {
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE ar_alcr;
	LSA_UINT16 block_field;

	/***/

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	if( CM_REQ_ALCR_SIZE != block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/blocktype mismatch"
			);
		return 1; /* block_length wrong */
	}

	/***/

	block_field = cm_ar_graph_insert_alarm_cr(
		argr,

		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_alcr_type),
		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_ethertype),
		CM_GET32_NTOH(data_ptr, CM_REQ_ALCR_alcr_properties),

		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_timeout100ms),
		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_retries),

		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_local_alarm_ref),

		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_max_alarm_data_length),

		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_alcr_tag_header_high),
		CM_GET16_NTOH(data_ptr, CM_REQ_ALCR_alcr_tag_header_low),

		&ar_alcr
		);

	if( block_field != 0 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "could not cm_ar_graph_insert_alarm_cr, field(%u)"
			, block_field
			);
		return block_field; /* invalid AlarmCR data */
	}

	CM_ASSERT(cm_is_not_null(ar_alcr));

	/***/

	if( cm_is_not_null(argr->alarm_cr) ) { /* always true */

		if( argr->alarm_cr->max_alarm_data_len < CM_ALARM_OFFSET_DATA + argr->cm.sv.dev->alarm_send_max_length ) {

			return 10; /* max_alarm_data_length too small (AP01232054) */
		}
	}

	/***/

	return 0; /* ok */
}

/*===========================================================================*/
/*=== BUILD IO-CR (UNMARSHAL) ===============================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_build_iocr(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE part1 /* block_data */
) {
	LSA_UINT32 size;
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;
	LSA_UINT16 block_field;

	/***/

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(part1, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	size = CM_REQ_IOCR1_SIZE;

	if( size > block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/blocktype mismatch"
			);
		return 1; /* block_length too short */
	}

	/***/

	block_field = cm_ar_graph_insert_io_cr(
		argr,

		CM_GET16_NTOH(part1, CM_REQ_IOCR1_iocr_type),
		CM_GET16_NTOH(part1, CM_REQ_IOCR1_iocr_ref),
		CM_GET16_NTOH(part1, CM_REQ_IOCR1_ethertype),
		CM_GET32_NTOH(part1, CM_REQ_IOCR1_iocr_properties),

		CM_GET16_NTOH(part1, CM_REQ_IOCR1_data_length),
		CM_GET16_NTOH(part1, CM_REQ_IOCR1_frame_id),

		CM_GET16_NTOH(part1, CM_REQ_IOCR1_send_clock), /* note: is checked against normative range; local range see below */
		CM_GET16_NTOH(part1, CM_REQ_IOCR1_reduction_ratio),
		CM_GET16_NTOH(part1, CM_REQ_IOCR1_phase),
		/* ignoring CM_REQ_IOCR1::sequence, search for AP00619993 */
		CM_GET32_NTOH(part1, CM_REQ_IOCR1_frame_send_offset),

		CM_GET16_NTOH(part1, CM_REQ_IOCR1_watchdog_factor_legacy), /* legacy but must check (AP01304706) */
		CM_GET16_NTOH(part1, CM_REQ_IOCR1_data_hold_factor),

		CM_GET16_NTOH(part1, CM_REQ_IOCR1_iocr_tag_header),

		part1 + CM_REQ_IOCR1_iocr_multicast_mac, /* copy as is */

		&ar_iocr
		);

	if( block_field != 0 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "could not cm_ar_graph_insert_io_cr, field(%u)"
			, block_field
			);
		return block_field; /* invalid IO CR data */
	}

	if( ! cm_edd_send_clock_range_ok_hw(argr->ar_com.channel, argr->ar_com.ar_send_clock) ) {

		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "send-clock(%u) out of local range"
			, argr->ar_com.ar_send_clock
			);
		return 10; /* unsupported send-clock */
	}

	CM_ASSERT(cm_is_not_null(ar_iocr));

	/***/

	{
	LSA_UINT16 api_cnt;

	api_cnt = CM_GET16_NTOH(part1, CM_REQ_IOCR1_number_of_related_apis);

	/* api_cnt == 0 checked in cm_ar_graph_check_consistency() */

	/* could optimize here and check for <= CM_PNIO_C_SDU_MAX_LENGTH */

	for( ; api_cnt != 0; --api_cnt ) {

		CM_COMMON_MEM_U8_PTR_TYPE part;
		CM_AR_GRAPH_IOAPI_PTR_TYPE  ar_ioapi;

		part = part1 + size; /* part 2 */

		size += CM_REQ_IOCR2_SIZE;

		if( size > block_size ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "length inconsistency"
				);
			return 1; /* number_of_related_apis too big / block_length too short */
		}

		block_field = cm_argr_insert_ioap(
						ar_iocr,
						CM_GET32_NTOH(part, CM_REQ_IOCR2_api),
						&ar_ioapi
						);

		if( block_field != 0 ) {
			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "could not cm_ar_graph_insert_ioap, field(%u)"
				, block_field
				);
			return block_field; /* invalid IOAPI data */
		}

		CM_ASSERT(cm_is_not_null(ar_ioapi));

		/***/

		{
		LSA_UINT16 rel_cnt;

		rel_cnt = CM_GET16_NTOH(part, CM_REQ_IOCR2_number_of_related_iodata_objects);

		/* rel_cnt == 0 checked in cm_ar_graph_check_consistency() */

		/* could optimize here and check for <= CM_PNIO_C_SDU_MAX_LENGTH */

		for( ; rel_cnt != 0; --rel_cnt ) {

			CM_UPPER_ARCB_IODATA_OBJECT_PTR_TYPE ar_iodata;

			part = part1 + size; /* part 3 */

			size += CM_REQ_IOCR3_SIZE;

			if( size > block_size ) {
				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "length inconsistency"
					);
				return 1; /* number_of_related_iodata_objects too big / block_length too short */
			}

			block_field = cm_argr_insert_iodata_objects(
				ar_ioapi,
				CM_GET16_NTOH(part, CM_REQ_IOCR3_slot_nr),
				CM_GET16_NTOH(part, CM_REQ_IOCR3_subslot_nr),
				CM_GET16_NTOH(part, CM_REQ_IOCR3_frame_offset),
				&ar_iodata
			);

			if( block_field != 0 ) {
				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "could not cm_argr_insert_iodata_objects, field(%u)"
					, block_field
					);
				return block_field; /* invalid IODATA */
			}

			CM_ASSERT(cm_is_not_null(ar_iodata));
		}
		}

		/***/

		part = part1 + size; /* part 4 */

		size += CM_REQ_IOCR4_SIZE;

		if( size > block_size ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "length inconsistency"
				);
			return 1; /* block_length too short */
		}

		{
		LSA_UINT16 rel_cnt;

		rel_cnt = CM_GET16_NTOH(part, CM_REQ_IOCR4_number_of_related_iocs);

		/* rel_cnt == 0 checked in cm_ar_graph_check_consistency() */

		/* could optimize here and check for <= CM_PNIO_C_SDU_MAX_LENGTH */

		for( ; rel_cnt != 0; --rel_cnt ) {

			CM_UPPER_ARCB_IOCS_PTR_TYPE ar_iocs;

			part = part1 + size; /* part 5 */

			size += CM_REQ_IOCR5_SIZE;

			if( size > block_size ) {
				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "length inconsistency"
					);
				return 1; /* number_of_related_iocs too big / block_length too short */
			}

			block_field = cm_argr_insert_iocs(
				ar_ioapi,
				CM_GET16_NTOH(part, CM_REQ_IOCR5_slot_nr),
				CM_GET16_NTOH(part, CM_REQ_IOCR5_subslot_nr),
				CM_GET16_NTOH(part, CM_REQ_IOCR5_frame_offset),
				&ar_iocs
			);

			if( block_field != 0 ) {
				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "could not cm_argr_insert_iocs, field(%u)"
					, block_field
					);
				return block_field; /* invalid IOCS */
			}

			CM_ASSERT(cm_is_not_null(ar_iocs));
		}
		}
	}
	}

	/***/

	if( size != block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "parse incomplete"
			);
		return 1; /* block_length too long */
	}

	/***/

	return 0; /* ok */
}

/*===========================================================================*/
/*=== BUILD MULTICAST-CR (UNMARSHAL) ========================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_build_mcr(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE block_ptr
) {
	LSA_UINT16 name_length;
	CM_COMMON_MEM_U8_PTR_TYPE name_ptr;
	CM_AR_GRAPH_MCR_PTR_TYPE mcr;
	LSA_UINT16 block_field;

	/***/

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(block_ptr, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	if( CM_REQ_MCR_SIZE > block_size ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/block-type mismatch"
			);
		return 1; /* block_length too short */
	}

	name_length = CM_GET16_NTOH(block_ptr, CM_REQ_MCR_station_name_length);

	if( CM_REQ_MCR_SIZE + name_length > block_size ) {
		/* note: padding after name, see pdu! */
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "name_length/block_length inconsistency"
			);
		return 1; /* name_length too long / block_length too short */
	}

	name_ptr = block_ptr + CM_REQ_MCR_SIZE;

	/***/

	block_field = cm_ar_graph_insert_mcr(
		argr,
		CM_GET16_NTOH(block_ptr, CM_REQ_MCR_iocr_ref),
		CM_GET32_NTOH(block_ptr, CM_REQ_MCR_mcr_properties),
		CM_GET16_NTOH(block_ptr, CM_REQ_MCR_mci_timeout_factor),
		name_length,
		name_ptr,
		&mcr
		);

	if( block_field != 0 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "could not cm_ar_graph_insert_mcr, field(%u)"
			, block_field
			);
		return block_field; /* invalid MCR data */
	}

	CM_ASSERT(cm_is_not_null(mcr));

	/***/

	/*
	padding = not parsed
	 */

	/***/

	return 0; /* ok */
}

/*===========================================================================*/
/*=== BUILD IO-AR (UNMARSHAL) ===============================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_build_ioar(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 host_ip,
	LSA_UINT32 block_size,
	CM_COMMON_MEM_U8_PTR_TYPE block_ptr,
	LSA_UINT32 unparsed_length,
	LSA_BOOL * ar_is_CIR, /* true if CIR (TIA 906437) */
	LSA_UINT32 r1_data_length /* != 0 if representative-AR */
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	CM_SV_ARSET_PTR_TYPE arset = LSA_NULL;
	CLRPC_UUID_TYPE ar_uuid;
	CLRPC_UUID_TYPE cmi_obj_uuid;
	LSA_UINT16 cmi_name_length;
	CM_COMMON_MEM_U8_PTR_TYPE cmi_name_ptr;
	LSA_UINT16 block_field;

	CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u)"
		, dev->device_nr
		);

	/***/

	{
	LSA_UINT16 block_version = CM_GET16_NTOH(block_ptr, CM_BLK_HEADER_version);

	if( (block_version & 0xFF00) != 0x0100 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.high(0x%x) not supported"
			, block_version
			);
		return 2; /* wrong block_version.high */
	}

	if( (block_version & 0x00FF) != 0x0000 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "block_version.low(0x%x) not supported"
			, block_version
			);
		return 3; /* wrong block_version.low */
	}
	}

	/***/

	if( CM_REQ_AR_SIZE > block_size) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "length/block-type mismatch"
			);
		return 1; /* block_length too short */
	}

	/***/

	CM_GET_UUID_NTOH(&ar_uuid, block_ptr, CM_REQ_AR_ar_uuid);

	argr->ar.ar_uuid = ar_uuid; /* assign early, see logging in caller */

	argr->cm.sv.session_key_connect = CM_GET16_NTOH(block_ptr, CM_REQ_AR_session_key);

	CM_GET_UUID_NTOH(&cmi_obj_uuid, block_ptr, CM_REQ_AR_cmi_obj_uuid);

	cmi_name_length = CM_GET16_NTOH(block_ptr, CM_REQ_AR_cmi_name_length);

	cmi_name_ptr = block_ptr + CM_REQ_AR_SIZE; /* follows cmi_name_length */

	if( CM_REQ_AR_SIZE + cmi_name_length != block_size) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cmi_name_length/block_length inconsistency"
			);
		return 12; /* cmi_name_length/block_length inconsistent */
	}

	/***/

	block_field = cm_ar_graph_insert_io_ar(
		argr,
		host_ip,
		CM_GET16_NTOH(block_ptr, CM_REQ_AR_ar_type),
		&ar_uuid, /* native */
		block_ptr + CM_REQ_AR_cmi_mac, /* as is */
		&cmi_obj_uuid, /* native */
		CM_GET32_NTOH(block_ptr, CM_REQ_AR_ar_properties),
		CM_GET16_NTOH(block_ptr, CM_REQ_AR_cmi_activity_timeout100ms),
		CM_HTON16(CM_GET16_NTOH(block_ptr, CM_REQ_AR_cmi_udp_port)), /* network byte order */
		cmi_name_length,
		cmi_name_ptr
		);

	if( block_field != 0 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "could not cm_ar_graph_insert_io_ar, field(%u)"
			, block_field
			);
		return block_field; /* invalid ar-block */
	}

	/***/

	if( (argr->ar.ar_properties & CM_AR_PROP_PRM_SERVER_CMINITIATOR) == 0 ) {
		CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_properties(0x%x) 'external prm-server' not supported"
			, argr->ar.ar_properties
			);
		return 9; /* 'external prm-server' not supported */
	}

	/*
	 * AP01034486:
	 *   if IOSAR but max_ars_IOS == 0 then must return "out of ar-resources"
	 *
	 * in general:
	 *   if the ARType is known but disabled/unsupported due to a configuration setting
	 *   then "out of ar-resources" rather than "ar-type not supported" is responded
	 *
	 * see cm_sv_ar_alloc()
	 */

	if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

		/* IODConnectReq ::= ARBlockReq (the first and only block) */

		if( unparsed_length != block_size ) { /* more blocks follow */

			return 252; /* tell "unknown blocks" */
		}

		/* return "out of ar-resources" if cm_sv_ar_alloc() below fails */
	}
	else { /* ARs with IOCRs */

		if( ! (
			argr->ar.ar_type == CM_AR_TYPE_SINGLE
			|| argr->ar.ar_type == CM_AR_TYPE_SUPERVISOR
			|| argr->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3 /* see cm_ar_graph_insert_io_ar() too */
			|| argr->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED /* see cm_ar_graph_insert_io_ar() too */
		) ) {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "ar-type(%u) not supported (unknown)"
				, argr->ar.ar_type
				);

			return 4; /* ar-type not supported (unknown) */
		}

		if( dev->SRD_enabled ) { /* S2 or R1 device */

			if( dev->R1_enabled ) { /* R1 */

				if( argr->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3 ) {

					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "ar-type(%u) RTC3 not supported for R1"
						, argr->ar.ar_type
						);

					return 253; /* out of ar-resources (RTC3 not supported for R1) */
				}

				/* Shared AR must be possible for R1, TIA 1587591 */
			}
			else { /* S2 */

				/* RTC3 and Shared AR must be possible for S2, TIA 1479398 */
				/* see logic in cm_sv_ar_alloc() */
			}
		}

		/* return "out of ar-resources" if cm_sv_ar_alloc() below fails */
	}

	/*
	 * check for rerun only if params look good
	 * (don't allow a malformed AR to abort an existing)
	 */

	{
	CM_AR_GRAPH_PTR_TYPE existing_argr = cm_sv_ar_lookup_by_aruuid(dev, &argr->ar.ar_uuid);

	if( cm_is_not_null(existing_argr) ) {

		if( (existing_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) { //R1 marker (rerun)

			/* note: the representative-AR is only killed by the original-AR or "bridge-broken" */

			CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "connect with ARUUID of the representative-AR. existing ar(%u) will NOT be aborted, cn_flags(0x%08x) ar_flags(0x%08x) r1_flags(0x%08x)"
				, existing_argr->ar_nr
				, existing_argr->cm.sv.cn_flags, existing_argr->cm.sv.ar_flags, existing_argr->cm.sv.r1_flags
				);

			return 254; /* a kind of rerun */
		}

		/***/

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "rerun or IOC power-off-on. existing ar(%u) will be aborted, cn_flags(0x%08x) ar_flags(0x%08x) r1_flags(0x%08x)"
			, existing_argr->ar_nr
			, existing_argr->cm.sv.cn_flags, existing_argr->cm.sv.ar_flags, existing_argr->cm.sv.r1_flags
			);

		cm_ar_com_down_debug_data_fmt(existing_argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_RERUN, 0);
		cm_ar_com_down(existing_argr, CM_AR_REASON_RERUN);

		/*
		 * note: could queue new, abort old, dequeue...
		 *       doing this would add complexity (so we do it this way until someone complains)
		 */

		return 254; /* rerun */
	}
	}

	/***/

	if( argr->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED ) {

		if( ! dev->SRD_enabled ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "IOCARSR but SRD not enabled (no resources for an AR-set)"
				);

			return 253; /* out of ar-resources (no resources for an AR-set) */
		}

		if( (argr->ar.ar_properties & CM_AR_PROP_SUPERVISOR_TAKEOVER_ALLOWED) != 0 ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "IOCARSR, supervisor-takeover not supported"
				);

			return 9; /* supervisor-takeover not supported */
		}

		if( dev->R1_enabled && (argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) == 0 ) { //R1 marker (COC)

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "R1-AR but not COC, ar-properties(0x%08x)"
				, argr->ar.ar_properties
				);

			return 9; /* ar-properties wrong, COC is mandatory for R1 */
		}

		/* ARUUID::Selector: bits 0-2 (ARnumber) = SRARNR, bits 3-4 (ARresource) = "two ARs", bits 5-15 = reserved */

		{
		LSA_UINT8 srarnr = CM_SV_ARUUID_TO_SRARNR(argr->ar.ar_uuid);
		LSA_BOOL left_or_above;

		if(
			(argr->ar.ar_uuid.node[5] & 0x07) != srarnr /* always false (sanity check for the macro) */
			|| (argr->ar.ar_uuid.node[5] & 0xF8) != 0x10
			|| argr->ar.ar_uuid.node[4] != 0
		) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "IOCARSR, ARUUID::Selector out of range"
				);

			return 5; /* ARUUID::Selector out of range */
		}

		/***/

		if( dev->R1_enabled ) { /* TIA 1488359 (obsoletes TIA 906437) */

			if( (dev->val_8i00 & 0xF000) == 0x8000 ) {

				left_or_above = LSA_TRUE;
			}
			else {

				left_or_above = LSA_FALSE;
			}

			/***/

			if( r1_data_length != 0 ) { /* representative-AR */

				if( left_or_above ) {

					left_or_above = LSA_FALSE;
				}
				else {

					left_or_above = LSA_TRUE;
				}
			}
		}
		else { /* S2 */

			left_or_above = LSA_TRUE;
		}

		if( left_or_above ) {

			if( ! (srarnr == 1 || srarnr == 2) ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "IOCARSR, ARUUID::Selector::ARnumber(%u) out of range"
					, srarnr
					);

				return 5; /* ARUUID::Selector::ARnumber out of range */
			}
		}
		else { /* right or below */

			if( ! (srarnr == 3 || srarnr == 4) ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "IOCARSR, ARUUID::Selector::ARnumber out of range"
					);

				return 5; /* ARUUID::Selector::ARnumber out of range */
			}
		}
		}

		/***/

#if CM_CFG_MAX_SV_ARSETS_PER_DEVICE != 1
#error "must implement arset-allocator/selector"
#endif

#if CM_SV_ARSET_MAX_ARS != 2
#error "who changed that?"
#endif

		arset = CM_SV_DEV_GET_ARSET(dev);

		if( arset->is_aborting ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "AR-set is aborting"
				);

			return 251; /* the AR-set is aborting (last AR (or all ARs) of the AR-set in CM_SV_AR_FL_DOWN_PHASE) */
		}
		else if( cm_is_not_null(arset->arr[1]) ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "AR-set contains already two ARs"
				);

			return 253; /* out of ar-resources (already two ARs) */
		}
		else if( cm_is_not_null(arset->arr[0]) ) {

			CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[0];
			CLRPC_UUID_PTR_TYPE uuid1 = &temp_argr->ar.ar_uuid;
			CLRPC_UUID_PTR_TYPE uuid2 = &argr->ar.ar_uuid;

			if( ! (
				uuid1->time_low == uuid2->time_low
				&& uuid1->time_mid == uuid2->time_mid
			) ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "only one AR-set supported"
					);

				return 253; /* out of ar-resources (different AR-set but have only one) */
			}

			if( CM_SV_ARUUID_TO_SRARNR(*uuid1) == CM_SV_ARUUID_TO_SRARNR(*uuid2) ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
					, "AR-set, AR-number(%u) already exists"
					, CM_SV_ARUUID_TO_SRARNR(*uuid2)
					);

				return 253; /* out of ar-resources (same SR-AR number is not allowed) */
			}

			if(
				uuid1->time_hi_and_version == uuid2->time_hi_and_version
				&& uuid1->clock_seq_hi_and_reserved == uuid2->clock_seq_hi_and_reserved
				&& uuid1->clock_seq_low == uuid2->clock_seq_low
				&& uuid1->node[0] == uuid2->node[0]
				&& uuid1->node[1] == uuid2->node[1]
				&& uuid1->node[2] == uuid2->node[2]
				&& uuid1->node[3] == uuid2->node[3]
			) {

				CM_ASSERT(*ar_is_CIR == LSA_FALSE); /* system redundancy */
			}
			else {

				if( ! dev->CIR_enabled ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "CIR-AR but CIR not enabled"
						);

					return 253; /* out of ar-resources (CIR-AR but CIR not enabled) */
				}

				/* note for "R2": only one CIR-AR at a time (trivial for "S2") */

				*ar_is_CIR = LSA_TRUE;
			}

			/***/

			if( *ar_is_CIR ) {

				cm_sv_arset_check_primary(temp_argr);

				if( temp_argr->cm.sv.r1_flags != 0 ) { /* R1 */

					if( (temp_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

						if( temp_argr != arset->primary ) {

							CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "CIR-AR but no primary AR"
								);

							return 251; /* AR-set state conflict (CIR but no primary AR) */
						}
					}
					else {

						/* note: "primary AR exists" cannot be checked using the representative-AR */
					}
				}
				else { /* S2 */

					if( temp_argr != arset->primary ) {

						CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
							, "CIR-AR but no primary AR"
							);

						return 251; /* AR-set state conflict (CIR but no primary AR) */
					}
				}
			}

			/* if an AR of the same AR-set is before appl-ready.cnf or aborting do not accept this AR */

			if( temp_argr->cm.sv.r1_flags == 0 ) { /* S2 (R1 see below) */

				if( (temp_argr->cm.sv.ar_flags & (CM_SV_AR_FL_CONNECT_PHASE | CM_SV_AR_FL_DOWN_PHASE)) != 0 ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "AR-set, other AR is starting up or aborting"
						);

					return 251; /* AR-set state conflict (AR of same AR-set is starting up or aborting) */
				}
			}
		}
		else {

			/* ok (this AR creates a new AR-set) */
		}

		/***/

		if( argr->cm.sv.r1_flags != 0 ) { /* sanity (only R1-ARs have r1_flags != 0) */
			CM_FATAL();
		}

		if( dev->R1_enabled ) { //R1 marker

			CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[0];

			if( r1_data_length != 0 ) {

				if( cm_is_not_null(temp_argr) && (temp_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {

					return 253; /* two representative-ARs (bug on other side) */
				}

				argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_REPRESENTATIVE;
				argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_EX_DISCONN;
			}
			else {

				if( cm_is_not_null(temp_argr) && (temp_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

					return 253; /* two original SR-ARs - either R2 or S2 on one head */
				}

				argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_ORIGINAL;
			}

			/***/

			if( cm_is_not_null(temp_argr) ) { /* determination of first-AR */

				if( (temp_argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) != 0 ) {

					CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
						, "dev/ar(%u/%u) existing SR-AR sk(0x%x) is aborting -> reject connect"
						, argr->device_nr, argr->ar_nr/*0!*/, temp_argr->session_key
						);

					return 251; /* AR-set state conflict (existing AR (original or representative) is aborting) */
				}

				/***/

				if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) { /* called from cm_sv_r1b_handle_connect() */

					if( (temp_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) == 0 ) {
						CM_FATAL(); /* a bug */
					}

					/***/

					if( (temp_argr->cm.sv.cn_flags & (CM_SV_AR_CNFL_WF_SEND_CLOCK | CM_SV_AR_CNFL_WF_FRAMES_ADD)) != 0 ) {

						CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
							, "dev/ar(%u/%u) representative-AR connecting and original-AR is WF_SEND_CLOCK or WF_FRAMES_ADD -> aborting original-AR sk(0x%x)"
							, argr->device_nr, argr->ar_nr/*0!*/, temp_argr->session_key
							);

						/* note: abort ORIG because ORIG did not send a bridge-message yet. if rejecting REPR on the other side the only AR of the AR-set is leaving and thus aborting the AR-set... */

						cm_ar_com_down_debug_data_fmt(temp_argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_CONNECT/*!*/, 0);
						//R1 VV: make cm_ar_com_down3() for the next 2 lines. see cm_sv_rpc_connect_response_join().
						cm_ar_com_down(temp_argr, CM_AR_REASON_R1_CONN);
						temp_argr->ar_com.down_pnio_status = CM_PNIO_FAULTY_Connect__CMRPC_ARSetStateConflictDuringConnectionEstablishment();
					}
					else if( (temp_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_EX_CONN_DONE) != 0 ) {

						if( (dev->val_8i00 & 0xF000) == 0x9000) {

							CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "dev/ar(%u/%u) 0x9jpp, representative-AR connecting and original-AR is EX_CONN_DONE -> aborting original-AR sk(0x%x)"
								, argr->device_nr, argr->ar_nr/*0!*/, temp_argr->session_key
								);

							cm_ar_com_down_debug_data_fmt(temp_argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_CONNECT/*!*/, 0);
							//R1 VV: make cm_ar_com_down3() for the next 2 lines. see cm_sv_rpc_connect_response_join().
							cm_ar_com_down(temp_argr, CM_AR_REASON_R1_CONN);
							temp_argr->ar_com.down_pnio_status = CM_PNIO_FAULTY_Connect__CMRPC_ARSetStateConflictDuringConnectionEstablishment();
						}
						else {

							CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "dev/ar(%u/%u) 0x8ipp, representative-AR connecting and original-AR is EX_CONN_DONE -> rejecting representativ-AR"
								, argr->device_nr, argr->ar_nr/*0!*/
								);

							return 251; /* AR-set state conflict */
						}
					}
					else if( (temp_argr->cm.sv.ar_flags & CM_SV_AR_FL_CONNECT_PHASE) != 0 ) { /* i.e. before rpc-appl-ready.cnf */

						CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
							, "dev/ar(%u/%u) representative-AR connecting and original-AR sk(0x%x) is before rpc-appl-ready.cnf -> rejecting representative-AR"
							, argr->device_nr, argr->ar_nr/*0!*/, temp_argr->session_key
							);

						return 251; /* AR-set state conflict */
					}
					else {

						/* do nothing (original-AR is in data-phase) */
					}
				}
				else {

					/* do nothing (is original-AR) */
				}
			}
			else {

				/* do nothing (no other AR to check against) */
			}
		}

		/***/

		CM_ASSERT(argr->ar_com.user_apdu_status == CM_EVENT_APDU_STATUS_INITIAL_SYSRED_VALUE); /* see cm_ar_graph_insert_io_ar() */

		argr->ar_com.user_apdu_status |= CM_EVENT_APDU_STATUS_BACKUP_HELPER; /* initial value is "all backup" */
	}

	/***/

	CM_ASSERT(argr->ar_nr == 0); /* not yet assigned (is done in cm_sv_ar_alloc()) */

	if( ! cm_sv_ar_alloc(dev, arset, argr) ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "no free ar-resource"
			);

		return 253; /* out of ar-resources */
	}

	/***/

	return 0; /* ok */
}

/*===========================================================================*/
/*=== PREPARE IOCR SUBFRAME DATA ============================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_argr_prepare_iocr_subframe_data(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {

	if( cm_is_not_null(argr->ir_info) ) {

		CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE ir_info_sub;
		for( CmListForeach (ir_info_sub, &argr->ir_info->ir_info_sub_list, CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE) ) {

			/*
			 * iocr not found: not checked here.
			 * duplicate iocr_ref: not checked here.
			 * see cm_ar_graph_check_ir_info_block()
			 */

			CM_AR_GRAPH_IO_CR_PTR_TYPE iocr = cm_ar_graph_find_io_cr (argr, ir_info_sub->iocr_ref);

			if( cm_is_not_null(iocr) ) {

				iocr->cm.subframe_data   = (ir_info_sub->subframe_data & 0xFF7F);
				iocr->cm.subframe_offset = ir_info_sub->subframe_offset;
			}
		}
	}
}

/*===========================================================================*/
/*=== BUILD AR-GRAPH FROM CONNECT-REQUEST (UNMARSHAL) =======================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_argr_build(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc,
	CM_COMMON_MEM_U8_PTR_TYPE r1_data_ptr, /* used if rpc==NULL */
	LSA_UINT32 r1_data_length              /* used if rpc==NULL */
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_length;
	LSA_UINT32 host_ip;
	LSA_UINT32 alloc_len;
	LSA_UINT32 cres_length;
	LSA_BOOL ar_block_seen;
	LSA_BOOL ar_is_CIR;

	/***/

	if( cm_is_null(rpc) ) { //R1 marker (building the representative-AR)

		if( cm_is_null(r1_data_ptr) ) {
			CM_FATAL();
		}

		data_ptr    = r1_data_ptr;
		data_length = r1_data_length;

		host_ip   = 0; /* don't care */
		alloc_len = 0; /* see below */
	}
	else {

		data_ptr    = rpc->args.sv.call->ndr_data + CLRPC_PNIO_OFFSET; /* sanity-checked in caller */
		data_length = rpc->args.sv.call->ndr_len  - CLRPC_PNIO_OFFSET;

		host_ip   = rpc->args.sv.call->host_ip;
		alloc_len = rpc->args.sv.call->alloc_len;
	}

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) data_length(%u)"
		, dev->device_nr, data_length
		);

	/*
	 * parse the connect-parameters
	 *
	 * IODConnectReq ::=
	 *   ARBlockReq, <--------------------- first block
	 *   { <------------------------------- order of group is arbitrary
	 *   [IOCRBlockReq*], <---------------- none for easy-supervisor
	 *   [AlarmCRBlockReq], <-------------- none for easy-supervisor
	 *   [ExpectedSubmoduleBlockReq*], <--- none for easy-supervisor
	 *   [PrmServerBlock],
	 *   [MCRBlockReq*],
	 *   [ARRPCBlockReq],
	 *   [IRInfoBlock] <------------------- IOCARSingle using RT_CLASS_3 with StartupMode == Advanced (V5.0)
	 *   [SRInfoBlock] <------------------- IOCARSR (always with StartupMode == Advanced, V5.1)
	 *   [ARVendorBlockReq*] <------------- only with StartupMode == Advanced (V5.2i3)
	 *   [ARFSUBlock] <-------------------- only with StartupMode == Advanced (V5.0i5)
	 *   [RSInfoBlock] <------------------- only with StartupMode == Advanced (V6.1i1)
	 *   }
	 */

	cres_length = CLRPC_PNIO_OFFSET;

	ar_block_seen = LSA_FALSE;

	ar_is_CIR = LSA_FALSE;

	for(;;) {

		LSA_UINT16 block_field;
		LSA_UINT16 block_type;
		LSA_UINT16 block_length;
		LSA_UINT32 block_size;

		/* all blocks start with a block-header */

		block_field = 0; /* type */

		if( (2) > data_length ) {
			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "too few data (no type field)"
				);
			return CM_PNIO_FAULTY_Connect__CMRPC_ArgsLengthInvalid();
		}

		block_type = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_type);

		/***/

		block_field = 1; /* length */

		block_length  = 0; /* to avoid a warning */
		block_size = 0; /* to avoid a warning */

		if( (2+2) > data_length ) {

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "too few data (no length field)"
				);
		}
		else {

			block_length = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_length);
			block_size = block_length + CM_BLK_HEADER_TypLen_SIZE;

			if( block_size > data_length ) { /* AP01201900 (2+2 was missing) */

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "too few data (block larger)"
					);
			}
			else {

				block_field = 2; /* version (high) */

				if( (1) > block_length ) { /* AP01201900 (must check against block_length) */

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "too few data (no version.high field)"
						);
				}
				else {

					block_field = 3; /* version (low) */

					if( (1+1) > block_length ) { /* AP01201900 (must check against block_length) */

						CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "too few data (no version.low field)"
							);
					}
					else {

						block_field = 4; /* payload (after version) */
					}
				}
			}
		}

		CM_ASSERT(block_field != 0) /* see above */

		/* have at least "type" */

		/***/

		if( ! ar_block_seen ) {

			if( block_type != CM_BLOCK_TYPE_AR_REQ ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "first block is not the ar-block"
					);

				return CM_PNIO_FAULTY_ARBlockReq(0); /* first block is not the ar-block */
			}
		}

		CM_ASSERT(
			block_type == CM_BLOCK_TYPE_AR_REQ
			|| ar_block_seen
		);

		switch( block_type ) {

		case CM_BLOCK_TYPE_AR_REQ:

			if( ar_block_seen ) {

				CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "more than one ar-block"
					);

				return CM_PNIO_FAULTY_ARBlockReq(0); /* more than one ar-block */
			}

			ar_block_seen = LSA_TRUE;

			/***/

			if( block_field > 3 ) {

				block_field = cm_sv_build_ioar(argr, host_ip, block_size, data_ptr, data_length, &ar_is_CIR, r1_data_length);
			}

			if( block_field != 0 ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "invalid io-ar, field(%u)"
					, block_field
					);

				switch( block_field ) {

				case 255: /* no mem */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

				case 254: /* CMDEV(!) state conflict (e.g., rerun) */
					return CM_PNIO_FAULTY_Connect__CMDEV_StateConflict();

				case 253: /* out of ar-resources */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfARResources();

				case 252: /* unknown blocks */
					return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();

				case 251: /* CMRPC(!) AR-set state conflict */
					return CM_PNIO_FAULTY_Connect__CMRPC_ARSetStateConflictDuringConnectionEstablishment();

				default:
					return CM_PNIO_FAULTY_ARBlockReq(block_field); /* invalid io-ar */
				}
			}

			cres_length += CM_RSP_ALCR_SIZE;
			break;

		/***/

		case CM_BLOCK_TYPE_IOCR_REQ:

			if( block_field > 3 ) {

				block_field = cm_sv_build_iocr(argr, block_size, data_ptr);
			}

			if( block_field != 0 ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "invalid iocr, field(%u)"
					, block_field
					);

				switch( block_field ) {

				case 255: /* no mem */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

				case 254: /* no provider- or consumer-resource */
				case 253: /* unreachable */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfProviderConsumerOrAlarmResources();

				default:
					return CM_PNIO_FAULTY_IOCRBlockReq(block_field); /* invalid iocr */
				}
			}

			cres_length += CM_RSP_IOCR_SIZE;
			break;

		/***/

		case CM_BLOCK_TYPE_MCR_REQ:

			if( block_field > 3 ) {

				block_field = cm_sv_build_mcr(argr, block_size, data_ptr);
			}

			if( block_field != 0 ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "invalid mcr, field(%u)"
					, block_field
					);

				switch( block_field ) {

				case 255: /* no mem */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

				default:
					return CM_PNIO_FAULTY_MCRBlockReq(block_field); /* invalid mcr */
				}
			}
			break;

		/***/

		case CM_BLOCK_TYPE_ALCR_REQ:

			if( block_field > 3 ) {

				if( cm_is_not_null(argr->alarm_cr) ) {

					CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "more than one alarm-cr"
						);

					block_field = 253; /* more than one alarm-cr */
				}
				else {

					block_field = cm_sv_build_alcr(argr, block_size, data_ptr);
				}
			}

			if( block_field != 0 ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "invalid alcr, field(%u)"
					, block_field
					);

				switch( block_field ) {

				case 255: /* no mem */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

				case 254: /* no alarm-ref */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfProviderConsumerOrAlarmResources();

				case 253: /* more than one alarm-cr */
					return CM_PNIO_FAULTY_Connect__CMRPC_WrongBlockCount();

				default:
					return CM_PNIO_FAULTY_AlarmCRBlockReq(block_field); /* invalid alcr */
				}
			}

			cres_length += CM_RSP_ALCR_SIZE;
			break;

		/***/

		case CM_BLOCK_TYPE_EXPSUBMOD_REQ:

			if( block_field > 3 ) {

				block_field = cm_sv_build_expsubmod(argr, block_size, data_ptr);
			}

			if( block_field != 0 ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "invalid expsubmod, field(%u)"
					, block_field
					);

				switch( block_field ) {

				case 255: /* no mem */
					return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

				default:
					return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(block_field);
				}
			}

			cres_length += 0; /* see below */
			break;

		/***/

		case CM_BLOCK_TYPE_PRMSRV_REQ:

			CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "prm-block not supported"
				);

			/* note: must adapt cm_sv_srd_consistency_check_argr() when supported */

			return CM_PNIO_FAULTY_PrmServerReqBlockReq(0); /* prm-block not supported */

		/***/

		case CM_BLOCK_TYPE_IRINFOBLOCK: /* only for RTClass3 advanced */

			if( ! (CM_SV_AR_IS_RTCLASS3(argr) && CM_SV_AR_IS_ADVANCED(argr)) ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks(); /* AP01203539 */
			}
			else if( cm_is_not_null(argr->ir_info) ) { /* already seen */

				return CM_PNIO_FAULTY_Connect__CMRPC_WrongBlockCount();
			}
			else {

				if( block_field > 3 ) {

					block_field = cm_sv_build_ir_info(argr, block_size, data_ptr);
				}

				if( block_field != 0 ) {

					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "invalid ir_info_block, field(%u)"
						, block_field
						);

					switch( block_field ) {

					case 255: /* no mem */
						return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

					default:
						return CM_PNIO_FAULTY_IRInfoBlock(block_field);
					}
				}
			}
			break;

		/***/

		case CM_BLOCK_TYPE_SRINFOBLOCK:

			if( argr->ar.ar_type != CM_AR_TYPE_SINGLE_SYSRED ) { /* IOCARSR implies advanced mode, see cm_ar_graph_insert_io_ar() */

				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
			}
			else if( cm_is_not_null(argr->sr_info) ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_WrongBlockCount();
			}
			else {

				if( block_field > 3 ) {

					block_field = cm_sv_build_sr_info(argr, block_size, data_ptr);
				}

				if( block_field != 0 ) {

					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "invalid sr_info, field(%u)"
						, block_field
						);

					switch( block_field ) {

					case 255: /* no mem */
						return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

					default:
						return CM_PNIO_FAULTY_SRInfoBlock(block_field);
					}
				}
			}
			break;

		/***/

#if 0 /* AP01556507: disable ARVendorBlock (V5.2i4) */
		case CM_BLOCK_TYPE_ARVENDORBLOCK_REQ: /* AP01354956 (V5.2i3) */

			if( ! CM_SV_AR_IS_ADVANCED(argr) ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
			}
			else {

				if( block_field > 3 ) {

					block_field = cm_sv_build_arvendor_info(argr, block_size, data_ptr);
				}

				if( block_field != 0 ) {

					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "invalid ARVendorBlockReq, field(%u)"
						, block_field
						);

					switch( block_field ) {

					case 255: /* no mem */
						return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

					default:
						return CM_PNIO_FAULTY_ARVendorBlockReq(block_field);
					}
				}
			}
			break;
#endif

		/***/

		case CM_BLOCK_TYPE_ARFSUBLOCK: /* AP01034170 */

			if( ! CM_SV_AR_IS_ADVANCED(argr) ) { /* note: legacy ARs use the record 0xE050 ARFSUDataAdjust */

				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
			}
			else if( argr->cm.sv.r1_flags != 0 ) { /* R1-ARs do not have an ARFSUBlock (TIA 1240470, S2-ARs: see comment there) */

				return CM_PNIO_FAULTY_ARFSUBlock(0);
			}
			else if( ! argr->ar_com.channel->usr.sv.can_ARFSUDataAdjust ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
			}
			else if( cm_is_not_null(argr->cm.sv.rec.ARFSUDataAdjust_ptr) ) { /* already seen */

				return CM_PNIO_FAULTY_Connect__CMRPC_WrongBlockCount();
			}
			else {

				if( block_field > 3 ) {

					LSA_UINT16 block_version = CM_GET16_NTOH(data_ptr, CM_BLK_HEADER_version);

					/* ARFSUBlock ::= ARFSUDataAdjust, reusing record-handling code */
					/* note: ARFSUBlock has another block type */
					/* note: cannot safely subtract CM_RECORD_OFFSET from data_ptr to fake a "write" (ARBlockReq may be less than 64 bytes) */

					/* check for BlockVersion 1.0, PNIO-Spec: No detailed checking rules are defined. */

					if( (block_version & 0xFF00) != 0x0100 ) {

						block_field = 2; /* wrong block_version.high */
					}
					else if( (block_version & 0x00FF) != 0x0000 ) {

						block_field = 3; /* wrong block_version.low */
					}
					else if( ! cm_sv_ARFSUDataAdjust_check(data_ptr, block_size, LSA_TRUE) ) { /* does not check the BlockVersion */

						block_field = 1; /* tell "wrong length", PNIO-Spec: No detailed checking rules are defined. */
					}
					else if( ! cm_sv_ARFSUDataAdjust_store(argr, data_ptr, block_size, LSA_TRUE) ) {

						return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();
					}
					else {

						block_field = 0; /* ok */
					}
				}

				if( block_field != 0 ) {

					CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "invalid ARFSUBlock, field(%u)"
						, block_field
						);

					return CM_PNIO_FAULTY_ARFSUBlock(block_field);
				}
			}
			break;

		/***/

		case CM_BLOCK_TYPE_RSINFOBLOCK: /* TIA 1645525 */

			if( ! CM_SV_AR_IS_ADVANCED(argr) ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
			}
			else if( ! dev->gsd_ReportingSystemSupported ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
			}
			else if( cm_is_not_null(argr->rs_info) ) { /* already seen */

				return CM_PNIO_FAULTY_Connect__CMRPC_WrongBlockCount();
			}
			else {

				if( block_field > 3 ) {

					if( ! cm_ar_graph_insert_rs_info(argr, data_ptr, block_size, &block_field) ) {

						CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "invalid RSInfoBlock, field(%u)"
							, block_field
							);

						switch( block_field ) {

						case 255: /* no mem */
							return CM_PNIO_FAULTY_Connect__CMRPC_OutOfMemory();

						default:
							return CM_PNIO_FAULTY_RSInfoBlock(block_field);
						}
					}
					else {

						block_field = 0; /* ok */
					}
				}
			}
			break;

		/* note: must adapt cm_sv_srd_consistency_check_argr() when new blocks are supported */

		default:

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "unknown block-type(0x%x)"
				, block_type
				);

			return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
		}

		/***/

		if( block_field != 0 || block_size > data_length ) { /* always false (or parser has a bug) */

			return CM_PNIO_FAULTY_Connect__CMRPC_UnknownBlocks();
		}

		data_ptr    += block_size;
		data_length -= block_size;

		if( data_length == 0 ) {

			break; /* all blocks parsed */
		}
	}

	/***/

	if( argr->ar_com.has_udp ) {

		argr->ar.cmr_udp_port = CM_IANA_PNIO_UDP_UNICAST_PORT_NBO;
	}
	else {

		argr->ar.cmr_udp_port = CM_HTON16(0x8892); /* AP00583478 */
	}

	/***/

#if CM_DEBUG
	cm_ar_graph_dump(argr); /* dump before consistency-check */
#endif

	/***/

	if( ! cm_sv_ar_graph_check_io_ar(argr) ) {

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "something wrong with cmi_udp_port"
			);

		return CM_PNIO_FAULTY_ARBlockReq(11); /* something wrong with cmi_udp_port */
	}

	/***/

	cm_sv_argr_prepare_iocr_subframe_data(argr);

	/***/

	{
	LSA_UINT32 pnio_err = cm_ar_graph_check_consistency(argr);

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		return pnio_err;
	}
	}

	/*** consistency check "PDEV" ***/

	if( argr->cm.sv.nr_of_pdev_submodules != 0 ) { /* PDEV-AR */

		/* note: all of the argr->cm.sv.nr_of_pdev_submodules are "mapped", see cm_sv_subslot_range_ok() */

		if( dev->nr_of_pdev_mappings == 0 || argr->cm.sv.nr_of_pdev_submodules > dev->nr_of_pdev_mappings ) {

			return CM_PNIO_FAULTY_ARBlockReq(4); /* unreachable (because: pdev-device, no duplicates, all "mapped") */
		}

		if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_LOCAL_PDEV_INTERFACE) == 0 ) {

			/* note: an AR with interface but no ports is ok (can be generated from GSDML) */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) does not contain PDev interface"
				, argr->device_nr, argr->ar_nr
				);

			return CM_PNIO_FAULTY_Connect__CMRPC_PdevWithoutIf();
		}
	}

	if( argr->cm.sv.r1_flags != 0 ) {

		/* note: see cm_sv_build_expsubmod() for non-SR-ARs (i.e. single-sided) */

		if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_LOCAL_PDEV_INTERFACE) == 0 ) {
			CM_FATAL(); /* a bug, see above */
		}

		if( (argr->cm.sv.pd_flags & CM_SV_AR_PDFL_REMOTE_PDEV_INTERFACE) == 0 ) {

			/* note: an AR with interface but no ports is ok (can be generated from GSDML) */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) R1-AR does not contain the remote PDev interface"
				, argr->device_nr, argr->ar_nr
				);

			return CM_PNIO_FAULTY_Connect__CMRPC_PdevWithoutIf();
		}

		if( argr->cm.sv.nr_of_remote_pdev_submodules == 0 ) {
			CM_FATAL(); /* a bug, see cm_sv_build_expsubmod() */
		}

		if( (argr->ar.ar_properties & CM_AR_PROP_COMBINED_OBJECT_CONTAINER) == 0 ) {
			CM_FATAL(); /* a bug, see cm_sv_build_ioar() */
		}
	}

	if( CM_SV_AR_IS_RTCLASS3(argr) ) {

		/* refactor:
		 * - cm_sv_ar_connect_indication_CO_problem() has a check "RTClass3 only if pdev-device"
		 * - this check should be done in cm_sv_build_ioar()
		 * - this will simplifiy the checks below
		 *
		 * note: argr->cm.sv.nr_of_pdev_submodules != 0 implies CM_SV_DEV_IS_PDEV_DEVICE()
		 */

		LSA_BOOL ok;

		if( CM_SV_AR_IS_ADVANCED(argr) ) {

			if( argr->cm.sv.nr_of_pdev_submodules == 0 ) {

				ok = LSA_FALSE; /* RTC3-ARs without PDEV are currently disabled, TIA 1163523 (V6.0i2.0x) updates AP00914416 (V5.0i3) */
			}
			else if( argr->cm.sv.nr_of_pdev_submodules == dev->nr_of_pdev_mappings ) {

				ok = LSA_TRUE;
			}
			else {

				ok = LSA_FALSE; /* partial-pdev not supported */
			}
		}
		else { /* a "legacy" RTC3-AR must contain all pdev-submodules (AP00774491) */

			if( argr->cm.sv.nr_of_pdev_submodules != 0 && argr->cm.sv.nr_of_pdev_submodules == dev->nr_of_pdev_mappings ) {

				ok = LSA_TRUE;
			}
			else {

				ok = LSA_FALSE;
			}
		}

		if( ! ok ) {

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) RTC3-AR must contain all pdev-submodules"
				, argr->device_nr, argr->ar_nr
				);

			return CM_PNIO_FAULTY_ARBlockReq(4); /* tell something wrong with ar_type */
		}
	}

	/*** consistency check "SYSRED" ***/

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

		if( argr->cm.sv.nr_of_pdev_submodules == 0 ) { /* TIA 1012004 (V6.0i1.28), TIA 1012003 (V5.4i1.6), TIA 1009954 (V5.3i3.6) */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "dev/ar(%u/%u) SR-AR but no pdev-submodules"
				, argr->device_nr, argr->ar_nr
				);

			return CM_PNIO_FAULTY_ARBlockReq(4); /* same as above */
		}

		/***/

		if( cm_is_null(arset->arr[1]) ) {

			if( argr != arset->arr[0] ) {
				CM_FATAL();
			}

			/* nothing to check (this AR created a new AR-set) */
		}
		else {

			CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[0];

			if( cm_is_null(temp_argr) || argr != arset->arr[1] ) {
				CM_FATAL();
			}

			/***/

			{
			LSA_UINT32 pnio_status = cm_sv_srd_consistency_check_argr(argr, temp_argr, ar_is_CIR);

			if( pnio_status != CM_PNIO_ERR_NONE ) {

				CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "cm_sv_srd_consistency_check_argr failed, pnio_status(0x%x)"
					, pnio_status
					);

				return pnio_status; /* AR-set mismatch */
			}
			}
		}
	}

	/***/

	if( ! CmListIsEmpty(&argr->ap_list) ) { /* IODConnectRes contains a ModuleDiffBlock */

		LSA_UINT32 mdb_length = CM_RSP_MDB_SIZE;

		{
		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

		for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			mdb_length += CM_RSP_API_SIZE;

			for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				mdb_length += CM_RSP_MOD_SIZE;

				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

					mdb_length += CM_RSP_SUB_SIZE;
				}
			}
		}
		}

		cres_length += mdb_length;
	}

	if( CM_SV_AR_IS_ADVANCED(argr) ) { /* IODConnectRes contains an ARServerBlockRes */

		/* note: must not use current length of station-name */

		cres_length += (CM_RSP_ARSERVER_SIZE + 240); /* must not use OHA_MAX_STATION_NAME_LEN (=255) */

	}

	if( CM_SV_AR_IS_ADVANCED(argr) ) { /* IODConnectRes contains an ARVendorBlockRes */

		CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend;

		for( CmListEach(ar_vend, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE) ) {

			cres_length += CM_SV_ARVENDORBLOCK_RES_MAXSIZE;
		}
	}

	if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) { //R1 marker

		cres_length = 0; /* see below */
	}

	if( cres_length > alloc_len ) {

		return CM_PNIO_FAULTY_Connect__CMRPC_ArgsLengthInvalid();
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== GET MODULE STATE ======================================================*/
/*===========================================================================*/

static LSA_UINT16
cm_sv_get_module_state(
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod
) {
	LSA_UINT16 mod_state;

	/* note: "wrong module" is legacy (CM V4.2i2, AP00826555) */

	if( ! CM_SV_SLOT_IS_PLUGGED(ar_mod->sv.real_sl) ) {

		mod_state = CM_MOD_STATE_NO_MODULE;
	}
	else if( ar_mod->soll.mod_ident != ar_mod->sv.real_sl->mod_ident ) {

		mod_state = CM_MOD_STATE_SUBSTITUTED_MODULE;
	}
	else {

		mod_state = CM_MOD_STATE_PROPER_MODULE;
	}

	return mod_state;
}

/*===========================================================================*/
/*=== GET SUBMODULE STATE "IDENT" ===========================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_get_submodule_state_ident( /* returns NO_SUBMODULE or WRONG or OK (not SUBSTITUTE) */
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
) {
	CM_SV_SLOT_PTR_TYPE sl = ar_mod->sv.real_sl;
	CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;
	LSA_UINT16 sub_state;

	if( ! CM_SV_SUBSLOT_IS_PLUGGED(su) ) {

		sub_state = CM_SUB_STATE_IDENT_NO_SUBMODULE;
	}
	else if( ar_mod->soll.mod_ident != sl->mod_ident ) {

		sub_state = CM_SUB_STATE_IDENT_WRONG;
	}
	else if( ar_sub->soll.sub_ident != su->sub_ident ) {

		sub_state = CM_SUB_STATE_IDENT_WRONG;
	}
	else { /* mod-ident and sub-ident match */

		sub_state = CM_SUB_STATE_IDENT_OK;
	}

	return sub_state;
}

/*===========================================================================*/
/*=== GET SUBMODULE STATE ===================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_get_submodule_state(
	CM_SV_DEVICE_CONST_PTR_TYPE dev, /* LSA_NULL if "without diag" */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_AR_GRAPH_AP_CONST_PTR_TYPE ar_ap,
	CM_AR_GRAPH_MODULE_CONST_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub
) {
	CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;
	LSA_UINT16 sub_state;

	/* logic revised with V5.0i2 (AP01025673) */

	sub_state = cm_sv_get_submodule_state_ident(ar_mod, ar_sub); /* "substitute" see below */

	/***/

	if( su->owner_ar_nr != argr->ar_nr ) { /* locked (i.e., not owned) */

		if( su->owner_ar_nr == 0 ) {

			sub_state |= CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED;
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_AR_PROBLEM) != 0 ) {

			sub_state |= CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED; /* tell so-locked (AP00914416), see cm_sv_ar_ownership_build() too */
		}
		else if( su->cand[0].is_supervisor ) {

			sub_state |= CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR;
		}
		else {

			sub_state |= CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER;
		}

		if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_TNA) != 0 ) {

			sub_state |= CM_SUB_STATE_ADD_INFO_TAKEOVER_NOT_ALLOWED;
		}
	}
	else { /* owned */

		if( sub_state == CM_SUB_STATE_IDENT_WRONG ) { /* ident mismatch (but plugged) */

			if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_WRONG) == 0 ) { /* user said "upgrade from wrong to substitute" */

				sub_state = CM_SUB_STATE_IDENT_SUBSTITUTE;
			}
		}

		/***/

		if( sub_state == CM_SUB_STATE_IDENT_NO_SUBMODULE ) {

			sub_state |= CM_SUB_STATE_AR_INFO_OWNED;
		}
		else if( sub_state == CM_SUB_STATE_IDENT_WRONG ) { /* note: "wrong" cannot be A.R.P. (search for AP00148255) */

			sub_state |= CM_SUB_STATE_AR_INFO_OWNED;
		}
		else if( (ar_sub->sv.sm_flags & (CM_SV_AR_SMFL_IS_ARP | CM_SV_AR_SMFL_DIRTY | CM_SV_AR_SMFL_PBE_PLG_REL)) != 0 ) {

			sub_state |= CM_SUB_STATE_AR_INFO_APPL_READY_PENDING;
		}
		else if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_ARP_E000) != 0 ) {

			sub_state |= CM_SUB_STATE_AR_INFO_APPL_READY_PENDING; /* note: this kind of "a.r.p." remains even if submodule is pulled and plugged */
		}
		else {

			/* note: since V6.0i1.50 (TIA 1092256) "pdev-problem" is reported as "diagnosis" (formerly: "a.r.p." on all pdev-submodules) */

			sub_state |= CM_SUB_STATE_AR_INFO_OWNED;
		}
	}

	/***/

	if( cm_is_not_null(dev) && cm_is_not_null(ar_ap) && cm_is_not_null(ar_mod) ) {

		LSA_UINT32 temp_maint;

		if( cm_sv_diag_maint_qual(dev, ar_ap->sv.real_ap, ar_mod->sv.real_sl, su, LSA_NULL, LSA_NULL, &temp_maint) ) {

			sub_state |= CM_SUB_STATE_DIAG_INFO_AVAILABLE;
		}

		if( (temp_maint & CM_MAINTENANCE_STATUS_REQUIRED) != 0 ) {

			sub_state |= CM_SUB_STATE_MAINTENANCE_REQUIRED;
		}

		if( (temp_maint & CM_MAINTENANCE_STATUS_DEMANDED) != 0 ) {

			sub_state |= CM_SUB_STATE_MAINTENANCE_DEMANDED;
		}

#if 0 /* future requirement, see TIA 1376282 */
		if( (temp_maint & CM_MAINTENANCE_STATUS_XXX) != 0 ) {

			sub_state |= CM_SUB_STATE_ADVICE_AVAILABLE;
		}
#endif

#if CM_CFG_SV_AR_HAS_MCONS
		if( ar_sub->out_via_mcr ) {

			LSA_UINT16 ext_err;

			ext_err = cm_sv_mcons_diag_value(argr, LSA_NULL, ar_ap, ar_mod, ar_sub);

			if( ext_err != 0 ) { /* has (appearing) diagnosis */

				sub_state |= CM_SUB_STATE_DIAG_INFO_AVAILABLE;
			}
		}
#endif
	}

	/***/

	sub_state |= CM_SUB_STATE_FORMAT_1;

	return sub_state;
}

/*===========================================================================*/
/*=== MARSHAL MODULE-DIFF-BLOCK =============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_marshal_module_diff_block(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_UPPER_ALARM_CONST_PTR_TYPE plug_alarm,
	LSA_UINT32 mdb_context, /* see CM_SV_MDB_CONTEXT_READ, ... */
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	LSA_UINT32 offs = *offs_ptr; /* update at the end */
	LSA_UINT32 offs_blklen;
	CM_AR_GRAPH_PTR_TYPE sysred_argr;

	if( cm_is_null( argr) ) {

		CM_FATAL();
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		if( cm_is_not_null(plug_alarm) ) {

			sysred_argr = LSA_NULL;
		}
		else {

			CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

			sysred_argr = cm_sv_arset_get_primary_or_first(arset);

			if( cm_is_null(sysred_argr) ) {

				/* case: AR1 comes (first); AR2 comes (backup); AR1 dies; AR1 comes again (AP01320211) */
				/* case: AR1 comes (first); AR2 comes (backup); AR1 dies before AR2 applrdy (TIA 1839413) */

#if CM_SV_ARSET_MAX_ARS != 2
#error "who changed that?"
#endif
				if(dev->pdev_owner_ar_nr == argr->ar_nr) {

					sysred_argr = LSA_NULL;
				}
				else if( arset->arr[1] == argr ) {

					CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[0];

					if( (temp_argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) == 0 ) { /* TIA 1839413 */

						sysred_argr = temp_argr;
					}
				}
				else {

					CM_AR_GRAPH_PTR_TYPE temp_argr = arset->arr[1];

					if( cm_is_not_null(temp_argr) && (temp_argr->cm.sv.ar_flags & CM_SV_AR_FL_DOWN_PHASE) == 0 ) { /* TIA 1839413 */

						sysred_argr = temp_argr;
					}
				}
			}

			if( sysred_argr == argr ) {

				sysred_argr = LSA_NULL;
			}
		}
	}
	else {

		sysred_argr = LSA_NULL;
	}

	/***/

	/*
	 * ModuleDiffBlock ::=
	 *   BlockHeader,
	 *   NumberOfAPIs,
	 *   ( API,
	 *     NumberOfModules,
	 *     ( SlotNumber, ModuleIdentNumber, ModuleState,
	 *       NumberOfSubmodules,
	 *       ( SubslotNumber, SubmoduleIdentNumber, SubmoduleState
	 *       )*
	 *     )*
	 *   )*
	 */

	if( offs + CM_RSP_MDB_SIZE > size ) {
		return LSA_FALSE; /* buffer too small */
	}

	CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_MOD_DIFF);
	offs_blklen = offs + 2; /* block_length written below */
	CM_PUT16_HTON(data, offs + 4, 0x0100/*V1.0*/);

	/***/

	{
	LSA_UINT32 offs_diff;
	LSA_UINT16 api_cnt;

	offs_diff = offs;

	api_cnt = 0; /* number_of_apis will be written below */

	offs += CM_RSP_MDB_SIZE;

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_AR_GRAPH_AP_PTR_TYPE sysred_ap = cm_is_null(sysred_argr) ? LSA_NULL : cm_ar_graph_find_ap(sysred_argr, ar_ap->api);
		LSA_UINT32 offs_api;
		LSA_UINT16 mod_cnt;

		if( mdb_context == CM_SV_MDB_CONTEXT_PBE_BRES ) {
			/* marshalling writes API-block and module-block always and does "undo" if no submodule-block */
			/* this works for connect (all submodules) but not for PBE: IOC calculated the RPC buffer for 1 submodule */
			/* beautify this! it should be possible to write the API-block and module-block after the loops in the "if not undo" case */
			LSA_BOOL more = LSA_FALSE;
			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
			for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;
				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {
					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {
						more = LSA_TRUE;
					}
				}
			}
			if( ! more ) {
				continue;
			}
		}

		if( offs + CM_RSP_API_SIZE > size ) {
			return LSA_FALSE; /* buffer too small */
		}

		offs_api = offs;

		CM_PUT32_HTON(data, offs + CM_RSP_API_api, ar_ap->api);
		mod_cnt = 0; /* number_of_modules will be written below */

		offs += CM_RSP_API_SIZE;

		{
		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE sysred_mod = cm_is_null(sysred_ap) ? LSA_NULL : cm_ar_graph_find_module(sysred_ap, ar_mod->slot_nr);
			CM_SV_SLOT_PTR_TYPE sl = ar_mod->sv.real_sl;
			LSA_UINT16 mod_state;
			LSA_UINT32 offs_mod;
			LSA_UINT16 sub_cnt;

			mod_state = cm_sv_get_module_state(ar_mod); /* note: "sysred_mod" has the same ident-number */

			if( mdb_context == CM_SV_MDB_CONTEXT_PBE_BRES ) {
				/* marshalling writes API-block and module-block always and does "undo" if no submodule-block */
				/* this works for connect (all submodules) but not for PBE: IOC calculated the RPC buffer for 1 submodule */
				/* beautify this! it should be possible to write the API-block and module-block after the loops in the "if not undo" case */
				LSA_BOOL more = LSA_FALSE;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;
				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {
					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {
						more = LSA_TRUE;
					}
				}
				if( ! more ) {
					continue;
				}
			}

			if( offs + CM_RSP_MOD_SIZE > size ) {
				return LSA_FALSE; /* buffer too small */
			}

			offs_mod = offs;

			CM_PUT16_HTON(data, offs + CM_RSP_MOD_slot_nr, ar_mod->slot_nr);
			CM_PUT32_HTON(data, offs + CM_RSP_MOD_mod_ident, sl->mod_ident); /* don't care if "no module" */
			CM_PUT16_HTON(data, offs + CM_RSP_MOD_mod_state, mod_state);

			sub_cnt = 0; /* number_of_submodules will be written below */
			offs += CM_RSP_MOD_SIZE;

			{
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				LSA_BOOL sub_context;

				if( mdb_context == CM_SV_MDB_CONTEXT_READ ) {

					sub_context = LSA_TRUE; /* all */
				}
				else if( cm_is_not_null(plug_alarm) ) { /* i.e., CM_SV_MDB_CONTEXT_PLUG_ARDY */

					if( plug_alarm->u.sv.void_ptr == (LSA_VOID_PTR_TYPE)ar_sub ) {

						sub_context = LSA_TRUE;
					}
					else {

						sub_context = LSA_FALSE;
					}
				}
				else if( mdb_context == CM_SV_MDB_CONTEXT_PBE_BRES || mdb_context == CM_SV_MDB_CONTEXT_PBE_ARDY ) {

					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

						sub_context = LSA_TRUE;
					}
					else {

						sub_context = LSA_FALSE;
					}
				}
				else { /* i.e., CM_SV_MDB_CONTEXT_CONN_CRES or CM_SV_MDB_CONTEXT_CONN_ARDY */

					sub_context = LSA_TRUE; /* all */
				}

				/***/

				if( sub_context ) {

					CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;
					LSA_UINT16 sub_state;

					if( mod_state == CM_MOD_STATE_NO_MODULE ) {

						CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "ap/sl/su(%u/%u/%u) mod-state(NO_MODULE)"
							, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr
							);

						sub_state = CM_SUB_STATE_PULLED; /* what the client assumes if the module is pulled */

						sub_cnt += 1; /* do not marshal, see below (AP01265470) */
					}
					else {

						CM_AR_GRAPH_SUBMODULE_PTR_TYPE sysred_sub = cm_is_null(sysred_mod) ? LSA_NULL : cm_ar_graph_find_submodule(sysred_ap, sysred_mod, ar_mod->slot_nr, ar_sub->subslot_nr);

						if( cm_is_not_null(sysred_sub) && cm_is_not_null(sysred_argr) ) {

							sub_state = cm_sv_get_submodule_state(dev/*with-diag*/, sysred_argr, sysred_ap, sysred_mod, sysred_sub);
						}
						else if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_CIR) != 0 && su->cand[0].ar_nr == argr->ar_nr ) {

							sub_state = CM_SUB_STATE_PULLED; /* CIR: report "pulled" until appl-ready */
						}
						else {

							sub_state = cm_sv_get_submodule_state(dev/*with-diag*/, argr, ar_ap, ar_mod, ar_sub);
						}

						CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "ap/sl/su(%u/%u/%u) sub-state(0x%04x) not-good(%u)"
							, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr
							, sub_state, (sub_state != CM_SUB_STATE_GOOD)
							);

						if( sub_state != CM_SUB_STATE_GOOD ) {

							if( offs + CM_RSP_SUB_SIZE > size ) {
								return LSA_FALSE; /* buffer too small */
							}

							CM_PUT16_HTON(data, offs + CM_RSP_SUB_subslot_nr, ar_sub->subslot_nr);
							CM_PUT32_HTON(data, offs + CM_RSP_SUB_sub_ident, su->sub_ident); /* don't care if "no submodule" */
							CM_PUT16_HTON(data, offs + CM_RSP_SUB_sub_state, sub_state);

							offs += CM_RSP_SUB_SIZE;

							sub_cnt += 1;
						}
						else {

							/* do not marshal */
						}
					}

					/***/

					if( mdb_context == CM_SV_MDB_CONTEXT_READ ) {

						/* do nothing, read or read-implicit(target-aruuid) */
					}
					else { /* connect-phase or prm-begin-end or plug/released-alarm */

						if( cm_is_not_null(plug_alarm) ) {

							/* CM_SV_AR_SMFL_EX_NEWDTA is set in cm_sv_ar_almi_ack_indication() */
						}
						else { /* connect-phase or prm-begin-end */

							if( mdb_context == CM_SV_MDB_CONTEXT_CONN_ARDY ) {

								/* note: the ModuleDiffBlock signals "physical" changes only in the connect-phase (before alarm-enable) */

								ar_sub->sv.st_told = sub_state;

								if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_CIR) != 0 ) {

									ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_CIR;

									if( ! CM_SV_SUBSLOT_IS_PLUGGED(su) && ar_sub->sv.st_told == CM_SUB_STATE_PULLED ) {

										/* do nothing (cm_sv_ar_submodule_state_reset() would trigger a pull-alarm) */
									}
									else {

										LSA_BOOL aplrdy_cancelled = cm_sv_ar_submodule_state_reset(ar_sub, LSA_TRUE/*dirty*/, LSA_TRUE/*wrong*/);

										if( aplrdy_cancelled ) {

											CM_FATAL();
										}

										argr->cm.sv.ar_flags |= CM_SV_AR_FL_DO_PULL_PLUG;
									}
								}
							}

							if( mdb_context == CM_SV_MDB_CONTEXT_PBE_ARDY ) {

								ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_PBE; /* reset */

								if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE_PLG_REL) != 0 ) {

									ar_sub->sv.sm_flags &= ~(CM_SV_AR_SMFL_PBE_PLG_REL | CM_SV_AR_SMFL_IS_PRMED | CM_SV_AR_SMFL_IS_ARP);
								}
							}

							/***/

							if( (ar_sub->sv.sm_flags & (CM_SV_AR_SMFL_IS_PRMED | CM_SV_AR_SMFL_IS_ARP)) == (CM_SV_AR_SMFL_IS_PRMED | 0) ) { /* owned and ok/subst and "not A.R.P." */

								ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_EX_NEWDTA;
							}
						}
					}
				}
			}
			}

			/***/

			if( sub_cnt != 0 ) { /* sub_context and not good */

				if( mod_state == CM_MOD_STATE_NO_MODULE ) {

					sub_cnt = 0; /* PNIO-Spec: If ModuleState=NO_MODUL then NumberOfSubmodules shall be zero. The subsequent part shall be omitted. */
				}

				CM_PUT16_HTON(data, offs_mod + CM_RSP_MOD_number_of_submodules, sub_cnt);

				mod_cnt += 1;
			}
			else {

				offs = offs_mod; /* rollback */
			}
		}
		}

		/***/

		if( mod_cnt != 0 ) {

			CM_PUT16_HTON(data, offs_api + CM_RSP_API_number_of_modules, mod_cnt);

			api_cnt += 1;
		}
		else {

			offs = offs_api; /* rollback */
		}
	}
	}

	/***/

	if( api_cnt != 0 ) {

		LSA_UINT32 blklen;

		CM_PUT16_HTON(data, offs_diff + CM_RSP_MDB_number_of_apis, api_cnt);

		/***/

		blklen = offs - (offs_blklen + 2/*block_length*/);

		CM_ASSERT(blklen <= 0xFFFF);

		CM_PUT16_HTON(data, offs_blklen, (LSA_UINT16)blklen);

		/***/

		if( offs > size ) {
			CM_FATAL(); /* a bug */
		}

		*offs_ptr = offs; /* update */
	}
	else {

		/* rollback: don't update offs_ptr */
	}
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== BUILD CONNECT-RESPONSE FROM AR-GRAPH (MARSHAL) ========================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_build_connect_response(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd_params = cm_edd_channel_params(channel);
	CM_COMMON_MEM_U8_PTR_TYPE data_ptr;
	LSA_UINT32 data_length;
	CM_COMMON_MEM_U8_PTR_TYPE ar_ptr;

	data_ptr    = rpc->args.sv.call->ndr_data   + CLRPC_PNIO_OFFSET; /* sanity-checked in caller */
	data_length = rpc->args.sv.call->alloc_len  - CLRPC_PNIO_OFFSET;

	CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) data_length(%u)"
		, argr->device_nr, argr->ar_nr
		, data_length
		);

	/*
	 * IODConnectRes ::=
	 *   ARBlockRes,
	 *   {
	 *   [IOCRBlockRes*], <------ optional for easy-supervisor
	 *   [AlarmCRBlockRes], <---- optional for easy-supervisor
	 *   [ModuleDiffBlock], <---- optional for easy-supervisor
	 *   [ARRPCBlockRes],
	 *   [ARServerBlock], <------ if StartupMode == Advanced
	 *   [ARVendorBlockRes*] <--- if StartupMode == Advanced
	 *   }
	 */

	ar_ptr = data_ptr; /* see below */

	/*
	 * marshal ARBlockRes
	 */

	if( CM_RSP_AR_SIZE > data_length ) {
		goto buffer_too_small;
	}

	CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_AR_RSP);
	CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_length,  CM_RSP_AR_SIZE - CM_BLK_HEADER_TypLen_SIZE);
	CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_version, 0x0100/*V1.0*/);

	CM_PUT16_HTON(data_ptr, CM_RSP_AR_ar_type, argr->ar.ar_type);
	CM_PUT_UUID_HTON(data_ptr, CM_RSP_AR_ar_uuid, &argr->ar.ar_uuid);
	CM_PUT16_HTON(data_ptr, CM_RSP_AR_session_key, argr->cm.sv.session_key_connect);
	CM_MEMCPY(data_ptr + CM_RSP_AR_cmr_mac, edd_params->MACAddr.MacAdr, CM_PDU_MAC_SIZE);
	CM_PUT16_HTON(data_ptr, CM_RSP_AR_cmr_udp_port, CM_NTOH16(argr->ar.cmr_udp_port));

	data_ptr    += CM_RSP_AR_SIZE;
	data_length -= CM_RSP_AR_SIZE;

	/*
	 * marshal an IOCRBlockRes for each IOCR
	 */

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

		if( CM_RSP_IOCR_SIZE > data_length ) {
			goto buffer_too_small;
		}

		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_IOCR_RSP);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_length,  CM_RSP_IOCR_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_version, 0x0100/*V1.0*/);

		CM_PUT16_HTON(data_ptr, CM_RSP_IOCR_iocr_type, ar_iocr->iocr_type);
		CM_PUT16_HTON(data_ptr, CM_RSP_IOCR_iocr_ref,  ar_iocr->iocr_ref);
		CM_PUT16_HTON(data_ptr, CM_RSP_IOCR_frame_id,  ar_iocr->frame_id);

		data_ptr    += CM_RSP_IOCR_SIZE;
		data_length -= CM_RSP_IOCR_SIZE;
	}
	}

	/*
	 * marshal an AlarmCRBlockRes for each(=1) AlarmCR
	 */

	{
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE ar_alcr = argr->alarm_cr;

	if( cm_is_not_null(ar_alcr) ) {

		if( CM_RSP_ALCR_SIZE > data_length ) {
			goto buffer_too_small;
		}

		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_type,    CM_BLOCK_TYPE_ALCR_RSP);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_length,  CM_RSP_ALCR_SIZE - CM_BLK_HEADER_TypLen_SIZE);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_version, 0x0100/*V1.0*/);

		CM_PUT16_HTON(data_ptr, CM_RSP_ALCR_alcr_type, CM_ALCR_TYPE_ALARM);
		CM_PUT16_HTON(data_ptr, CM_RSP_ALCR_local_alarm_ref, ar_alcr->local_alarm_ref);
		CM_PUT16_HTON(data_ptr, CM_RSP_ALCR_max_alarm_data_length, channel->acp.max_alarm_data_length);

		data_ptr    += CM_RSP_ALCR_SIZE;
		data_length -= CM_RSP_ALCR_SIZE;
	}
	}

	/*
	 * marshal the ModuleDiffBlock
	 */

	{
	LSA_UINT32 written = 0;
	LSA_BOOL ok;

	ok = cm_sv_marshal_module_diff_block(argr, LSA_NULL, CM_SV_MDB_CONTEXT_CONN_CRES, data_ptr, data_length, &written);

	if( ! ok ) {

		goto buffer_too_small;
	}
	else {

		data_ptr    += written;
		data_length -= written;
	}
	}

	/*
	 * marshal the ARServerBlock (AP00996856)
	 */

	if( CM_SV_AR_IS_ADVANCED(argr) ) {

		CM_COMMON_MEM_U8_PTR_TYPE nos;
		LSA_UINT16 nos_length;
		LSA_UINT16 pad_length;

		/***/

		if( ! cm_oha_get_name_of_station(channel, &nos_length, &nos) ) {

			/* do nothing */
		}

		/***/

		if( nos_length > 240 ) { /* actually always false, search for OHA_MAX_STATION_NAME_LEN */

			nos_length = 0;
		}

		pad_length = ((4 - ((CM_RSP_ARSERVER_SIZE + nos_length) & 3)) & 3);

		/***/

		if( CM_RSP_ARSERVER_SIZE + nos_length + pad_length > data_length ) {
			goto buffer_too_small;
		}

		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_type, CM_BLOCK_TYPE_ARSERVERBLOCK);
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_length, (LSA_UINT16)(CM_RSP_ARSERVER_SIZE - CM_BLK_HEADER_TypLen_SIZE + nos_length + pad_length));
		CM_PUT16_HTON(data_ptr, CM_BLK_HEADER_version, 0x0100/*V1.0*/);

		CM_PUT16_HTON(data_ptr, CM_RSP_ARSERVER_cmr_name_length, nos_length);

		data_ptr    += CM_RSP_ARSERVER_SIZE;
		data_length -= CM_RSP_ARSERVER_SIZE;

		CM_MEMCPY(data_ptr, nos, nos_length);

		data_ptr    += nos_length;
		data_length -= nos_length;

		CM_MEMSET(data_ptr, 0, pad_length);

		data_ptr    += pad_length;
		data_length -= pad_length;
	}

	/*
	 * marshal the ARVendorBlockRes (AP01354956)
	 */

	if( CM_SV_AR_IS_ADVANCED(argr) ) {

		CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend;

		for( CmListEach(ar_vend, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE) ) {

			if( ar_vend->res_data_len != 0 ) { /* not always true, see cm_sv_ar_connect_update() */

				if( ar_vend->res_data_len > data_length ) {
					goto buffer_too_small;
				}

				CM_MEMCPY(data_ptr, ar_vend->data + ar_vend->req_data_len, ar_vend->res_data_len);

				data_ptr    += ar_vend->res_data_len;
				data_length -= ar_vend->res_data_len;
			}
		}
	}

	/***/

	if( data_length >= 0x80000000 ) {
		CM_FATAL(); /* a bug, subtracted too much (PDU is smaller than 2GB...) */
	}

	return CM_CAST_U32(data_ptr - ar_ptr);

	/***/

buffer_too_small:
	CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "buffer too small"
			);
	return 0; /* error */
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
