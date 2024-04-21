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
/*  F i l e               &F: cm_sv7.c                                  :F&  */
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
/*  - internal alarm-send                                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7700
#define CM_MODULE_ID       7700

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== CONSISTENCY CHECK "SYSRED" ============================================*/
/*===========================================================================*/

LSA_UINT32
cm_sv_srd_consistency_check_argr(
	CM_AR_GRAPH_PTR_TYPE argr, /* the new AR */
	CM_AR_GRAPH_PTR_TYPE argr_ex, /* the existing AR */
	LSA_BOOL is_cir
) {
	LSA_UINT32 length, length_ex; /* used to compare lengths of lists */

	/*
	 * AP01200252, attachment: 2011-07-07__SRD__PDU-Checking-rules.doc
	 * - the alarm-data-length has to be the same (no dependency on which AR is "first")
	 * - the alarm-property "prio" has to be the same (no dependency on which AR is "first")
	 * - even if CIR, the expected-configs must match (change is done in two steps: remove old, add new)
	 * - the other checks are actually non-functional (e.g., same buffer-layout for single provider-buffer, an optimization for non-CIR)
	 */

	/* ARBlockReq */

	/*
	argr->ar.ar_type ... checked in caller
	argr->ar.ar_uuid ... checked in caller
	*/

	if( argr->ar.ar_properties != argr_ex->ar.ar_properties ) {

		return CM_PNIO_FAULTY_ARBlockReq(9);
	}

	if( argr->ar.cmi_timeout100ms != argr_ex->ar.cmi_timeout100ms ) {

		return CM_PNIO_FAULTY_ARBlockReq(10);
	}

	/* IOCRBlockReq */

	CmListLength(&argr->io_cr_list, &length);
	CmListLength(&argr_ex->io_cr_list, &length_ex);

	if( length != 2 || length != length_ex ) {

		return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
	}

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

	for( CmListEach(iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

		CM_AR_GRAPH_IO_CR_PTR_TYPE iocr_ex;

		for( CmListEach(iocr_ex, &argr_ex->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

			if( iocr->iocr_type == iocr_ex->iocr_type ) { /* note: "iocr_type" is the only match-criterion */

				break;
			}
		}

		if( cm_is_null(iocr_ex) ) {

			return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
		}

		if( ! (iocr->iocr_type == CM_IOCR_TYPE_INPUT || iocr->iocr_type == CM_IOCR_TYPE_OUTPUT) ) {

			return CM_PNIO_FAULTY_IOCRBlockReq(4); /* unreachable (length = 2, 1 input and 1 output) */
		}

		if( iocr->ethertype != iocr_ex->ethertype ) { /* ethertype == LT */

			return CM_PNIO_FAULTY_IOCRBlockReq(6);
		}

		if( iocr->iocr_properties != iocr_ex->iocr_properties ) {

			return CM_PNIO_FAULTY_IOCRBlockReq(7);
		}

		if( iocr->send_clock != iocr_ex->send_clock ) {

			return CM_PNIO_FAULTY_IOCRBlockReq(10);
		}

		if( is_cir ) {

			/* may change data_length, reduction_ratio, phase, data_hold_factor, submodules */
		}
		else {

			if( iocr->data_length != iocr_ex->data_length ) {

				return CM_PNIO_FAULTY_IOCRBlockReq(8);
			}

			if( iocr->reduction_ratio != iocr_ex->reduction_ratio ) {

				return CM_PNIO_FAULTY_IOCRBlockReq(11);
			}

			/* note: phase may be different (see AP01200252) */

			if( iocr->data_hold_factor != iocr_ex->data_hold_factor ) {

				return CM_PNIO_FAULTY_IOCRBlockReq(16);
			}

			/***/

			/* NumberofAPIs */

			CmListLength(&iocr->related_ap_list, &length);
			CmListLength(&iocr_ex->related_ap_list, &length_ex);

			if( length != length_ex ) {

				return CM_PNIO_FAULTY_IOCRBlockReq(19);
			}

			{
			CM_AR_GRAPH_IOAPI_PTR_TYPE iocr_ap;

			for( CmListEach(iocr_ap, &iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE) ) {

				CM_AR_GRAPH_IOAPI_PTR_TYPE iocr_ap_ex = cm_ar_graph_find_ioapi(iocr_ex, iocr_ap->api);

				if( cm_is_null (iocr_ap_ex) ) {

					return CM_PNIO_FAULTY_IOCRBlockReq(20);
				}

				/* NumberOfIODataObjects */

				CmListLength(&iocr_ap->related_iodata_objects_list, &length);
				CmListLength(&iocr_ap_ex->related_iodata_objects_list, &length_ex);

				if( length != length_ex ) {

					return CM_PNIO_FAULTY_IOCRBlockReq(21);
				}

				{
				CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE iocr_iodata;

				for( CmListEach(iocr_iodata, &iocr_ap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE) ) {

					CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE iocr_iodata_ex = cm_ar_graph_find_iodata(iocr_ap_ex, iocr_iodata->slot_nr, iocr_iodata->subslot_nr);

					if( cm_is_null(iocr_iodata_ex) ) {

						return CM_PNIO_FAULTY_IOCRBlockReq(23);
					}

					if( iocr_iodata->frame_offset != iocr_iodata_ex->frame_offset ) {

						return CM_PNIO_FAULTY_IOCRBlockReq(24);
					}
				}
				}

				/* NumberOfIOCS */

				CmListLength(&iocr_ap->related_iocs_list, &length);
				CmListLength(&iocr_ap_ex->related_iocs_list, & length_ex);

				if( length != length_ex ) {

					return CM_PNIO_FAULTY_IOCRBlockReq(25);
				}

				{
				CM_AR_GRAPH_IOCS_PTR_TYPE iocr_iocs;

				for( CmListEach(iocr_iocs, &iocr_ap->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE) ) {

					CM_AR_GRAPH_IOCS_PTR_TYPE iocr_iocs_ex = cm_ar_graph_find_iocs(iocr_ap_ex, iocr_iocs->slot_nr, iocr_iocs->subslot_nr);

					if( cm_is_null(iocr_iocs_ex) ) {

						return CM_PNIO_FAULTY_IOCRBlockReq(27);
					}

					if( iocr_iocs->frame_offset != iocr_iocs_ex->frame_offset ) {

						return CM_PNIO_FAULTY_IOCRBlockReq(28);
					}
				}
				}
			}
			}
		}
	}
	}

	/* AlarmCRBlockReq */

	if( cm_is_null(argr->alarm_cr) || cm_is_null(argr_ex->alarm_cr) ) { /* always false */

		return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
	}
	else {

		CM_AR_GRAPH_ALARM_CR_PTR_TYPE alarm_cr    = argr->alarm_cr;
		CM_AR_GRAPH_ALARM_CR_PTR_TYPE alarm_cr_ex = argr_ex->alarm_cr;

		if( alarm_cr->alcr_type != alarm_cr_ex->alcr_type ) {

			return CM_PNIO_FAULTY_AlarmCRBlockReq(4);
		}

		if( alarm_cr->ethertype != alarm_cr_ex->ethertype ) { /* ethertype == LT */

			return CM_PNIO_FAULTY_AlarmCRBlockReq(5);
		}

		if( alarm_cr->alcr_properties != alarm_cr_ex->alcr_properties ) {

			return CM_PNIO_FAULTY_AlarmCRBlockReq(6);
		}

		if( is_cir ) {

			/* may change timeout100ms (V6.1i1.33, TIA 2032176) and retries */
		}
		else {

			if( alarm_cr->timeout100ms != alarm_cr_ex->timeout100ms ) {

				return CM_PNIO_FAULTY_AlarmCRBlockReq(7);
			}

			if( alarm_cr->retries != alarm_cr_ex->retries ) {

				return CM_PNIO_FAULTY_AlarmCRBlockReq(8);
			}
		}

		if( alarm_cr->max_alarm_data_len != alarm_cr_ex->max_alarm_data_len ) {

			return CM_PNIO_FAULTY_AlarmCRBlockReq(10);
		}
	}

	/* ExpectedSubmoduleBlockReq */

	if( argr->cm.sv.nr_of_pdev_submodules != argr_ex->cm.sv.nr_of_pdev_submodules ) {

		/* note: CIR from n to 0 would be possible, but the opposite is not (pdev-submodules cannot be plugged) */

		/* note: an SR-AR always contains the pdev-submodules (TIA 1012004) */

		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "not same number of pdev-submodules (not allowed for CIR either)"
			);

		return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(9);
	}

	if( argr->cm.sv.nr_of_submodules != argr_ex->cm.sv.nr_of_submodules && !is_cir ) {

		return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(9);
	}
	else {

		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

		for( CmListEach(ar_ap, &argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE) ) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			for( CmListEach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE) ) {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				for( CmListEach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ) {

					CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub_ex;

					cm_ar_graph_find_api_slot_subslot(argr_ex, ar_ap->api, ar_mod->slot_nr, ar_sub->subslot_nr, LSA_NULL, LSA_NULL, &ar_sub_ex);

					if( cm_is_null(ar_sub_ex) ) {

						if( is_cir ) {

							ar_sub->sv.sm_flags |= CM_SV_AR_SMFL_CIR; /* do plug-alarm after transition to primary */
						}
						else {

							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(10);
						}
					}
					else {

						/* mod */
						if( ar_mod->soll.mod_ident != ar_sub_ex->mod->soll.mod_ident ) {

							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(7);
						}

						if( ar_mod->soll.mod_properties != ar_sub_ex->mod->soll.mod_properties ) {

							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(8);
						}

						/* submod */
						if( ar_sub->soll.sub_ident != ar_sub_ex->soll.sub_ident ) {

							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(11);
						}

						if( ar_sub->soll.sub_properties != ar_sub_ex->soll.sub_properties ) {

							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(12);
						}

						{
						LSA_UINT32 i;
						for( i = 0; i < 2; i++ ) {

							if( ar_sub->descr[i].data_length != ar_sub_ex->descr[i].data_length ) {

								return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(14);
							}

							if( ar_sub->descr[i].data_length != 0xFFFF ) { /* i.e., valid */

								if( ar_sub->descr[i].data_description != ar_sub_ex->descr[i].data_description ) {

									return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(13);
								}

								if( ar_sub->descr[i].iops_length != ar_sub_ex->descr[i].iops_length ) {

									return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(15);
								}

								if( ar_sub->descr[i].iocs_length != ar_sub_ex->descr[i].iocs_length ) {

									return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(16);
								}
							}
						}
						}
					}

				}
			}
		}
	}

	/* PrmServerBlock -- not yet supported */

	/* MCRBlockReq */

	if ( !CmListIsEmpty(&argr->mcr_list) || !CmListIsEmpty(&argr_ex->mcr_list) ) { /* always false */

		return CM_PNIO_FAULTY_Connect__CMRPC_WrongBlockCount();
	}

	/* ARRPCBlockReq -- not yet supported (and no fields to check) */

	/* IRInfoBlock */

	if( cm_is_not_null(argr->ir_info) || cm_is_not_null(argr_ex->ir_info) ) { /* always false */

		return CM_PNIO_FAULTY_Connect__CMRPC_WrongBlockCount();
	}

	/* SRInfoBlock */

	if( cm_is_null(argr->sr_info) || cm_is_null(argr_ex->sr_info) ) { /* always false */

		return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
	}
	else {

		CM_AR_GRAPH_SR_INFO_PTR_TYPE sr_info	= argr->sr_info;
		CM_AR_GRAPH_SR_INFO_PTR_TYPE sr_info_ex = argr_ex->sr_info;

		if( is_cir ) {

			/* may change rdh_factor */
		}
		else {

			if( sr_info->rdh_factor != sr_info_ex->rdh_factor ) {

				return CM_PNIO_FAULTY_SRInfoBlock(4);
			}
		}

		if( sr_info->sr_properties != sr_info_ex->sr_properties ) {

			return CM_PNIO_FAULTY_SRInfoBlock(5);
		}
	}

	/* ARVendorBlockReq (AP01354956) */

	{
	CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend;
	CM_AR_GRAPH_ARVENDOR_PTR_TYPE ar_vend_ex;

	ar_vend    = CmListFirst(&argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE);
	ar_vend_ex = CmListFirst(&argr_ex->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE);

	while( cm_is_not_null(ar_vend) && cm_is_not_null(ar_vend_ex) ) {

		if(
			ar_vend->api != ar_vend_ex->api
			|| ar_vend->ap_structure_identifier != ar_vend_ex->ap_structure_identifier
			|| ar_vend->req_data_len != ar_vend_ex->req_data_len
		) {

			return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
		}
		else {

			if( CM_MEMCMP(ar_vend->data, ar_vend_ex->data, ar_vend->req_data_len) != 0 ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
			}
		}

		ar_vend    = CmListNext(&argr->arvendor_list, &ar_vend->link, CM_AR_GRAPH_ARVENDOR_PTR_TYPE);
		ar_vend_ex = CmListNext(&argr_ex->arvendor_list, &ar_vend_ex->link, CM_AR_GRAPH_ARVENDOR_PTR_TYPE);
	}

	if( cm_is_not_null(ar_vend) || cm_is_not_null(ar_vend_ex) ) {

		return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
	}
	}

	/* ARFSUBlock */

	{
	CM_SV_AR_REC_TYPE *rec    = &argr->cm.sv.rec;
	CM_SV_AR_REC_TYPE *rec_ex = &argr_ex->cm.sv.rec;

	if( cm_is_null(rec->ARFSUDataAdjust_ptr) && cm_is_null(rec_ex->ARFSUDataAdjust_ptr) ) {

		/* ok */
	}
	else if( cm_is_not_null(rec->ARFSUDataAdjust_ptr) && cm_is_not_null(rec_ex->ARFSUDataAdjust_ptr) ) {

		if( rec->ARFSUDataAdjust_len != rec_ex->ARFSUDataAdjust_len ) {

			return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
		}

		if( CM_MEMCMP(rec->ARFSUDataAdjust_ptr, rec_ex->ARFSUDataAdjust_ptr, rec->ARFSUDataAdjust_len) != 0 ) {

			return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
		}
	}
	else {

		return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
	}
	}

	/* RSInfoBlock */

	if( is_cir ) {

		/* rs_info blocks may differ, TIA 1678311 */
	}
	else {

		if( cm_is_null(argr->rs_info) && cm_is_null(argr_ex->rs_info) ) {

			/* ok */
		}
		else if( cm_is_not_null(argr->rs_info) && cm_is_not_null(argr_ex->rs_info) ) {

			CM_AR_GRAPH_RS_INFO_PTR_TYPE rs_info    = argr->rs_info;
			CM_AR_GRAPH_RS_INFO_PTR_TYPE rs_info_ex = argr_ex->rs_info;

			if( rs_info->rs_info_data_len != rs_info_ex->rs_info_data_len ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
			}

			if( CM_MEMCMP(rs_info->rs_info_data, rs_info_ex->rs_info_data, rs_info->rs_info_data_len) != 0 ) {

				return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
			}
		}
		else {

			return CM_PNIO_FAULTY_Connect__CMRPC_ARSetParameterConflictDuringConnectionEstablishment();
		}
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*===========================================================================*/
/*=== R1-BRIDGE DEVICE R1B INIT =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_r1b_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	dev->r1b.bridge_state = CM_SV_DEV_R1B_STATE_UNKNOWN;

	/***/

	dev->r1b.control.bridge_broken_rqb = LSA_NULL;
	dev->r1b.control.device_remove_rqb = LSA_NULL;

	/***/

	dev->r1b.send.resource = LSA_NULL;
	CmListInitialize(&dev->r1b.send.queue);

	/***/

	dev->r1b.upd.msg_begin_and_diag = LSA_NULL;
	dev->r1b.upd.msg_alarms         = LSA_NULL;
	dev->r1b.upd.msg_end            = LSA_NULL;

	/***/

	dev->r1b.proxy_connect_done = LSA_NULL;

	/***/

	dev->r1b.diag.msg_user_diag    = LSA_NULL;
	dev->r1b.diag.msg_pdev_diag    = LSA_NULL;

	CmListInitialize(&dev->r1b.diag.queue);
	dev->r1b.diag.current          = LSA_NULL;
	dev->r1b.diag.curr_elem_idx    = 0;
	dev->r1b.diag.update_pending   = LSA_FALSE;

	dev->r1b.diag.pdev_diag_posted = LSA_FALSE;
	dev->r1b.diag.pdev_diag_again  = LSA_FALSE;

	/***/

	dev->r1b.pdev_A.buffer     = LSA_NULL;

	CmListInitialize(&dev->r1b.pdev_A.queue);

	dev->r1b.pdev_A.pd         = LSA_NULL;
	dev->r1b.pdev_A.user_id    = 0;
	dev->r1b.pdev_A.wf_posted  = LSA_FALSE;

	/***/

	dev->r1b.pdev_B.buffer     = LSA_NULL;
	dev->r1b.pdev_B.rqb        = LSA_NULL;

	dev->r1b.pdev_B.br_next    = LSA_NULL;
	dev->r1b.pdev_B.pd         = LSA_NULL;
	dev->r1b.pdev_B.user_id    = 0;
	dev->r1b.pdev_B.wf_posted  = LSA_FALSE;

	/***/

	dev->r1b.stat_A.buffer     = LSA_NULL;
	CmListInitialize(&dev->r1b.stat_A.queue);

	dev->r1b.stat_A.curr       = LSA_NULL;
	dev->r1b.stat_A.wf_posted  = LSA_FALSE;

	/***/

	dev->r1b.stat_B.buffer     = LSA_NULL;
	dev->r1b.stat_B.iter       = LSA_NULL;

	dev->r1b.stat_B.br_next    = LSA_NULL;
	dev->r1b.stat_B.wf_posted  = LSA_FALSE;

	/***/

	{
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	r1pd_alarm->msg_mirror_set    = LSA_NULL;
	r1pd_alarm->msg_mirror_cancel = LSA_NULL;
	r1pd_alarm->msg_mirror_clr    = LSA_NULL;

	r1pd_alarm->wf_set_posted    = LSA_FALSE;
	r1pd_alarm->wf_cancel_posted = LSA_FALSE;
	r1pd_alarm->wf_clr_posted    = LSA_FALSE;

	r1pd_alarm->orig = LSA_NULL;
	r1pd_alarm->mirr = LSA_NULL;

	r1pd_alarm->orig_is_completed = LSA_FALSE;
	r1pd_alarm->mirr_is_completed = LSA_FALSE;
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE DEVICE R1B ALLOC ============================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_dev_r1b_alloc(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	dev->r1b.upd.msg_begin_and_diag = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1B_PREFIX_ALLOC_MAX);

	if( cm_is_null(dev->r1b.upd.msg_begin_and_diag) ) {

		return LSA_FALSE;
	}

	dev->r1b.upd.msg_alarms = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1B_UPD_ALARMS_MESSAGE_MAX);

	if ( cm_is_null(dev->r1b.upd.msg_alarms) ) {

		return LSA_FALSE;
	}

	dev->r1b.upd.msg_end = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_UPDATE_END);

	if( cm_is_null(dev->r1b.upd.msg_end) ) {

		return LSA_FALSE;
	}

	/***/

	dev->r1b.proxy_connect_done = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_CONNECT_DONE);

	if (cm_is_null(dev->r1b.proxy_connect_done)) {

		return LSA_FALSE;
	}

	/***/

	dev->r1b.diag.msg_user_diag = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1B_PREFIX_ALLOC_MAX);

	if( cm_is_null(dev->r1b.diag.msg_user_diag) ) {

		return LSA_FALSE;
	}

	dev->r1b.diag.msg_pdev_diag = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1B_PREFIX_ALLOC_MAX);

	if (cm_is_null(dev->r1b.diag.msg_pdev_diag)) {

		return LSA_FALSE;
	}

	/***/

	dev->r1b.pdev_A.buffer = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1B_PREFIX_ALLOC_MAX);

	if( cm_is_null(dev->r1b.pdev_A.buffer) ) {

		return LSA_FALSE;
	}

	/***/

	dev->r1b.pdev_B.buffer = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1B_PREFIX_ALLOC_MAX);

	if( cm_is_null(dev->r1b.pdev_B.buffer) ) {

		return LSA_FALSE;
	}

	dev->r1b.pdev_B.rqb = cm_upper_alloc_rqb(dev->channel);

	if( cm_is_null(dev->r1b.pdev_B.rqb) ) {

		return LSA_FALSE;
	}

	/***/

	dev->r1b.stat_A.buffer = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_GET_APDU_STATUS);

	if( cm_is_null(dev->r1b.stat_A.buffer) ) {

		return LSA_FALSE;
	}

	dev->r1b.stat_B.buffer = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_GET_APDU_STATUS_DONE);

	if( cm_is_null(dev->r1b.stat_B.buffer) ) {

		return LSA_FALSE;
	}

	{
	CM_LOCAL_MEM_PTR_TYPE local_mem;
	CM_SV_ITERATOR_PTR_TYPE iter_ptr;

	CM_ALLOC_LOCAL_MEM(&local_mem, sizeof(*iter_ptr));
	iter_ptr = (CM_SV_ITERATOR_PTR_TYPE)local_mem;

	if( cm_is_null(iter_ptr) ) {

		return LSA_FALSE;
	}

	iter_ptr->dev = dev;
	iter_ptr->pdev.state = CM_PD_ITER_PDEV_ARData_GetAPDUStatus; /* see cm_sv_frame_get_apdu_status_done() */

	dev->r1b.stat_B.iter = iter_ptr;
	}

	/***/

	{
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	r1pd_alarm->msg_mirror_set = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1B_PDEV_ALARM_MIRROR_SET_MESSAGE_MAX);

	if( cm_is_null(r1pd_alarm->msg_mirror_set) ) {

		return LSA_FALSE;
	}

	r1pd_alarm->msg_mirror_cancel = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CANCEL);

	if( cm_is_null(r1pd_alarm->msg_mirror_cancel) ) {

		return LSA_FALSE;
	}

	r1pd_alarm->msg_mirror_clr = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CLR);

	if( cm_is_null(r1pd_alarm->msg_mirror_clr) ) {

		return LSA_FALSE;
	}

	/***/

	r1pd_alarm->mirr = cm_upper_alloc_rqb(dev->channel);

	if( cm_is_null(r1pd_alarm->mirr) ) {

		return LSA_FALSE;
	}
	else {

		CM_UPPER_RQB_PTR_TYPE alarm_rb = r1pd_alarm->mirr;

		CM_RQB_SET_OPCODE(alarm_rb, 0/*unused*/);

		alarm_rb->args.sv.ar_alarm_send->alarm_data = (CM_COMMON_MEM_U8_PTR_TYPE)cm_sv_upper_mem_alloc(dev->channel, CM_ALARM_PAYLOAD_MAX_woUSI); /* note: USI is stored in "alarm_tag" */

		if( cm_is_null(alarm_rb->args.sv.ar_alarm_send->alarm_data) ) {

			return LSA_FALSE;
		}
	}
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== R1-BRIDGE DEVICE R1B UNDO-INIT ========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_r1b_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	CM_ASSERT( dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_UNKNOWN || dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_BROKEN );

	CM_ASSERT( cm_is_null(dev->r1b.control.bridge_broken_rqb) );

	/***/

	if( cm_is_not_null(dev->r1b.upd.msg_begin_and_diag) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.upd.msg_begin_and_diag);
	}

	if ( cm_is_not_null(dev->r1b.upd.msg_alarms) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.upd.msg_alarms);
	}

	if( cm_is_not_null(dev->r1b.upd.msg_end) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.upd.msg_end);
	}

	/***/

	if( cm_is_not_null(dev->r1b.proxy_connect_done) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.proxy_connect_done);
	}

	/***/

	if( cm_is_not_null(dev->r1b.diag.msg_user_diag) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.diag.msg_user_diag);
	}

	if (cm_is_not_null(dev->r1b.diag.msg_pdev_diag)) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.diag.msg_pdev_diag);
	}

	CM_ASSERT(CmListIsEmpty(&dev->r1b.diag.queue));
	CM_ASSERT(cm_is_null(dev->r1b.diag.current));

	CM_ASSERT(dev->r1b.diag.curr_elem_idx == 0);

	CM_ASSERT(dev->r1b.diag.pdev_diag_posted == LSA_FALSE);

	/***/

	if( cm_is_not_null(dev->r1b.pdev_A.buffer) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.pdev_A.buffer);
	}

	CM_ASSERT( CmListIsEmpty(&dev->r1b.pdev_A.queue) );
	CM_ASSERT( cm_is_null(dev->r1b.pdev_A.pd) );
	CM_ASSERT( dev->r1b.pdev_A.wf_posted == LSA_FALSE );

	/***/

	if( cm_is_not_null(dev->r1b.pdev_B.buffer) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.pdev_B.buffer);
	}

	if( cm_is_not_null(dev->r1b.pdev_B.rqb) ) {

		cm_upper_free_rqb(dev->channel, dev->r1b.pdev_B.rqb);
	}

	CM_ASSERT( cm_is_null(dev->r1b.pdev_B.br_next) );
	CM_ASSERT( cm_is_null(dev->r1b.pdev_B.pd) );
	CM_ASSERT( dev->r1b.pdev_B.wf_posted == LSA_FALSE );

	/***/

	if( cm_is_not_null(dev->r1b.stat_A.buffer) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.stat_A.buffer);
	}

	CM_ASSERT( CmListIsEmpty(&dev->r1b.stat_A.queue) );
	CM_ASSERT( cm_is_null(dev->r1b.stat_A.curr) );
	CM_ASSERT( dev->r1b.stat_A.wf_posted == LSA_FALSE );

	/***/

	if( cm_is_not_null(dev->r1b.stat_B.buffer) ) {

		cm_sv_r1b_prefix_free(dev->channel, dev->r1b.stat_B.buffer);
	}

	if( cm_is_not_null(dev->r1b.stat_B.iter) ) {

		LSA_UINT16 rc;
		CM_FREE_LOCAL_MEM(&rc, dev->r1b.stat_B.iter);
		CM_ASSERT(rc == LSA_RET_OK);
	}

	CM_ASSERT( cm_is_null(dev->r1b.stat_B.br_next) );
	CM_ASSERT( dev->r1b.stat_B.wf_posted == LSA_FALSE );

	/***/

	{
	CM_SV_ARSET_PTR_TYPE arset = CM_SV_DEV_GET_ARSET(dev);
	CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &arset->r1b.pd_alarm;

	if( cm_is_not_null(r1pd_alarm->msg_mirror_set) ) {

		cm_sv_r1b_prefix_free(dev->channel, r1pd_alarm->msg_mirror_set);
	}

	if( cm_is_not_null(r1pd_alarm->msg_mirror_cancel) ) {

		cm_sv_r1b_prefix_free(dev->channel, r1pd_alarm->msg_mirror_cancel);
	}

	if( cm_is_not_null(r1pd_alarm->msg_mirror_clr) ) {

		cm_sv_r1b_prefix_free(dev->channel, r1pd_alarm->msg_mirror_clr);
	}

	CM_ASSERT( r1pd_alarm->wf_set_posted == LSA_FALSE );
	CM_ASSERT( r1pd_alarm->wf_cancel_posted == LSA_FALSE );
	CM_ASSERT( r1pd_alarm->wf_clr_posted == LSA_FALSE );
	CM_ASSERT( cm_is_null(r1pd_alarm->orig) );

	if( cm_is_not_null(r1pd_alarm->mirr) ) {

		CM_UPPER_RQB_PTR_TYPE upper = r1pd_alarm->mirr;
		r1pd_alarm->mirr = LSA_NULL;

		CM_ASSERT(CM_RQB_GET_OPCODE(upper) == 0/*unused*/);

		if( cm_is_not_null(upper->args.sv.ar_alarm_send->alarm_data) ) {

			cm_sv_upper_mem_free(dev->channel, upper->args.sv.ar_alarm_send->alarm_data);
		}
		cm_upper_free_rqb(dev->channel, upper);
	}

	CM_ASSERT( r1pd_alarm->orig_is_completed == LSA_FALSE );
	CM_ASSERT( r1pd_alarm->mirr_is_completed == LSA_FALSE );
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
