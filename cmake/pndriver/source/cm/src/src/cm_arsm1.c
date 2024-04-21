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
/*  F i l e               &F: cm_arsm1.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  AR state-machines                                                        */
/*  - cons apdu-status changed                                               */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  71
#define CM_MODULE_ID       71

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*===========================================================================*/
/*=== CM-AR-COM CONSUMER APDU-STATUS CHANGED ================================*/
/*===========================================================================*/

LSA_VOID
cm_ar_com_cons_apdu_status_changed (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_UINT32 state_event
) {
	/* see CM_EVENT_APDU_STATUS_INITIAL_VALUE */

	if (state_event & EDD_CSRT_CONS_EVENT_STOP) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) STOP", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_STOP;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_RUN;
	}
	else if (state_event & EDD_CSRT_CONS_EVENT_RUN) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) RUN", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_RUN;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_STOP;
	}

	/***/

	if (state_event & EDD_CSRT_CONS_EVENT_STATION_FAILURE) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) STATION_FAILURE", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_STATION_PROBLEM;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_STATION_OK;
	}
	else if (state_event & EDD_CSRT_CONS_EVENT_STATION_OK) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) STATION_OK", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_STATION_OK;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_STATION_PROBLEM;
	}

	/***/

	if (state_event & EDD_CSRT_CONS_EVENT_BACKUP) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) BACKUP", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_BACKUP;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_PRIMARY;
	}
	else if (state_event & EDD_CSRT_CONS_EVENT_PRIMARY) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) PRIMARY", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_PRIMARY;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_BACKUP;
	}

	/***/

	if (state_event & EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) SYSRED-OK", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_SYSRED_OK;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_SYSRED_PROBLEM;
	}
	else if (state_event & EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS) {

		CM_AR_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "ConsumerApduInd: dev(%u) ar(%u) frame_id(0x%04x) acp_id(%u) SYSRED-PROBLEM", argr->device_nr, argr->ar_nr, iocr->frame_id, iocr->cm.acp_id);

		iocr->cm.cons_data_status |= CM_EVENT_APDU_STATUS_SYSRED_PROBLEM;
		iocr->cm.cons_data_status &= ~CM_EVENT_APDU_STATUS_SYSRED_OK;
	}

	/***/

	if( iocr->cm.is_multicast ) {

#if CM_CFG_MAX_MULTICAST_CHANNELS
		if( argr->ar_com.path_type != CM_PATH_TYPE_MULTICAST ) {
			CM_FATAL();
		}
		else {
			cm_mc_ar_mcons_data_status_check(argr, iocr);
		}
#else
		CM_FATAL(); /* a bug */
#endif
	}
	else {

		cm_ar_com_cons_apdu_status_check_unicast (argr);
	}
}

/*===========================================================================*/
/*=== CM-AR-COM::cons_apdu_status_count =====================================*/
/*===========================================================================*/

static LSA_VOID
cm_ar_com_cons_apdu_status_count_unicast (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr,
	LSA_UINT16 *cr_cnt,
	LSA_UINT16 *operate,
	LSA_UINT16 *station,
	LSA_UINT16 *primary,
	LSA_UINT16 *backup,
	LSA_UINT16 *sysred
	)
{
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

	*cr_cnt = *operate = *station = *primary = *backup = *sysred = 0;

	for (CmListForeach (iocr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {
		if (iocr->cm.cr_state == CM_AR_CR_ALLOCATED) {
			continue; /* ignore */
		}
		if (! iocr->cm.is_consumer) {
			continue; /* only consumer */
		}
		if (iocr->cm.is_multicast) {
			continue; /* only unicast */
		}
		else {
			*cr_cnt += 1;
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_RUN) {
				*operate += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_STATION_OK) {
				*station += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_PRIMARY) {
				*primary += 1;
			}
			if(iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_BACKUP) {
				*backup += 1;
			}
			if (iocr->cm.cons_data_status & CM_EVENT_APDU_STATUS_SYSRED_OK) {
				*sysred += 1;
			}
		}
	}
	CM_AR_TRACE_08(argr->trace_idx, LSA_TRACE_LEVEL_CHAT,
		"dev/ar(%u/%u)  cr(%u) op(%u) st(%u) pr(%u) bk(%u) srd(%u)",
		argr->device_nr, argr->ar_nr,
		*cr_cnt,  *operate, *station, *primary, *backup, *sysred
		);
}

/*===========================================================================*/
/*=== CM-AR-COM::cons_apdu_status_check_unicast =============================*/
/*===========================================================================*/

LSA_VOID
cm_ar_com_cons_apdu_status_check_unicast (
	CM_AR_GRAPH_PTR_TYPE  argr
)
{
	LSA_UINT16  cr_cnt;
	LSA_UINT16  operate;
	LSA_UINT16  station;
	LSA_UINT16  primary;
	LSA_UINT16  backup;
	LSA_UINT16  sysred;

	/* AP00754080 (change of apdustatus not indicated to user)
	 *
	 * the formerly "return if not cm_ar_com_is_up()" here was the problem. the intention
	 * was to defer the checking until all-again before making the 'all-decision'.
	 *
	 * presumably, the bug krept in with the reset-to-unknown functionality.
	 * (is_up() is false between reset and all-again and the check on all-again was missing.)
	 *
	 * however, it is not necessary to wait for all-again because the reset-to-unknown does
	 * not affect the apdustatus bits (only the miss/again bits). since the apdustatus is
	 * indicated only after in-data.ind (= all-again) there is no problem on startup either.
	 */

	if ( cm_ar_com_is_down(argr) ) {
		CM_AR_TRACE_00(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "already down"
			);
		return;
	}

	/***/

	cm_ar_com_cons_apdu_status_count_unicast (argr,
			& cr_cnt, & operate, & station, & primary, & backup, & sysred
		);

	if (cr_cnt > 0) {

		LSA_UINT16  new_apdu_status = 0; /* ind: see cm_event_apdu_status_enum */
		LSA_UINT16  apdu_delta;

		if (operate == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_RUN; /* all */
		}
		else {
			new_apdu_status |= CM_EVENT_APDU_STATUS_STOP; /* any */
		}

		if (station == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_STATION_OK; /* all */
		}
		else {
			new_apdu_status |= CM_EVENT_APDU_STATUS_STATION_PROBLEM; /* any */
		}

		if (primary == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_PRIMARY; /* all */
		}
		else {

			if( argr->ar_com.path_type == CM_PATH_TYPE_SERVER ) { /* device */

				new_apdu_status |= CM_EVENT_APDU_STATUS_BACKUP; /* any */

				if (backup == cr_cnt) {
					new_apdu_status |= CM_EVENT_APDU_STATUS_BACKUP_HELPER; /* all backup */
				}
			}
			else {

				if (backup == cr_cnt) {
					new_apdu_status |= CM_EVENT_APDU_STATUS_BACKUP; /* all backup */
				}
			}
		}

		if (sysred == cr_cnt) {
			new_apdu_status |= CM_EVENT_APDU_STATUS_SYSRED_OK; /* all */
		}
		else {
			new_apdu_status |= CM_EVENT_APDU_STATUS_SYSRED_PROBLEM; /* any */
		}

		apdu_delta = CM_CAST_U16((~ argr->ar_com.user_apdu_status) & new_apdu_status);

		CM_AR_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev(%u) ar(%u) Datastatus-Check: old(0x%x) new(0x%x) diff(0x%x)"
			, argr->device_nr, argr->ar_nr
			, argr->ar_com.user_apdu_status, new_apdu_status, apdu_delta
			);

		if (apdu_delta != 0) {

			switch (argr->ar_com.path_type) {
#if CM_CFG_MAX_CLIENTS
			case CM_PATH_TYPE_CLIENT:
				if (cm_cl_ar_com_apdu_status_ind (argr, apdu_delta)) {
					argr->ar_com.user_apdu_status = new_apdu_status;
				}
				break;
#endif
#if CM_CFG_MAX_SERVERS
			case CM_PATH_TYPE_SERVER:
				if (cm_sv_ar_data_status_indication (argr, new_apdu_status, argr->ar_com.user_apdu_status)) {
					argr->ar_com.user_apdu_status = new_apdu_status;
				}
				break;
#endif
			default:
				CM_AR_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_FATAL, "!path_type(%u)", argr->ar_com.path_type);
				CM_FATAL1 (argr->ar_com.path_type);
				return;
			}
		}
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
