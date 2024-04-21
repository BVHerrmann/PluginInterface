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
/*  F i l e               &F: cm_sv9.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, M-Consumer things                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7900
#define CM_MODULE_ID       7900

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

#if CM_CFG_SV_AR_HAS_MCONS
#error "this code is out of date (m-consumers are currently not supported)"
#error "mcons-alarm and DeviceAdd::alarm_send_max_length, implement consistency like cm-pd, search for AP01232054"
#endif

/*===========================================================================*/
#if CM_CFG_SV_AR_HAS_MCONS /* or else exclude from compilation */
/*===========================================================================*/

LSA_BOOL
cm_sv_mcons_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE io_cr,
	LSA_OPCODE_TYPE cm_opcode,
	LSA_UINT16 nare_rsp, /* see NARE_RSP_xxx */
	CM_MAC_PTR_TYPE source_mac, /* ethernet mac-addr or error-mac */
	LSA_UINT32 source_ip, /* ip-addr or error-ip */
	LSA_UINT16 data_status_delta
) {
	CM_AR_GRAPH_PTR_TYPE argr;

	CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "iocr(%08X) cm_opcode(%u)", io_cr, cm_opcode);

	/***/

	argr = io_cr->cm.backlink;

	/***/

	if( cm_ar_com_is_down(argr) ) {
		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "iocr(%08X) ar already down", io_cr);
		return LSA_FALSE; /* no indication, mcons must not wait for a response */
	}

	/***/

	if( cm_opcode != CM_OPC_SV_EVENT_MCONS_DATA_STATUS_IND ) {

		cm_sv_ar_problem_indicator_eval(argr); /* trigger: m-consumer changed */

		cm_sv_dev_led_info_eval(argr->cm.sv.dev); /* trigger: m-consumer changed */
	}

	/***/

	{
	CM_UPPER_RQB_PTR_TYPE event = cm_sv_dev_get_event_notnull(LSA_NULL, argr, cm_opcode);
	CM_UPPER_EVENT_PTR_TYPE param = event->args.sv.ar_event;

	switch( cm_opcode ) {

	case CM_OPC_SV_EVENT_MCONS_ADDR_INFO_IND:

		param->u.sv.mcons_addr_info.cr_ref     = io_cr->iocr_ref;

		if( cm_is_not_null(source_mac) ) {

			param->u.sv.mcons_addr_info.info_class = (nare_rsp == NARE_RSP_OK) ? CM_ADDR_INFO_CLASS_OK : CM_ADDR_INFO_CLASS_NARE;
			param->u.sv.mcons_addr_info.nare_rsp   = nare_rsp;
			param->u.sv.mcons_addr_info.source_mac = *source_mac;
			param->u.sv.mcons_addr_info.source_ip  = source_ip;
		}
		else {

			param->u.sv.mcons_addr_info.info_class = CM_ADDR_INFO_CLASS_MISS;
			param->u.sv.mcons_addr_info.nare_rsp   = 0;

			CM_MEMSET(&param->u.sv.mcons_addr_info.source_mac, 0, sizeof (param->u.sv.mcons_addr_info.source_mac));

			param->u.sv.mcons_addr_info.source_ip  = 0;
		}
		break;

	case CM_OPC_SV_EVENT_MCONS_IN_DATA_IND:

		param->u.sv.mcons_in_data.cr_ref    = io_cr->iocr_ref;
		break;

	case CM_OPC_SV_EVENT_MCONS_OFFLINE_IND:

		param->u.sv.mcons_offline.cr_ref = io_cr->iocr_ref;
		break;

	case CM_OPC_SV_EVENT_MCONS_DATA_STATUS_IND:

		param->u.sv.mcons_data_status.cr_ref = io_cr->iocr_ref;
		param->u.sv.mcons_data_status.data_status = data_status_delta;
		break;

	default:
		CM_FATAL(); /* a bug */
		break;
	}

	cm_callback(channel, CM_OK, &event);
	}

	/***/

	return LSA_TRUE; /* indication, mcons has to wait for the response */
}

/*===========================================================================*/

static CM_AR_GRAPH_IO_CR_PTR_TYPE
cm_sv_mcons_response__notnull(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb,
	LSA_UINT16 cr_ref
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	/***/

	CM_SV_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "dev/ar(%u/%u) sk(0x%x) cm_opcode(%u)"
		, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
		, param->session_key
		, CM_RQB_GET_OPCODE(rb)
		);

	/***/

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, xxx);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
		/* not reached */
	}

	/***/

	ar_iocr = cm_ar_graph_find_io_cr(argr, cr_ref);

	if( cm_is_null(ar_iocr) ) {
		CM_SV_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_FATAL, "iocr_ref not found");
		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
		/* not reached */
	}

	CM_ASSERT(argr == ar_iocr->cm.backlink);

	/***/

	cm_sv_dev_reprovide_event(dev, rb); /* first provide */

	cm_xxx_mcons_upper_response(channel, ar_iocr);

	/***/

	return ar_iocr;
}

/*===========================================================================*/

static LSA_VOID
cm_sv_mcons_alarm(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr,
	LSA_UINT16 ext_err
) {
	CM_AR_GRAPH_AP_PTR_TYPE  ar_ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  ar_mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  ar_sub;

	CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "iocr(%08X) ext_err(0x%x)", ar_iocr, ext_err);

	/***/

	{
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioap;
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;

	ioap = CmListFirst(&ar_iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE);
	if( cm_is_null(ioap) ) {
		CM_FATAL(); /* a bug */
		return;
	}

	iodata = CmListFirst(&ioap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE);
	if( cm_is_null(iodata) ) {
		CM_FATAL(); /* a bug */
		return;
	}

	cm_ar_graph_find_api_slot_subslot(ar_iocr->cm.backlink, ioap->api, iodata->slot_nr, iodata->subslot_nr, &ar_ap, &ar_mod, &ar_sub);
	if( cm_is_null(ar_sub) ) {
		CM_FATAL(); /* a bug */
		return;
	}
	}

	/***/

	{
	LSA_UINT16  alarm_data_len;
	CM_UPPER_MEM_U8_PTR_TYPE  alarm_data;

	alarm_data_len = 2+2+2 + 2+4;

	alarm_data = (CM_UPPER_MEM_U8_PTR_TYPE)cm_sv_upper_mem_alloc(channel, alarm_data_len);

	if( cm_is_null(alarm_data) ) {

		cm_ar_com_down (ar_iocr->cm.backlink, CM_AR_REASON_MEM);
		return;
	}

	/*
	AlarmType CM_ALARM_TYPE_MCR_MISMATCH

	ChannelErrorType ..... 0x8005

	ExtChannelErrorType
		0x8000	Multicast Consumer CR timed out	(alarm/diagnosis)
		0x8001	Address Resolution Failed	(diagnosis)
		0x8002	Multicast Consumer CR Okay with M-Provider Status Mismatch	(alarm/diagnosis)

	ExtChannelAddValue ... 0 if no information (not specified in CMDEV)
	*/

	{
	LSA_UINT16  usi;
	LSA_UINT16  prop;

	CM_ASSERT(
		ext_err == 0x8000 /* Multicast Consumer CR timed out */
		/*|| ext_err == 0x8001 -- Address Resolution Failed is not an alarm */
		|| ext_err == 0 /* Multicast Consumer CR Okay */
		|| ext_err == 0x8002 /* Multicast Consumer CR Okay with M-Provider Status Mismatch */
	);

	if( ext_err != 0/*Multicast Consumer CR Okay*/ ) {

		usi  = 0x8002; /* ExtChannelDiagnosis */
		prop = (CM_CHPR_MAINT_ERR | CM_CHPR_SPEC_APP);
	}
	else {

		usi  = 0x8000; /* ChannelDiagnosis, 24.01.2006: "going" shall use the short form */
		prop = (CM_CHPR_MAINT_ERR | CM_CHPR_SPEC_DIS);
	}

	/***/

	CM_ASSERT (alarm_data_len == 12); /* see above */

	alarm_data_len = 0;

	CM_PUT16_HTON (alarm_data, alarm_data_len, 0x8000/*ChannelNumber*/);
	alarm_data_len += 2;

	CM_PUT16_HTON (alarm_data, alarm_data_len, prop);
	alarm_data_len += 2;

	CM_PUT16_HTON (alarm_data, alarm_data_len, 0x8005/*ChannelErrorType*/);
	alarm_data_len += 2;

	if( usi == 0x8002 ) {

		CM_PUT16_HTON (alarm_data, alarm_data_len, ext_err);
		alarm_data_len += 2;

		CM_PUT32_HTON (alarm_data, alarm_data_len, 0/*ExtChannelAddValue*/);
		alarm_data_len += 4;

		CM_ASSERT (alarm_data_len == 12); /* see above */
	}
	else {

		CM_ASSERT (alarm_data_len == 6); /* see above */
	}

	/***/

#if CM_DEBUG
	CM_ASSERT(cm_sv_ar_alarm_send_format_ok(param));
#endif

	/***/

	if( ! cm_sv_internal_alarm_xxx (channel,
			ar_iocr->cm.backlink, ar_ap, ar_mod, ar_sub,
			CM_OPC_SV_AR_ALARM_MCONS, CM_ALARM_TYPE_MCR_MISMATCH,
			usi, alarm_data_len, alarm_data
			)
	) {
		LSA_UINT16  ret_val;

		/* already down or no memory */

		cm_ar_com_down(argr, xxx);

		cm_sv_upper_mem_free(channel, alarm_data);
	}
	}
	}
}

/*===========================================================================*/

static LSA_VOID
cm_sv_mcons_addr_info_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "cr_ref(%u)", rb->args.sv.ar_event->u.sv.mcons_addr_info.cr_ref);

	ar_iocr = cm_sv_mcons_response__notnull(channel, rb, rb->args.sv.ar_event->u.sv.mcons_addr_info.cr_ref);

	if down then join-disconnect(ar_iocr->cm.backlink);
}

/*===========================================================================*/

static LSA_VOID
cm_sv_mcons_in_data_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "cr_ref(%u)", rb->args.sv.ar_event->u.sv.mcons_in_data.cr_ref);

	ar_iocr = cm_sv_mcons_response__notnull(channel, rb, rb->args.sv.ar_event->u.sv.mcons_in_data.cr_ref);

	cm_sv_mcons_alarm(channel, ar_iocr, 0/*Multicast Consumer CR Okay*/);

	if down then join-disconnect(ar_iocr->cm.backlink);
}

/*===========================================================================*/

static LSA_VOID
cm_sv_mcons_offline_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "cr_ref(%u)", rb->args.sv.ar_event->u.sv.mcons_offline.cr_ref);

	ar_iocr = cm_sv_mcons_response__notnull(channel, rb, rb->args.sv.ar_event->u.sv.mcons_offline.cr_ref);

	cm_sv_mcons_alarm(channel, ar_iocr, 0x8000/*Multicast Consumer CR timed out*/);

	if down then join-disconnect(ar_iocr->cm.backlink);
}

/*===========================================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_mcons_data_status_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "cr_ref(%u)", rb->args.sv.ar_event->u.sv.mcons_data_status.cr_ref);

	ar_iocr = cm_sv_mcons_response__notnull(channel, rb, rb->args.sv.ar_event->u.sv.mcons_data_status.cr_ref);

	if down then join-disconnect(ar_iocr->cm.backlink);
}

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_mcons_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {

#error "to be done when m-consumers are supported"

/* opcodes were refactored into one.
 * switch on mcons_event:
 * - cm_sv_mcons_addr_info_response()
 * - cm_sv_mcons_in_data_response()
 * - cm_sv_mcons_offline_response()
 * - cm_sv_mcons_data_status_response()
 */

}

/*===========================================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_mcons_diag_value(
	CM_AR_GRAPH_PTR_TYPE argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr,
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap,
	CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub
) {
	LSA_UINT16 ext_err;

	if( cm_is_not_null(ar_iocr) ) {

		CM_ASSERT(ar_iocr->cm.is_consumer && ar_iocr->cm.is_multicast); /* see caller */
	}
	else {

		CM_AR_GRAPH_IOAPI_PTR_TYPE ioapi;
		CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE iodata;

		CM_ASSERT(
			cm_is_not_null(ar_ap)
			&& cm_is_not_null(ar_mod)
			&& cm_is_not_null(ar_sub)
			);

		CM_ASSERT(
			ar_sub->out_via_mcr /* is a m-consumer-submodule */
			);

		for( CmListEach(ar_iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE) ) {

			if( ar_iocr->cm.is_aux ) { /* necessary? aux-iocrs are unicast */
				continue;
			}

			if( ar_iocr->cm.is_consumer && ar_iocr->cm.is_multicast ) {

				ioapi = cm_ar_graph_find_ioapi(ar_iocr, ar_ap->api);

				if( cm_is_not_null(ioapi) ) {

					iodata = cm_ar_graph_find_iodata(ioapi, ar_mod->slot_nr, ar_sub->subslot_nr);

					if( cm_is_not_null(iodata) ) {

						break; /* note: is only once in a mcr */
					}
				}
			}
		}

		/* found the iocr containing the subslot */

		if( cm_is_null(ar_iocr) ) {
			CM_FATAL(); /* a bug, see consistency-check */
		}
	}

	if( cm_xxx_mcons_is_up(ar_iocr) ) {

		ext_err = 0; /* Multicast Consumer CR Okay */
	}
	else { /* has (appearing) diagnosis */

		if( cm_xxx_mcons_is_resolved(ar_iocr) ) { /* down but resolved, thus down because of miss */

			ext_err = 0x8000; /* Multicast Consumer CR timed out */
		}
		else {

			ext_err = 0x8001; /* Address Resolution Failed */
		}
	}

	/***/

	CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr_ref(%u) ext_err(%u)"
		, ar_iocr->iocr_ref, ext_err
		);

	return ext_err;
}

/*===========================================================================*/
#else
# ifdef CM_MESSAGE
# pragma CM_MESSAGE("***** not compiling server multicast-consumer (CM_CFG_SV_AR_HAS_MCONS=" CM_STRINGIFY(CM_CFG_SV_AR_HAS_MCONS) ")")
# endif /* CM_MESSAGE */
#endif

/*===========================================================================*/
#else
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("***** not compiling server part (CM_CFG_MAX_SERVERS=" CM_STRINGIFY(CM_CFG_MAX_SERVERS) ")")
#endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
