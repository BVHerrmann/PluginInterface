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
/*  F i l e               &F: cm_sv46.c                                 :F&  */
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
/*  - data-status-changed-indication                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7460
#define CM_MODULE_ID       7460

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

#if (CM_DATA_STATUS_INIT | EDD_CSRT_DSTAT_BIT_DATA_VALID) != EDD_CSRT_DSTAT_LOCAL_INIT_VALUE
#error "who changed that?"
#endif

static LSA_UINT16
cm_sv_data_status_ev_to_ds( /* event-coding (2 bits) to datastatus-coding (1 bit) */
	LSA_UINT16 ev
) {
	LSA_UINT16 ds = 0;

	if( (ev & CM_EVENT_APDU_STATUS_PRIMARY) != 0 ) {

		ds |= CM_DATA_STATUS_PRIMARY;
	}
	else {

		if( (ev & CM_EVENT_APDU_STATUS_BACKUP_HELPER) != 0 ) {

			ds |= CM_DATA_STATUS_ALL_BACKUP;
		}
	}

	if( (ev & CM_EVENT_APDU_STATUS_RUN) != 0 ) {

		ds |= CM_DATA_STATUS_RUN;
	}

	if( (ev & CM_EVENT_APDU_STATUS_STATION_OK) != 0 ) {

		ds |= CM_DATA_STATUS_OK;
	}

	if( (ev & CM_EVENT_APDU_STATUS_SYSRED_PROBLEM) != 0 ) {

		ds |= CM_DATA_STATUS_REDUNDANCY;
	}

	return ds;
}

/*===========================================================================*/
/*=== DATA-STATUS INDICATION ================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_data_status_indication(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT16 data_status_curr,
	LSA_UINT16 data_status_prev
) {
	CM_CHANNEL_PTR_TYPE channel = argr->ar_com.channel;
	LSA_BOOL ind_done;

	/***/

	if( ! channel->usr.sv.enable_DataStatusIndication ) {

		ind_done = LSA_FALSE; /* not enabled */
	}
	else if( (argr->cm.sv.ar_flags & (CM_SV_AR_FL_CONNECT_PHASE | CM_SV_AR_FL_PREDATA_PHASE)) != 0 ) {

		/* note: PNIO-Spec says that io-data "shall be evaluated" only after appl-ready.cnf/new-data, assuming this applies to data-status too */

		ind_done = LSA_FALSE; /* still connecting */
	}
	else if( (argr->cm.sv.cn_flags & CM_SV_AR_CNFL_WF_DATA_STATUS_RSP) != 0 ) {

		ind_done = LSA_FALSE; /* indication running */
	}
	else if( cm_ar_com_is_down(argr) ) {

		ind_done = LSA_FALSE; /* already down (note: actually unreachable) */
	}
	else {

		CM_UPPER_RQB_PTR_TYPE event = cm_sv_dev_get_event_notnull(LSA_NULL, argr, CM_OPC_SV_AR_DATA_STATUS_IND);
		CM_UPPER_EVENT_PTR_TYPE param = event->args.sv.ar_event;

		param->u.sv.ar_data_status.curr = cm_sv_data_status_ev_to_ds(data_status_curr);
		param->u.sv.ar_data_status.prev = cm_sv_data_status_ev_to_ds(data_status_prev);

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev/ar(%u/%u) curr(0x%04x) prev(0x%04x)"
			, argr->device_nr, argr->ar_nr
			, param->u.sv.ar_data_status.curr
			, param->u.sv.ar_data_status.prev
			);

		/***/

		argr->cm.sv.cn_flags |= CM_SV_AR_CNFL_WF_DATA_STATUS_RSP;

		cm_callback(channel, CM_OK, &event);

		ind_done = LSA_TRUE;
	}

	return ind_done;
}

/*===========================================================================*/
/*=== AR DATA-STATUS RESPONSE ===============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_data_status_response(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;
	CM_AR_GRAPH_PTR_TYPE argr;

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_WF_DATA_STATUS_RSP);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		CM_FATAL(); /* bug or cm-user fiddled with the RQB */
	}
	else {

		argr->cm.sv.cn_flags &= ~CM_SV_AR_CNFL_WF_DATA_STATUS_RSP;

		cm_sv_dev_reprovide_event(argr->cm.sv.dev, rb);

		/***/

		if( cm_ar_com_is_down(argr) ) {

			cm_sv_ar_disconnect_indication_join(argr); /* trigger: CM_SV_AR_CNFL_WF_DATA_STATUS_RSP */
		}
		else {

			cm_ar_com_cons_apdu_status_check_unicast(argr); /* maybe another delta in the meantime */
		}
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
