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
/*  F i l e               &F: cm_sv6.c                                  :F&  */
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
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7600
#define CM_MODULE_ID       7600

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== AR CONNECT ALLOC ======================================================*/
/*===========================================================================*/

CM_UPPER_SV_AR_CONNECT_PTR_TYPE
cm_sv_ar_connect_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_connect;
	LSA_UINT16 nr_of_iocrs;
	LSA_UINT32 alloc_size;
	LSA_UINT32 cmi_station_name_offset;
	LSA_UINT32 ARFSUBlock_offset;
	LSA_UINT32 ARVendorBlock_offset;
	LSA_UINT32 ARVendorBlock_count;

	/***/

	nr_of_iocrs = 0;

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

		nr_of_iocrs += 1;
	}
	}

	alloc_size = sizeof(*ar_connect);

	if( nr_of_iocrs == 0 ) {

		/* easy-supervisor, ar_connect->iocr[0] will be left uninitialized */
	}
	else {

		alloc_size += (nr_of_iocrs - 1) * sizeof(ar_connect->iocr[0]);
	}

	cmi_station_name_offset = alloc_size;

	alloc_size += argr->ar.cmi_station_name_length + 1/*zero*/;

	if( CM_SV_AR_IS_ADVANCED(argr) && argr->cm.sv.rec.ARFSUDataAdjust_len != 0 ) { /* AP01034170 */

		alloc_size = ((alloc_size + 3) & ~3); /* align the ARFSUBlock */

		ARFSUBlock_offset = alloc_size;

		alloc_size += argr->cm.sv.rec.ARFSUDataAdjust_len;
	}
	else {

		ARFSUBlock_offset = 0;
	}

	if( CM_SV_AR_IS_ADVANCED(argr) && ! CmListIsEmpty(&argr->arvendor_list) ) {

		alloc_size = ((alloc_size + 3) & ~3); /* align the ARVendorBlock "array" */

		ARVendorBlock_offset = alloc_size;
		ARVendorBlock_count = 0;

		{
		CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend;

		for( CmListEach(ar_vend, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE) ) {

			LSA_UINT16 vend_len = ar_vend->req_data_len;

			if( vend_len < 2+2 ) {

				CM_FATAL(); /* a bug (user needs something; at least the block-type/length) */
			}
			else if( vend_len < CM_SV_ARVENDORBLOCK_RES_MAXSIZE ) { /* see ARVendorBlockRes in FDIS 61784-2_V23_FINAL__TC2WG11_TO_PNO_CO__Rev15.doc */

				vend_len = CM_SV_ARVENDORBLOCK_RES_MAXSIZE;
			}
			else {

				/* ARVendorBlockRes fits into ARVendorBlockReq */
			}

			vend_len = ((vend_len + 3) & ~3); /* align */

			alloc_size += sizeof(LSA_UINT32) + vend_len; /* space for "offset into array" and ARVendorBlockReq/ARVendorBlockRes */

			ARVendorBlock_count += 1;
		}
		}

		alloc_size += sizeof(LSA_UINT32); /* space for "size" */
	}
	else {

		ARVendorBlock_offset = 0;
		ARVendorBlock_count = 0;
	}

	/***/

	ar_connect = (CM_UPPER_SV_AR_CONNECT_PTR_TYPE)cm_sv_upper_mem_alloc(argr->ar_com.channel, alloc_size);

	if( cm_is_null(ar_connect) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) could not cm_sv_ar_connect_alloc"
			, argr->device_nr, argr->ar_nr
			);
	}
	else {

		LSA_UINT8 *cmi_station_name_ptr = (LSA_UINT8 *)ar_connect + cmi_station_name_offset;

		CM_MEMCPY(cmi_station_name_ptr, argr->ar.cmi_station_name, argr->ar.cmi_station_name_length);

		cmi_station_name_ptr[argr->ar.cmi_station_name_length] = 0; /* zero-terminated */

		/* caller information */

		ar_connect->host_ip = argr->ar.host_ip;

		ar_connect->cmi_station_name = cmi_station_name_ptr;

		ar_connect->cmi_obj_uuid = argr->ar.cmi_obj_uuid;

		ar_connect->ar_uuid = argr->ar.ar_uuid;

		/* AR information */

		ar_connect->alarm_send_max_length = cm_sv_ar_almi_send_max_length(argr, 0/*!*/);

		ar_connect->ar_type = argr->ar.ar_type;

		ar_connect->ar_properties = argr->ar.ar_properties;

		ar_connect->is_iosar_with_device_access = (cm_is_iosar_with_device_access(argr) ? CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_YES : CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_NO);

		/* AR information for FSU (fast startup) */

		if(
			ARFSUBlock_offset != 0 /* see above */
			&& argr->cm.sv.rec.ARFSUDataAdjust_len != 0 /* actually always true */
			&& cm_is_not_null(argr->cm.sv.rec.ARFSUDataAdjust_ptr) /* actually always true */
		) {

			LSA_UINT8 *ARFSUBlock_ptr = (LSA_UINT8 *)ar_connect + ARFSUBlock_offset;

			CM_MEMCPY(ARFSUBlock_ptr, argr->cm.sv.rec.ARFSUDataAdjust_ptr, argr->cm.sv.rec.ARFSUDataAdjust_len);

			ar_connect->ARFSUBlock_ptr = ARFSUBlock_ptr; /* AP01034170 */
		}
		else {

			ar_connect->ARFSUBlock_ptr = LSA_NULL;
		}

		/* AR information for CM_AR_TYPE_SINGLE_SYSRED (system redundancy) */

		if( cm_is_not_null(argr->sr_info) ) {

			ar_connect->sr_rdht_msec = argr->sr_info->rdh_factor * 1/*msec*/;

			ar_connect->sr_properties = argr->sr_info->sr_properties;
		}
		else {

			ar_connect->sr_rdht_msec = 0;

			ar_connect->sr_properties = 0;
		}

		ar_connect->sr_firstAR = CM_SV_SR_FIRST_AR_NO;

		/* AR vendor-specific information for CM_AR_PROP_STARTUP_MODE_ADVANCED */

		if( ARVendorBlock_offset != 0 && ARVendorBlock_count != 0 ) { /* see above */

			LSA_UINT32 *ARVendorBlock_ptr = (LSA_UINT32 *)((LSA_UINT8 *)ar_connect + ARVendorBlock_offset);

			{
			LSA_UINT32 idx = 0;
			LSA_UINT32 ofs = ARVendorBlock_count + 1;
			CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend;

			for( CmListEach(ar_vend, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE) ) {

				LSA_UINT16 vend_len = ar_vend->req_data_len;

				if( vend_len < 2+2 ) {

					CM_FATAL(); /* a bug (see alloc above) */
				}
				else {

					ARVendorBlock_ptr[idx] = ofs;

					CM_MEMCPY(&ARVendorBlock_ptr[ofs], ar_vend->data, vend_len);
				}

				if( vend_len < CM_SV_ARVENDORBLOCK_RES_MAXSIZE ) {

					vend_len = CM_SV_ARVENDORBLOCK_RES_MAXSIZE;
				}

				vend_len = ((vend_len + 3) & ~3); /* align (see alloc above) */

				ofs += (vend_len / 4);
				idx += 1;
			}

			ARVendorBlock_ptr[ARVendorBlock_count] = ofs; /* "size" */
			}

			ar_connect->ARVendorBlock_cnt = ARVendorBlock_count;
			ar_connect->ARVendorBlock_ptr = ARVendorBlock_ptr;
		}
		else {

			ar_connect->ARVendorBlock_cnt = 0;
			ar_connect->ARVendorBlock_ptr = LSA_NULL;
		}

		/* AR information for "Reporting System" (only for CM_AR_PROP_STARTUP_MODE_ADVANCED) */

		ar_connect->has_RSInfoBlock = (cm_is_not_null(argr->rs_info) ? CM_SV_HAS_RSINFOBLOCK_YES : CM_SV_HAS_RSINFOBLOCK_NO);

		/***/

		ar_connect->disconnect.reason_code = CM_AR_REASON_NONE;

		ar_connect->disconnect.reason_pnio_status = CM_PNIO_ERR_NONE;

		ar_connect->disconnect.was_in_data = CM_SV_WAS_IN_DATA_NO;

		/***/

		ar_connect->send_clock_31250ns = 0; /* sanity, see cm_sv_ar_connect_build_notnull() */

		ar_connect->nr_of_iocrs = 0; /* sanity, see cm_sv_ar_connect_build_notnull() */
	}

	/***/

	return ar_connect;
}

/*===========================================================================*/
/*=== AR CONNECT BUILD ======================================================*/
/*===========================================================================*/

CM_UPPER_RQB_PTR_TYPE
cm_sv_ar_connect_build_notnull(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_connect = argr->cm.sv.ar_connect_mem;
	CM_UPPER_RQB_PTR_TYPE event;

	/***/

	if( cm_is_null(ar_connect) ) {

		CM_FATAL(); /* a bug */
	}

	/***/

	ar_connect->send_clock_31250ns = 0;

	ar_connect->nr_of_iocrs = 0;

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

		CM_UPPER_SV_AR_CONNECT_IOCR_PTR_TYPE cn_iocr = &ar_connect->iocr[ar_connect->nr_of_iocrs];

		/***/

		if( ar_connect->send_clock_31250ns == 0 ) {

			ar_connect->send_clock_31250ns = ar_iocr->cm.adpt.send_clock; /* not: argr->ar_com.ar_send_clock, TIA 216515 */
		}

		/***/

		cn_iocr->iocr_ref = ar_iocr->iocr_ref;

		cn_iocr->frame_id = ar_iocr->frame_id;

		cn_iocr->iocr_type = ar_iocr->iocr_type;

		cn_iocr->iocr_properties = ar_iocr->iocr_properties;

		cn_iocr->iocr_user_id.void_ptr = 0; /* zero! see documentation */

		cn_iocr->reduction_ratio = ar_iocr->cm.adpt.reduction_ratio; /* TIA 216515 */

		cn_iocr->cycle_phase = ar_iocr->cm.adpt.phase; /* AP00856570, AP01388026 (tell adapted phase) */

		cn_iocr->sum_iodata = ar_iocr->cm.sv.sum_iodata; /* TIA 2612940 */
		cn_iocr->sum_ioxs   = ar_iocr->cm.sv.sum_ioxs;   /* TIA 2612940 */

		if( ar_iocr->cm.subframe_data != 0 ) {

			cn_iocr->io_buffer_size = ((ar_iocr->cm.subframe_data >> 8) & 0xFF);
		}
		else {

			cn_iocr->io_buffer_size = ar_iocr->cm.partial_length;
		}

		/***/

		cn_iocr->eddx_apdu_status_length = ar_iocr->cm.apdu_status_length;
		cn_iocr->eddx_data_status_offset = ar_iocr->cm.edd_ds_offset;
		cn_iocr->eddx_data_offset        = EDD_DATAOFFSET_INVALID; /* sanity */

		cn_iocr->eddi_cw_offset          = ar_iocr->cm.edd_cw_offset; /* AP00618087: additional infos for optimized io-update */
		cn_iocr->eddi_apdu_status_offset = ar_iocr->cm.edd_as_offset; /* AP00630092: pass pointer to APDUStatus to cm-user */
		cn_iocr->eddi_buffer_properties  = (ar_iocr->cm.is_consumer ? EDD_CONS_BUFFER_PROP_IRTE_IMG_INVALID : EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID); /* sanity */

		cn_iocr->eddx_cr_number          = EDD_CRNUMBER_UNDEFINED; /* sanity */
		cn_iocr->eddp_forward_mode       = EDD_FORWARDMODE_UNDEFINED; /* sanity */
		cn_iocr->eddp_ct_outdatastart    = 0; /* sanity */

#if 1///HERA hack
		if( cm_edd_is_HERA(argr->ar_com.channel) ) {

			/* temporary solution: use "eddi_cw_offset" as is-DFP flag*/
			cn_iocr->eddi_cw_offset = (ar_iocr->cm.subframe_data != 0 ? 1 : 0);

			if( ar_iocr->cm.subframe_data != 0 ) { /* is-DFP */

				if( ar_iocr->cm.is_consumer ) {
					// SeqID(2) + Subframe::C_SDU(io_buffer_size) + SFPosLenCcDs(4) + SeqID(2)
					// HERA-ALLOC --> adds SeqID at the front and SFPosLenCcDs+SeqID at the end
					// CM ----------> adds nothing to buffer size
					// HERA-COPY ---> copies Subframe::C_SDU, no HERA-specific offset-adjustments necessary
				}
				else {
					// SeqID(2) + DS(1) + SFPosLenCcDs(4) + Subframe::C_SDU(io_buffer_size) + SFCRC(16) + TERM(2) + SeqID(2)
					// HERA-ALLOC --> adds SeqID+DS+SFPosLenCcDs at the front and SFCRC+TERM+SeqID at the end
					// CM ----------> adds nothing to buffer size
					// HERA-COPY ---> copies Subframe::C_SDU, no HERA-specific offset-adjustments necessary
				}
			}
			else { /* is-NORMAL */

				if( ar_iocr->cm.is_consumer ) {
					// SeqID(2) + C_SDU(io_buffer_size) + APDU_Entry(4) + SeqID(2)
					// HERA-ALLOC --> adds SeqID at the front and APDU_Entry+SeqID at the end
					// CM ----------> adds nothing to buffer size
					// HERA-COPY ---> copies Subframe::C_SDU+APDU_Entry, no HERA-specific offset-adjustments necessary

					// note: CM-SV could add APDU_Entry, however ARCB_HELPER cannot do it.
				}
				else {
					// SeqID(2) + DS(1) + C_SDU(io_buffer_size) + SeqID(2)
					// HERA-ALLOC --> adds SeqID+DS at the front and SeqID at the end
					// CM ----------> adds nothing to buffer size
					// HERA-COPY ---> copies C_SDU, no HERA-specific offset-adjustments necessary
				}
			}
		}
#endif

		/***/

		ar_connect->nr_of_iocrs += 1;
	}
	}

	/***/

	if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_AR_PROBLEM) != 0 ) {

		if( CM_SV_AR_IS_ADVANCED(argr) ) {

			if( !CM_SV_AR_IS_RTCLASS3(argr) && (argr->cm.sv.ar_flags & CM_SV_AR_FL_PD_ATTACHED) != 0 ) {

				/* ok */
			}
			else {

				/* IOCRs were not added and the parameters eddi_cw_offset, etc. are invalid */

				ar_connect->send_clock_31250ns = 0;

				ar_connect->nr_of_iocrs = 0;
			}
		}
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		ar_connect->sr_firstAR = (cm_sv_arset_is_first(argr) ? CM_SV_SR_FIRST_AR_YES : CM_SV_SR_FIRST_AR_NO);
	}

	/***/

	event = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_CONNECT_IND);

	event->args.sv.ar_event->u.sv.ar_connect = ar_connect;

	return event;
}

/*===========================================================================*/
/*=== AR CONNECT UPDATE =====================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_connect_update(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_connect
) {
	CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev/ar(%u/%u) nr_of_iocrs(%u) ARVendorBlock_cnt(%u)"
		, argr->device_nr, argr->ar_nr
		, ar_connect->nr_of_iocrs, ar_connect->ARVendorBlock_cnt
		);

	{
	LSA_UINT16 idx;

	for( idx = 0; idx < ar_connect->nr_of_iocrs; ++idx ) {

		CM_UPPER_SV_AR_CONNECT_IOCR_PTR_TYPE cn_iocr = &ar_connect->iocr[idx];

		CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr = cm_ar_graph_find_io_cr(argr, cn_iocr->iocr_ref);

		if( cm_is_null(ar_iocr) ) {

			return LSA_FALSE; /* the cm-user fiddled with the RQB */
		}

		CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "dev/ar(%u/%u) ref(%u) frame_id(0x%04x) iocr_user_id(0x%08x) cycle_phase(%u -> %u)"
			, argr->device_nr, argr->ar_nr
			, ar_iocr->iocr_ref
			, ar_iocr->frame_id
			, cn_iocr->iocr_user_id.void_ptr
			, ar_iocr->cm.adpt.phase, cn_iocr->cycle_phase
			);

		/* update */

		ar_iocr->cm.sv.iocr_user_id = cn_iocr->iocr_user_id;

		ar_iocr->cm.adpt.phase = cn_iocr->cycle_phase; /* AP00856570 */

		ar_iocr->cm.edd_data_offset     = cn_iocr->eddx_data_offset;
		ar_iocr->cm.edd_properties      = cn_iocr->eddi_buffer_properties;
		ar_iocr->cm.edd_cr_number       = cn_iocr->eddx_cr_number;
		ar_iocr->cm.edd_forward_mode    = cn_iocr->eddp_forward_mode;
		ar_iocr->cm.edd_ct_outdatastart = cn_iocr->eddp_ct_outdatastart;
	}
	}

	/***/

	{
	LSA_UINT32 idx;

	for( idx = 0; idx < ar_connect->ARVendorBlock_cnt; ++idx ) {

		LSA_UINT8 *dat = (LSA_UINT8 *)&ar_connect->ARVendorBlock_ptr[ ar_connect->ARVendorBlock_ptr[idx] ];

		LSA_UINT16 len = CM_GET16_NTOH(dat, 2);
		LSA_UINT16 asi = CM_GET16_NTOH(dat, 6);
		LSA_UINT32 api = CM_GET32_NTOH(dat, 8);

		CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend = cm_ar_graph_find_arvendor(argr, api, asi);

		if(
			cm_is_null(ar_vend)
			|| ar_vend->res_data_len != 0
			|| CM_GET16_NTOH(dat, 0) != CM_BLOCK_TYPE_ARVENDORBLOCK_RSP
			|| 2+2 + len > CM_SV_ARVENDORBLOCK_RES_MAXSIZE /* note: no min-size check */
		) {

			/* a user-bug, ignore (the controller will complain) */
		}
		else {

			len += 2+2;

			ar_vend->res_data_len = len;

			CM_MEMCPY(ar_vend->data + ar_vend->req_data_len, dat, len);
		}
	}
	}

	/***/

	return LSA_TRUE; /* ok */
}

/*===========================================================================*/
/*=== AR OWNERSHIP ALLOC ====================================================*/
/*===========================================================================*/

CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE
cm_sv_ar_ownership_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_UINT32 alloc_size;
	CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership;

	/***/

	alloc_size = sizeof(*ar_ownership);

	if( argr->cm.sv.nr_of_submodules == 0 ) {

		CM_FATAL(); /* a bug, easy-supervisor should not come here */
	}
	else {

		alloc_size += (argr->cm.sv.nr_of_submodules - 1) * sizeof(ar_ownership->elem[0]);
	}

	/***/

	ar_ownership = (CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE)cm_sv_upper_mem_alloc(argr->ar_com.channel, alloc_size);

	if( cm_is_null(ar_ownership) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) could not cm_sv_ar_ownership_alloc"
			, argr->device_nr, argr->ar_nr
			);
	}
	else {

		ar_ownership->nr_of_elems = 0; /* sanity */
	}

	/***/

	return ar_ownership;
}

/*===========================================================================*/
/*=== AR OWNERSHIP BUILD DESCRIPTION ========================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_ar_ownership_build_descr(
	CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE elem,
	LSA_UINT16 dir,
	CM_AR_GRAPH_SUBMODULE_CONST_PTR_TYPE ar_sub,
	CM_LIST_ENTRY_CONST_PTR_TYPE ar_iocr_list /* TIA 1127383 (N-IOCRs) */
) {
	CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_DESCR_PTR_TYPE descr = &elem->descr[dir];
	CM_AR_GRAPH_IO_CR_PTR_TYPE data_iocr;
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocs_iocr;
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE ar_iodt;
	CM_AR_GRAPH_IOCS_PTR_TYPE ar_iocs;

	/***/

	if( ar_sub->descr[dir].data_length == 0xFFFF ) { /* invalid, see cm_ar_graph_insert_submodule() */

		descr->data_description = CM_SUB_DATA_DESCR_TYPE_RESERVED_0; /* invalid */

		descr->data_length = 0;
		descr->iops_length = 0;
		descr->iocs_length = 0;
	}
	else { /* valid */

		descr->data_description = ar_sub->descr[dir].data_description;

		descr->data_length = ar_sub->descr[dir].data_length;
		descr->iops_length = ar_sub->descr[dir].iops_length;
		descr->iocs_length = ar_sub->descr[dir].iocs_length;
	}

	/***/

	data_iocr = LSA_NULL;
	iocs_iocr = LSA_NULL;

	ar_iodt = LSA_NULL;
	ar_iocs = LSA_NULL;

	if( descr->data_description != CM_SUB_DATA_DESCR_TYPE_RESERVED_0 ) { /* valid */

		CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

		for( CmListEach(ar_iocr, ar_iocr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

			CM_AR_GRAPH_IOAPI_PTR_TYPE ar_ioapi = cm_ar_graph_find_ioapi(ar_iocr, elem->api);

			if( cm_is_not_null(ar_ioapi) ) {

				if( cm_is_null(data_iocr) ) {

					if(
						(dir == 0/*input*/ && ar_iocr->iocr_type == CM_IOCR_TYPE_INPUT)
						||
						(dir == 1/*output*/ && ar_iocr->iocr_type == CM_IOCR_TYPE_OUTPUT)
					) {

						ar_iodt = cm_ar_graph_find_iodata(ar_ioapi, elem->slot_nr, elem->subslot_nr);

						if( cm_is_not_null(ar_iodt) ) {

							data_iocr = ar_iocr; /* found */
						}
					}
				}

				if( cm_is_null(iocs_iocr) ) {

					if(
						(dir == 0/*input*/ && ar_iocr->iocr_type == CM_IOCR_TYPE_OUTPUT)
						||
						(dir == 1/*output*/ && ar_iocr->iocr_type == CM_IOCR_TYPE_INPUT)
					) {

						ar_iocs = cm_ar_graph_find_iocs(ar_ioapi, elem->slot_nr, elem->subslot_nr);

						if( cm_is_not_null(ar_iocs) ) {

							iocs_iocr = ar_iocr; /* found */
						}
					}
				}
			}
		}

		if( cm_is_null(data_iocr) || cm_is_null(ar_iodt) || cm_is_null(iocs_iocr) || cm_is_null(ar_iocs) ) {
			CM_FATAL(); /* a bug, see cm_ar_graph_check_consistency() */
		}
	}

	/***/

	if( cm_is_not_null(data_iocr) && cm_is_not_null(ar_iodt) ) {

		descr->data_user_id = data_iocr->cm.sv.iocr_user_id;
		descr->data_offset  = ar_iodt->frame_offset;

		if( data_iocr->cm.subframe_data != 0 ) {

			if( descr->data_offset < data_iocr->cm.subframe_offset ) {

				CM_FATAL(); /* a bug */
			}
			else {

				descr->data_offset -= data_iocr->cm.subframe_offset;
			}
		}
		else {

			if( descr->data_offset < data_iocr->cm.partial_offset ) {

				CM_FATAL(); /* a bug */
			}
			else {

				descr->data_offset -= data_iocr->cm.partial_offset;
			}
		}

		/* not: descr->data_offset += data_iocr->cm.apdu_status_length -- see cm_sv_ar_connect_build_notnull() */
	}
	else {

		descr->data_user_id.void_ptr = 0;
		descr->data_offset = 0;
	}

	if( cm_is_not_null(iocs_iocr) && cm_is_not_null(ar_iocs) ) {

		descr->iocs_user_id = iocs_iocr->cm.sv.iocr_user_id;
		descr->iocs_offset  = ar_iocs->frame_offset;

		if( iocs_iocr->cm.subframe_data != 0 ) {

			if( descr->iocs_offset < iocs_iocr->cm.subframe_offset ) {

				CM_FATAL(); /* a bug */
			}
			else {

				descr->iocs_offset -= iocs_iocr->cm.subframe_offset;
			}
		}
		else {

			if( descr->iocs_offset < iocs_iocr->cm.partial_offset ) {

				CM_FATAL(); /* a bug */
			}
			else {

				descr->iocs_offset -= iocs_iocr->cm.partial_offset;
			}
		}

		/* not: descr->iocs_offset += iocs_iocr->cm.apdu_status_length -- see cm_sv_ar_connect_build_notnull() */
	}
	else {

		descr->iocs_user_id.void_ptr = 0;
		descr->iocs_offset = 0;
	}
}

/*===========================================================================*/
/*=== AR OWNERSHIP BUILD ====================================================*/
/*===========================================================================*/

CM_UPPER_RQB_PTR_TYPE
cm_sv_ar_ownership_build(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 ar_context
) {
	CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership = argr->cm.sv.ar_ownership_mem;

	/***/

	if( cm_is_null(ar_ownership) ) {

		CM_FATAL(); /* a bug */
	}

	/***/

	ar_ownership->ar_context = ar_context;

	ar_ownership->set_mrp_off = CM_SET_MRP_OFF_NO; /* AP00824058 */

	ar_ownership->nr_of_elems = 0;

	{
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su = ar_sub->sv.real_su;

				LSA_UINT16 cand_session_key; /* see table in the documentation */

				if( ar_context == CM_SV_AR_CONTEXT_CONN ) {

					/* note: initially ar_sub->sv.sm_flags = CM_SV_AR_SMFL_WRONG, see cm_ar_graph_insert_submodule() */

					if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_AR_PROBLEM) != 0 ) {

						cand_session_key = 0; /* make all submodules so-locked */
					}
					else if( su->cand[0].ar_nr == argr->ar_nr ) { /* is head-of-queue */

						if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) { /* a pdev-submodule */

							if( cm_sv_ar_is_pdev_owner(argr) ) {

								cand_session_key = argr->session_key;
							}
							else {

								cand_session_key = 0;
							}

							if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, su->subslot_nr) /* TIA 1449259 */
								&& argr->cm.sv.nr_of_pdev_submodules == 1 /* i.e. only interface, no ports */
							) {

								cand_session_key = 0;
							}
						}
						else { /* a normal subslot */

							cand_session_key = argr->session_key;
						}

						/***/

						if( cm_is_not_null(argr->cm.sv.arset) ) {

							if( ! cm_sv_arset_is_first(argr) ) {

								if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_CIR) != 0 ) {

									/* CIR: offer ownership, however still kind of "pulled", see cm_sv_marshal_module_diff_block() */
								}
								else {

									cand_session_key = 0; /* only the temporally first AR parameterizes */
								}
							}
						}
					}
					else { /* not head-of-queue */

						cand_session_key = 0;

#if CM_CFG_MAX_SV_ARS >= 2 /* cand[1] is within bounds */

						/*
						 * supervisor-takeover is done in three steps
						 * 1st step) ar-ownership.ind: offer ownership if possible
						 * 2nd step) user sees "curr-owner -> new-owner", sets IOXS=BAD in the curr-owner, and assigns the new owner
						 * 3rd step) ar-ownership.rsp: complete the takeover
						 *
						 * compare a "physical pull" (trigger is a hardware event)
						 * with this "logical pull" (trigger is the ownership.ind)
						 * in both cases the user starts with IOXS=BAD
						 * and then telling CM that something has happened (remove.req / ownership.rsp)
						 */

						if(
							su->cand[1].ar_nr == argr->ar_nr           /* next in line */
							&& su->cand[1].is_supervisor               /* and is supervisor */
							&& ! su->cand[0].no_takeover               /* and takeover allowed */
							&& ! CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) /* and not a pdev-submodule */
						) {

							cand_session_key = argr->session_key; /* takeover */
						}
#endif
					}
				}
				else if( ar_context == CM_SV_AR_CONTEXT_DISC ) {

					if( su->owner_ar_nr == argr->ar_nr ) { /* is owner */

						cand_session_key = argr->session_key;
					}
					else { /* not owner */

						cand_session_key = 0;
					}
				}
				else if( ar_context == CM_SV_AR_CONTEXT_PLUG ) {

					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_OSHP) != 0 ) {

						ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_OSHP; /* done */

						/***/

						if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_AR_PROBLEM) != 0 ) {

							cand_session_key = 0; /* make all submodules so-locked */
						}
						else if( CM_SV_SUBSLOT_IS_PLUGGED(su) ) {

							if( su->cand[0].ar_nr == argr->ar_nr ) { /* is head-of-queue */

								if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) { /* a pdev-submodule */

									if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PBE) != 0 ) {

										cand_session_key = argr->session_key;
									}
									else {

										cand_session_key = argr->session_key; /* TIA 954400 */
									}

									if( CM_SV_SUBSLOT_IS_PDEV_LOCAL(argr->cm.sv.dev, su->subslot_nr) ) { /* TIA 1449259 */

										LSA_UINT16 slot_nr;
										LSA_UINT16 subslot_nr;
										LSA_UINT32 mod_ident;
										LSA_UINT32 sub_ident;
										LSA_UINT8  im0_bits;
										LSA_UINT16 pdev_properties;
										CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub_if;

										LSA_BOOL port_exists = cm_pdsv_port_from_id(argr->ar_com.channel, 0 /*interface*/, &slot_nr, &subslot_nr, &mod_ident, &sub_ident, &im0_bits, &pdev_properties);

										if( port_exists ) { /* always true */

											cm_ar_graph_find_api_slot_subslot(argr, 0, slot_nr, subslot_nr, LSA_NULL, LSA_NULL, &ar_sub_if);

											if( cm_is_null(ar_sub_if) || ar_sub_if->sv.real_su->owner_ar_nr != argr->ar_nr ) {

												cand_session_key = 0;
											}
										}

									}
								}
								else { /* a normal subslot */

									cand_session_key = argr->session_key;
								}
							}
							else { /* not head-of-queue */

								cand_session_key = 0;
							}
						}
						else { /* pulled */

							cand_session_key = 0;
						}
					}
					else { /* idle */

						cand_session_key = 0;
					}
				}
				else if( ar_context == CM_SV_AR_CONTEXT_PULLPDEV ) {

					if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_PULLPDEV) != 0 ) {

						ar_sub->sv.sm_flags &= ~CM_SV_AR_SMFL_PULLPDEV; /* done */

						if( (argr->cm.sv.ar_flags & CM_SV_AR_FL_AR_PROBLEM) != 0 ) {

							cand_session_key = 0; /* make all submodules so-locked */
						}
						else if( su->owner_ar_nr == argr->ar_nr ) { /* is owner */

							cand_session_key = argr->session_key;
						}
						else {

							cand_session_key = 0;
						}
					}
					else {

						cand_session_key = 0;
					}
				}
				else {

					cand_session_key = 0; /* unreachable */
				}

				/***/

				if( cand_session_key != 0 || ar_context == CM_SV_AR_CONTEXT_CONN ) {

					CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE elem = &ar_ownership->elem[ar_ownership->nr_of_elems];

					elem->api        = ar_ap->api;
					elem->slot_nr    = ar_mod->slot_nr;
					elem->subslot_nr = ar_sub->subslot_nr;

					elem->mod_ident      = ar_mod->soll.mod_ident;
					elem->mod_properties = ar_mod->soll.mod_properties;

					elem->sub_ident      = ar_sub->soll.sub_ident;
					elem->sub_properties = ar_sub->soll.sub_properties;

					/***/

					{
					LSA_UINT16 dir;

					for( dir = 0; dir < 2; ++dir ) {

						cm_sv_ar_ownership_build_descr(elem, dir, ar_sub, &argr->io_cr_list);

						/***/

						if( ar_sub->inp_via_mcr ) { /* m-provider (inputs are written to m-provider cr) */

							/* not yet (search for AP00774495) */
						}

						if( ar_sub->out_via_mcr ) { /* m-consumer (outputs are read from m-consumer cr) */

							/* not yet (search for AP00774501) */
						}
					}
					}

					/***/

					/* AP00881547: preset the "sub_state" to help the cm-user implementing pre-AP00826555 behaviour */
					/* note: must not use cm_sv_get_submodule_state() here */

					if( cm_sv_get_submodule_state_ident(ar_mod, ar_sub) == CM_SUB_STATE_IDENT_WRONG ) {

						elem->is_wrong = CM_SV_IS_WRONG_YES; /* must check: wrong or subst? */
					}
					else {

						elem->is_wrong = CM_SV_IS_WRONG_NO; /* need not check */
					}

					/***/

					elem->owner_session_key = cand_session_key;

					/***/

					CM_SV_TRACE_07(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "dev/ar(%u/%u) ap/sl/su(%u/%u/%05u) OSHP-IND: offered owner_session_key(0x%x) is_wrong(%u)"
						, argr->device_nr, argr->ar_nr
						, elem->api, elem->slot_nr, elem->subslot_nr
						, elem->owner_session_key
						, elem->is_wrong
						);

					/***/

					ar_ownership->nr_of_elems += 1;
				}
			}
		}
	}
	}

	CM_ASSERT(ar_ownership->nr_of_elems <= argr->cm.sv.nr_of_submodules);

	/***/

	if( ar_ownership->nr_of_elems == 0 ) {

		return LSA_NULL;
	}
	else {

		CM_UPPER_RQB_PTR_TYPE event = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_OWNERSHIP_IND);

		event->args.sv.ar_event->u.sv.ar_ownership = ar_ownership;

		return event;
	}
}

/*===========================================================================*/
/*=== AR PRM-END ALLOC ======================================================*/
/*===========================================================================*/

CM_UPPER_SV_AR_PRM_END_PTR_TYPE
cm_sv_ar_prm_end_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_UINT32 alloc_size;
	CM_UPPER_SV_AR_PRM_END_PTR_TYPE ar_prm_end;

	/***/

	alloc_size = sizeof(*ar_prm_end);

	if( argr->cm.sv.nr_of_submodules == 0 ) {

		CM_FATAL(); /* a bug, easy-supervisor should not come here */
	}
	else {

		alloc_size += (argr->cm.sv.nr_of_submodules - 1) * sizeof(ar_prm_end->elem[0]);
	}

	/***/

	ar_prm_end = (CM_UPPER_SV_AR_PRM_END_PTR_TYPE)cm_sv_upper_mem_alloc(argr->ar_com.channel, alloc_size);

	if( cm_is_null(ar_prm_end) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) could not cm_sv_ar_prm_end_alloc"
			, argr->device_nr, argr->ar_nr
			);
	}
	else {

		ar_prm_end->nr_of_elems = 0; /* sanity */
	}

	/***/

	return ar_prm_end;
}

/*===========================================================================*/
/*=== AR IN-DATA ALLOC ======================================================*/
/*===========================================================================*/

CM_UPPER_SV_AR_IN_DATA_PTR_TYPE
cm_sv_ar_in_data_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_UINT32 alloc_size;
	CM_UPPER_SV_AR_IN_DATA_PTR_TYPE ar_in_data;

	/***/

	alloc_size = sizeof(*ar_in_data);

	if( argr->cm.sv.nr_of_submodules == 0 ) {

		CM_FATAL(); /* a bug, easy-supervisor should not come here */
	}
	else {

		alloc_size += (argr->cm.sv.nr_of_submodules - 1) * sizeof(ar_in_data->elem[0]);
	}

	/***/

	ar_in_data = (CM_UPPER_SV_AR_IN_DATA_PTR_TYPE)cm_sv_upper_mem_alloc(argr->ar_com.channel, alloc_size);

	if( cm_is_null(ar_in_data) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) could not cm_sv_ar_in_data_alloc"
			, argr->device_nr, argr->ar_nr
			);
	}
	else {

		ar_in_data->nr_of_elems = 0; /* sanity */
	}

	/***/

	return ar_in_data;
}

/*===========================================================================*/
/*=== AR RIR ALLOC ==========================================================*/
/*===========================================================================*/

CM_UPPER_SV_AR_RIR_PTR_TYPE
cm_sv_ar_rir_alloc(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_UINT32 alloc_size;
	CM_UPPER_SV_AR_RIR_PTR_TYPE ar_rir;
	LSA_UINT16 nr_of_elems;

	/***/

	alloc_size = sizeof(*ar_rir);

	CmListLength(&argr->io_cr_list,&nr_of_elems);

	if( nr_of_elems == 0 ) {

		CM_FATAL(); /* a bug, easy-supervisor should not come here */
	}
	else {

		alloc_size += (nr_of_elems - 1) * sizeof(ar_rir->elem[0]);
	}

	/***/

	ar_rir = (CM_UPPER_SV_AR_RIR_PTR_TYPE)cm_sv_upper_mem_alloc(argr->ar_com.channel, alloc_size);

	if( cm_is_null(ar_rir) ) {

		CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "dev/ar(%u/%u) could not cm_sv_ar_rir_alloc"
			, argr->device_nr, argr->ar_nr
			);
	}
	else {

		ar_rir->nr_of_elems = 0; /* sanity */
	}

	/***/

	return ar_rir;
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
