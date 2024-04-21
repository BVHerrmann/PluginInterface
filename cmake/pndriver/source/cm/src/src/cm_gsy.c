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
/*  F i l e               &F: cm_gsy.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  GSY (generic sync) things                                                */
/*                                                                           */
/*  - alloc GSY-RQB                                                          */
/*  - free GSY-RQB                                                           */
/*  - channel open                                                           */
/*  - channel info                                                           */
/*  - channel close                                                          */
/*  - lower-done callback function                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	41
#define CM_MODULE_ID		41

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_GSY

/*===========================================================================*/

static LSA_VOID
cm_gsy_diag_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
);

static LSA_VOID
cm_gsy_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
);

static LSA_VOID
cm_gsy_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
);


/*===========================================================================*/
/*=== GSY CHANNEL-INIT / -UNDO-INIT =========================================*/
/*===========================================================================*/

LSA_VOID
cm_gsy_channel_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	/***/

	channel->usr.gsy.do_sync_clock = LSA_FALSE;
	channel->usr.gsy.subdomain = LSA_FALSE;
	channel->usr.gsy.is_sync = LSA_FALSE;

	channel->usr.gsy.is_rate_valid = LSA_FALSE;
	channel->usr.gsy.rcv_sync_prio = 0;
	channel->usr.gsy.local_prio    = 0;
	channel->usr.gsy.pll_state     = GSY_PLL_STATE_SYNC_OFF;

	CM_MEMSET(&channel->usr.gsy.master_mac_addr, 0, sizeof(channel->usr.gsy.master_mac_addr));

	/* provide some diag-indication resources (must do it in context of CM-GSY, not CM-PD) */

	cm_gsy_diag_indication(channel, LSA_NULL);

	/* provide some prm-indication resources (must do it in context of CM-GSY, not CM-PD) */

	cm_gsy_prm_indication(channel, LSA_NULL);
}

/*===========================================================================*/

LSA_UINT16
cm_gsy_channel_undo_init(
	CM_CHANNEL_CONST_PTR_TYPE channel
) {
	/* NOTE: caller wipes out channel->usr.xxx when returning CM_OK */

	if( channel->state != CM_CH_READY ) {
		CM_FATAL(); /* see caller */
	}

	if( cm_is_not_null(channel->lower.channel[CM_PATH_TYPE_PD]) ) {
		CM_FATAL(); /* cm-pd not unlinked */
	}

	return CM_OK;
}

/*-----------------------------------------------------------------------------
//	get system-channel
//---------------------------------------------------------------------------*/
CM_CHANNEL_PTR_TYPE
cm_gsy_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
) {
	channel = channel->lower.channel[CM_PATH_TYPE_GSY];

	if( cm_is_null(channel) ) {
		CM_FATAL(); /* a bug */
	}

	return channel;
}

/*-----------------------------------------------------------------------------
//	alloc lower GSY-RQB that fits for all requests
//---------------------------------------------------------------------------*/
static CM_GSY_LOWER_RQB_PTR_TYPE
cm_gsy_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get(channel);

	CM_GSY_LOWER_RQB_PTR_TYPE lower_rqb_ptr;

		/* alloc the RQB */
	CM_GSY_ALLOC_LOWER_RQB (&lower_rqb_ptr, cm_null_user_id, sizeof (CM_GSY_LOWER_RQB_TYPE), gsy_channel->sysptr);

	if (cm_is_null (lower_rqb_ptr)) {
		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) cannot alloc GSY RQB, len(%u)"
			, channel->my_handle, sizeof (CM_GSY_LOWER_RQB_TYPE)
			);
		return LSA_NULL;
	}

#if CM_DEBUG
	CM_GSY_SET_NEXT_RQB_PTR (lower_rqb_ptr, LSA_NULL); /* sanity */
	CM_GSY_SET_PREV_RQB_PTR (lower_rqb_ptr, LSA_NULL); /* sanity */
#endif

	return lower_rqb_ptr;
}


/*-----------------------------------------------------------------------------
//	free the RQB and the attached parameters
//---------------------------------------------------------------------------*/
LSA_VOID
cm_gsy_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
)
{
	CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get(channel);

	CM_ASSERT (cm_is_not_null(lower_rqb_ptr));

	{
	LSA_UINT16	rc;
	CM_GSY_FREE_LOWER_RQB (
		& rc,
		lower_rqb_ptr,
		gsy_channel->sysptr
		);
	CM_ASSERT (rc == LSA_RET_OK);
	LSA_UNUSED_ARG(rc);
	}
}


/*-----------------------------------------------------------------------------
//	open the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_gsy_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
)
{
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_GSY);

	CM_ASSERT (cm_gsy_channel_get(channel) == channel);

	CM_ASSERT (channel->state == CM_CH_OPEN);

	CM_ASSERT (channel->lower.handle == CM_GSY_INVALID_HANDLE);

	/***/

	channel->lower.prealloc.gsy = LSA_NULL;

	gsy = cm_gsy_alloc_rqb (channel);

	if (cm_is_null (gsy)) {
		cm_channel_done (channel, CM_ERR_ALLOC_LOWER);
		return;
	}

	CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%u) sys_path(%u)"
		, channel->my_handle, channel->usr.ch.curr->args.channel.open->sys_path
		);

	gsy->args.channel.handle = CM_GSY_INVALID_HANDLE;
	gsy->args.channel.handle_upper = channel->my_handle;
	gsy->args.channel.sys_path = channel->usr.ch.curr->args.channel.open->sys_path;
	gsy->args.channel.gsy_request_upper_done_ptr = cm_gsy_request_lower_done;

	CM_GSY_SET_USER_ID_HANDLE_DEV_AR (gsy, channel->my_handle, 0, 0);

	CM_GSY_SET_HANDLE (gsy, CM_GSY_INVALID_HANDLE);
	CM_GSY_SET_OPCODE (gsy, GSY_OPC_OPEN_CHANNEL);

	CM_GSY_OPEN_CHANNEL_LOWER (gsy, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_gsy_channel_open_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy
)
{
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_OPEN);

	rc = CM_GSY_GET_RESPONSE (gsy);

	if (rc == GSY_RSP_OK) {

		channel->lower.handle = gsy->args.channel.handle;
		channel->lower.prealloc.gsy = gsy;

		CM_ASSERT(channel->lower.handle != CM_GSY_INVALID_HANDLE);

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u) gsy-handle(%u)"
			, channel->my_handle, channel->lower.handle
			);

		rc = CM_OK;
	}
	else {

		CM_ASSERT(channel->lower.handle == CM_GSY_INVALID_HANDLE);

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) gsy-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;

		cm_gsy_free_rqb(channel, gsy);
	}

	cm_channel_done (channel, rc);
}


/*-----------------------------------------------------------------------------
//	close the channel
//---------------------------------------------------------------------------*/
LSA_VOID
cm_gsy_channel_close (
	CM_CHANNEL_CONST_PTR_TYPE channel
)
{
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	CM_ASSERT (channel->path_type == CM_PATH_TYPE_GSY);

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	CM_ASSERT (cm_is_not_null(channel->lower.prealloc.gsy));

	gsy = channel->lower.prealloc.gsy;

	CM_GSY_SET_USER_ID_HANDLE_DEV_AR (gsy, channel->my_handle, 0, 0);

	CM_GSY_SET_HANDLE (gsy, channel->lower.handle);
	CM_GSY_SET_OPCODE (gsy, GSY_OPC_CLOSE_CHANNEL);

	CM_GSY_CLOSE_CHANNEL_LOWER (gsy, channel->sysptr);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_gsy_channel_close_done (
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy
) {
	LSA_UINT16 rc;

	CM_ASSERT (channel->state == CM_CH_CLOSE);

	rc = CM_GSY_GET_RESPONSE (gsy);

	if (rc == GSY_RSP_OK) {

		cm_gsy_free_rqb (channel, gsy);

		channel->lower.handle = CM_GSY_INVALID_HANDLE;
		channel->lower.prealloc.gsy = LSA_NULL;

		CM_GSY_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "h(%u)"
			, channel->my_handle
			);

		rc = CM_OK;
	}
	else {

		/* don't invalidate gsy-handle, channel is not closed! */

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%u) gsy-rsp(%u)"
			, channel->my_handle, rc
			);

		rc = CM_ERR_LOWER_LAYER;
	}

	cm_channel_done (channel, rc);
}

/*===========================================================================*/
/*=== DIAG INDICATION =======================================================*/
/*===========================================================================*/

static LSA_VOID
cm_gsy_diag_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
) {
	if( cm_is_null(gsy) ) { /* provide */

		gsy = cm_gsy_alloc_rqb(channel);

		if( cm_is_null(gsy) ) {
			CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot gsy-alloc");
			CM_FATAL();
			return;
		}

		/***/

		{
		GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE *ind = &gsy->args.DiagInd;

		ind->Trigger = GSY_TRIGGER_CHECK; /* using the "stateful" mode */

		ind->IndCount = 0; /* sanity (an out-paramter!) */
		ind->SyncId   = 0; /* sanity (an out-paramter!) */

		ind->StateSync     = channel->usr.gsy.is_sync ? GSY_DIAG_SYNCID_SYNCHRONIZED : GSY_DIAG_SYNCID_NOT_SYNCHRONIZED;
		ind->MasterMacAddr = channel->usr.gsy.master_mac_addr;
		ind->Subdomain     = channel->usr.gsy.subdomain ? GSY_DIAG_SUBDOMAIN_CONFIGURED : GSY_DIAG_SUBDOMAIN_NO_RECORD;
		ind->RateValid     = channel->usr.gsy.is_rate_valid ? GSY_DIAG_RATE_VALID : GSY_DIAG_RATE_INVALID;
		ind->RcvSyncPrio   = channel->usr.gsy.rcv_sync_prio;
		ind->LocalPrio     = channel->usr.gsy.local_prio;
		ind->PLLState      = channel->usr.gsy.pll_state;

		ind->DriftInterval = 0; /* don't care for GSY_TRIGGER_CHECK */
		ind->UserValue1    = 0; /* don't care for GSY_TRIGGER_CHECK */
		ind->UserValue2    = 0; /* don't care for GSY_TRIGGER_CHECK */
		}

		cm_gsy_request_lower(channel, GSY_OPC_DIAG_INDICATION_PROVIDE, 0, 0, gsy);
	}
	else { /* handle indication and do re-provide */

		GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE *ind = &gsy->args.DiagInd;

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "gsy-diag.IND SyncId(%u) IndCount(%u)"
			, ind->SyncId, ind->IndCount
			);

		if( channel->state >= CM_CH_CLOSE ) {

			CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);

			CM_ASSERT(
				CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK
				|| CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK_CANCEL
				);

			cm_gsy_free_rqb(channel, gsy);
		}
		else if( CM_GSY_GET_RESPONSE(gsy) != GSY_RSP_OK ) {

			CM_FATAL(); /* what? why? */
		}
		else {

			CM_GSY_TRACE_12(channel->trace_idx,LSA_TRACE_LEVEL_NOTE_HIGH
				, "StateSync(%u) Subdomain(%u) MasterMac(%02x-%02x-%02x-%02x-%02x-%02x) RateValid(%u) RcvSyncPrio(%u) LocalPrio(%u) PLLState(%u)"
				, ind->StateSync, ind->Subdomain
				, ind->MasterMacAddr.MacAdr[0], ind->MasterMacAddr.MacAdr[1], ind->MasterMacAddr.MacAdr[2]
				, ind->MasterMacAddr.MacAdr[3], ind->MasterMacAddr.MacAdr[4], ind->MasterMacAddr.MacAdr[5]
				, ind->RateValid, ind->RcvSyncPrio, ind->LocalPrio, ind->PLLState
				);

			channel->usr.gsy.is_sync           = (GSY_DIAG_SYNCID_SYNCHRONIZED == ind->StateSync) ? LSA_TRUE : LSA_FALSE; /* LSA_TRUE if SyncId is synchronized, else LSA_FALSE */
			channel->usr.gsy.master_mac_addr   = ind->MasterMacAddr;
			channel->usr.gsy.subdomain         = (GSY_DIAG_SUBDOMAIN_CONFIGURED == ind->Subdomain) ? LSA_TRUE : LSA_FALSE; /* LSA_TRUE if Sync-Subdomain configured (LSA_FALSE: no PDSyncData record loaded) */
			channel->usr.gsy.is_rate_valid     = (GSY_DIAG_RATE_VALID == ind->RateValid) ? LSA_TRUE : LSA_FALSE;
			channel->usr.gsy.rcv_sync_prio     = ind->RcvSyncPrio;
			channel->usr.gsy.local_prio        = ind->LocalPrio;
			channel->usr.gsy.pll_state         = ind->PLLState;

			if( channel->usr.gsy.subdomain ) { /* have PDSyncData */

				/* note: master_mac_addr contains the current master-mac */

				/* note: master_mac_addr may be "all zero" during takeover (best master) */

				if (ind->SyncId == GSY_SYNCID_CLOCK) {
					channel->usr.gsy.do_sync_clock = LSA_TRUE;
				}
				else {
					channel->usr.gsy.do_sync_clock = LSA_FALSE;
				}
			}
			else {

				CM_ASSERT(channel->usr.gsy.is_sync == LSA_FALSE); /* sanity */

				channel->usr.gsy.is_sync = LSA_FALSE; /* make it explicit */

				/* note: master_mac_addr contains the last known master-mac */

				channel->usr.gsy.do_sync_clock = LSA_FALSE;
			}

			if( ! channel->usr.gsy.is_sync ) {

				/* OHA has to send "all zero" when not sync */

				CM_MEMSET(&channel->usr.gsy.master_mac_addr, 0, sizeof(channel->usr.gsy.master_mac_addr));
			}

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

			if( cm_is_null(pd_channel) ) { /* e.g. when closing the pd-channel */

				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "gsy-diag.IND but no pd-channel, ignored (but storing values)"
					);
			}
			else {

				cm_pd_sync_state_changed(pd_channel, /*gsy_*/channel);
			}
			}

			/***/

			cm_gsy_request_lower(channel, GSY_OPC_DIAG_INDICATION_PROVIDE, 0, 0, gsy); /* re-provide (stateful!) */
		}
	}
}

/*===========================================================================*/
/*=== PRM INTERFACE =========================================================*/
/*===========================================================================*/

CM_PORT_UINT
cm_gsy_prm_provide_mask(
	CM_CHANNEL_PTR_TYPE		channel
) {
	CM_PORT_UINT mask;

	LSA_UNUSED_ARG(channel);

	mask = ((CM_PORT_UINT)2 << 0/*interface*/) - 1;

	return mask;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID
cm_gsy_prm_indication(
	CM_CHANNEL_PTR_TYPE channel,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy /* provide: LSA_NULL */
) {
	if( cm_is_null(gsy) ) { /* provide */

		LSA_UINT16 max = 2/*!*/;
		LSA_UINT16 cnt;

		for( cnt = 0; cnt < max; ++cnt ) {

			LSA_UINT32 size;
			CM_GSY_LOWER_MEM_PTR_TYPE lower_mem;

			gsy = cm_gsy_alloc_rqb(channel);

			if( cm_is_null(gsy) ) {
				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot gsy-alloc-rqb");
				CM_FATAL();
			}

			size = sizeof(GSY_PRM_IND_DATA_TYPE) * GSY_PRM_IND_MAX_ENTRIES;

			if (size > 0xFFFF) { /* always false (see cast below) */
				CM_FATAL();
			}

			CM_GSY_ALLOC_LOWER_MEM (
				&lower_mem,
				cm_null_user_id,
				CM_CAST_U16(size),
				channel->sysptr
				);

			if (cm_is_null (lower_mem)) {
				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_FATAL, "cannot gsy-alloc-mem");
				CM_FATAL();
			}

			/***/

			gsy->args.PrmInd.EddPortId = 0/*interface*/; /* an in-paramter! */

			gsy->args.PrmInd.DiagCnt = 0; /* sanity (an out-paramter!) */

			gsy->args.PrmInd.pDiag = (GSY_UPPER_PRM_IND_PTR_TYPE)lower_mem;

			cm_gsy_request_lower(channel, GSY_OPC_PRM_INDICATION_PROVIDE, 0, 0, gsy);
		}

		if( (CM_PORT_UINT)0x01/*provide_mask*/ != cm_gsy_prm_provide_mask(channel) ) {
			CM_FATAL(); /* a bug */
		}
	}
	else { /* handle indication and do re-provide */

		CM_GSY_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "gsy-prm.IND DiagCnt(%u) EddPortId(%u)"
			, gsy->args.PrmInd.DiagCnt
			, gsy->args.PrmInd.EddPortId
			);

		if( channel->state >= CM_CH_CLOSE ) {

			CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "channel is closing, freeing rqb"
				);

			CM_ASSERT(
				CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK
				|| CM_GSY_GET_RESPONSE(gsy) == GSY_RSP_OK_CANCEL
				);

			CM_ASSERT(cm_is_not_null(gsy->args.PrmInd.pDiag));

			{
			LSA_UINT16 rc;
			CM_GSY_FREE_LOWER_MEM (& rc, gsy->args.PrmInd.pDiag, channel->sysptr);
			CM_ASSERT (rc == LSA_RET_OK);
			}

			cm_gsy_free_rqb(channel, gsy);
		}
		else if( CM_GSY_GET_RESPONSE(gsy) != GSY_RSP_OK ) {

			CM_FATAL(); /* what? why? */
		}
		else {

			if( gsy->args.PrmInd.DiagCnt > GSY_PRM_IND_MAX_ENTRIES ) {
				CM_FATAL();
			}

			/***/

			{
			CM_CHANNEL_PTR_TYPE pd_channel = channel->lower.channel[CM_PATH_TYPE_PD];

			if( cm_is_null(pd_channel) )
			{
				CM_GSY_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_CHAT, "gsy-prm.IND but no pd-channel, ignored");
			}
			else
			{
				cm_pd_gsy_prm_indication_done(pd_channel, &gsy->args.PrmInd);
			}
			}

			/***/

			/*
			gsy->args.PrmInd.EddPortId = unchanged (an in-parameter!)
			*/

			gsy->args.PrmInd.DiagCnt = 0; /* sanity (an out-paramter!) */

			cm_gsy_request_lower(channel, GSY_OPC_PRM_INDICATION_PROVIDE, 0, 0, gsy); /* re-provide */
		}
	}
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_gsy_prm_prepare (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (cm_is_not_null(gsy));

	/* no args */

	cm_gsy_request_lower(channel, GSY_OPC_PRM_PREPARE, 0, 0, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_gsy_prm_write (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (cm_is_not_null(gsy));

	gsy->args.PrmRecord.EddPortId = edd_port_id;
	gsy->args.PrmRecord.Slot = slot_number;
	gsy->args.PrmRecord.Subslot = subslot_number;
	gsy->args.PrmRecord.RecordIndex = record_index;
	gsy->args.PrmRecord.RecordDataLen = record_data_length;

	/* record_data used as reference, check this */
	gsy->args.PrmRecord.pRecordData = CM_CAST_GSY_MEM_U8_PTR (record_data);

	cm_gsy_request_lower(channel, GSY_OPC_PRM_WRITE, 0, 0, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_gsy_prm_end (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (cm_is_not_null(gsy));

#if GSY_PRM_PORT_IS_COMPACT != 0 || GSY_PRM_PORT_IS_MODULAR != 1
#error "GSY_PRM_PORT_IS_COMPACT/GSY_PRM_PORT_IS_MODULAR are not 0/1"
#endif

	cm_pd_prm_set_prm_port_flags (channel, gsy->args.PrmEnd.IsModularPort
		, sizeof (gsy->args.PrmEnd.IsModularPort), sizeof (gsy->args.PrmEnd.IsModularPort[0])
		, CM_PDEV_PROP_TYPE_MODULAR
		);

	cm_gsy_request_lower(channel, GSY_OPC_PRM_END, 0, 0, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_gsy_prm_commit (
	CM_CHANNEL_PTR_TYPE channel
) {
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (cm_is_not_null(gsy));

#if GSY_PRM_APPLY_RECORD_PORT_PARAMS != 0 || GSY_PRM_APPLY_DEFAULT_PORT_PARAMS != 1
#error "GSY_PRM_APPLY_RECORD_PORT_PARAMS/GSY_PRM_APPLY_DEFAULT_PORT_PARAMS are not 0/1"
#endif

	cm_pd_prm_set_prm_port_flags (channel, gsy->args.PrmCommit.ApplyDefaultPortparams
		, sizeof (gsy->args.PrmCommit.ApplyDefaultPortparams), sizeof (gsy->args.PrmCommit.ApplyDefaultPortparams[0])
		, CM_PDEV_PROP_DEFAULT_ENABLED
		);

	cm_gsy_request_lower(channel, GSY_OPC_PRM_COMMIT, 0, 0, gsy);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16
cm_gsy_prm_read (
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				edd_port_id,
	LSA_UINT16				slot_number,
	LSA_UINT16				subslot_number,
	LSA_UINT32				record_index,
	LSA_UINT32				record_data_length,
	CM_UPPER_MEM_PTR_TYPE	record_data
) {
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	gsy = cm_gsy_alloc_rqb (channel);
	if (cm_is_null (gsy)) {
		return CM_ERR_ALLOC_LOWER;
	}

	gsy->args.PrmRecord.EddPortId = edd_port_id;
	gsy->args.PrmRecord.Slot = slot_number;
	gsy->args.PrmRecord.Subslot = subslot_number;
	gsy->args.PrmRecord.RecordIndex = record_index;
	gsy->args.PrmRecord.RecordDataLen = record_data_length;

	/* record_data used as reference, check this */
	gsy->args.PrmRecord.pRecordData = CM_CAST_GSY_MEM_U8_PTR (record_data);

	cm_gsy_request_lower(channel, GSY_OPC_PRM_READ, 0, 0, gsy);

	return CM_OK;
}

/*-----------------------------------------------------------------------*/

LSA_UINT16
cm_gsy_ptcp_master_control (
	CM_CHANNEL_PTR_TYPE		channel,
	CM_UPPER_PD_PTCP_MASTER_CONTROL_CONST_PTR_TYPE ptcp_master_control
) {
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy;

	gsy = cm_gsy_alloc_rqb(channel);
	if (cm_is_null (gsy)) {
		return CM_ERR_ALLOC_LOWER;
	}

	gsy->args.MasterControl.Mode      = ptcp_master_control->mode;
	gsy->args.MasterControl.UTCOffset = ptcp_master_control->utc_offset;

	cm_gsy_request_lower(channel, GSY_OPC_MASTER_CONTROL, 0, 0, gsy);

	return CM_OK_PENDING;
}

/*===========================================================================*/
/*=== PULL/PLUG =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_gsy_pullplug_change_port (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8  submodule_state,
	LSA_UINT8  apply_default_portparams
	)
{
	CM_CHANNEL_PTR_TYPE  gsy_channel = cm_gsy_channel_get(channel);
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy = gsy_channel->lower.prealloc.gsy;

	CM_ASSERT (cm_is_not_null(gsy));

	gsy->args.PrmChangePort.PortID = channel->usr.pd.pullplug.port->port_id;
	gsy->args.PrmChangePort.ModuleState = submodule_state;

#if GSY_PRM_APPLY_RECORD_PORT_PARAMS != 0 || GSY_PRM_APPLY_DEFAULT_PORT_PARAMS != 1
#error "GSY_PRM_APPLY_RECORD_PORT_PARAMS/GSY_PRM_APPLY_DEFAULT_PORT_PARAMS are not 0/1"
#endif

	gsy->args.PrmChangePort.ApplyDefaultPortparams = apply_default_portparams;

	cm_gsy_request_lower (channel, GSY_OPC_PRM_CHANGE_PORT, 0, 0, gsy);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_gsy_pullplug_change_port_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_CONST_PTR_TYPE  gsy
	)
{
	if (cm_pd_sc_map_response(LSA_COMP_ID_GSY, CM_GSY_GET_RESPONSE (gsy)) != CM_OK)
	{
		CM_FATAL1 (CM_GSY_GET_RESPONSE (gsy));
	}

#if GSY_PRM_PORT_PARAMS_APPLICABLE != 0 || GSY_PRM_PORT_PARAMS_NOT_APPLICABLE != 1
#error "GSY_PRM_PORT_PARAMS_APPLICABLE/GSY_PRM_PORT_PARAMS_NOT_APPLICABLE are not 0/1"
#endif

	cm_pd_pullplug_sm_trigger (channel, CM_PD_SCBF_GSY, gsy->args.PrmChangePort.PortparamsNotApplicable);
}

/*===========================================================================*/
/*=== GSY REQUEST LOWER =====================================================*/
/*===========================================================================*/

static LSA_VOID
cm_gsy_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_GSY_LOWER_RQB_PTR_TYPE gsy
) {
	CM_CHANNEL_PTR_TYPE gsy_channel = cm_gsy_channel_get(channel);

	CM_GSY_SET_USER_ID_HANDLE_DEV_AR (gsy, channel->my_handle, device_nr, ar_nr);

	CM_GSY_SET_HANDLE (gsy, gsy_channel->lower.handle);
	CM_GSY_SET_OPCODE (gsy, opcode);

	CM_GSY_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, ">>> gsy-rqb(0x%08x) gsy-opc(%u) f-h(%u) f-dev(%u) f-ar(%u)"
		, gsy, CM_GSY_GET_OPCODE (gsy)
		, CM_GSY_GET_USER_ID_HANDLE (gsy), CM_GSY_GET_USER_ID_DEV (gsy), CM_GSY_GET_USER_ID_AR (gsy)
		);

	CM_GSY_REQUEST_LOWER (gsy, gsy_channel->sysptr);
}


/*===========================================================================*/
/*=== GSY LOWER DONE =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_gsy_request_lower_done (
	CM_GSY_LOWER_RQB_PTR_TYPE  gsy
) {
	CM_CHANNEL_PTR_TYPE  channel;

	if( cm_is_null(gsy) ) {

		CM_GSY_TRACE_00 (0, LSA_TRACE_LEVEL_FATAL,
			"cm_is_null(gsy)"
			);

		CM_FATAL ();
		return;
	}

	channel = cm_channel_from_handle (CM_GSY_GET_USER_ID_HANDLE (gsy));

	if( cm_is_null (channel) ) {

		CM_GSY_TRACE_01 (0, LSA_TRACE_LEVEL_FATAL,
			"invalid handle(%u)",
			CM_GSY_GET_USER_ID_HANDLE (gsy)
			);

		CM_FATAL ();
		return;
	}

	/***/

	switch (CM_GSY_GET_RESPONSE (gsy)) {
	case GSY_RSP_OK:
	case GSY_RSP_OK_CANCEL:
		CM_GSY_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "<<< gsy-rqb(0x%08x) gsy-opc(%u) gsy-rsp(%u)  t-h(%u)"
			, gsy, CM_GSY_GET_OPCODE (gsy), CM_GSY_GET_RESPONSE (gsy)
			, CM_GSY_GET_USER_ID_HANDLE (gsy)
			);
		break;

	default:
		CM_GSY_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "<<< gsy-rqb(0x%08x) gsy-opc(%u) gsy-rsp(%u)  t-h(%u)"
			, gsy, CM_GSY_GET_OPCODE (gsy), CM_GSY_GET_RESPONSE (gsy)
			, CM_GSY_GET_USER_ID_HANDLE (gsy)
			);
		break;
	}

	/***/

	switch( channel->path_type ) {

	case CM_PATH_TYPE_GSY:

		switch (CM_GSY_GET_OPCODE (gsy)) {

		case GSY_OPC_OPEN_CHANNEL:
			cm_gsy_channel_open_done (channel, gsy);
			break;

		case GSY_OPC_CLOSE_CHANNEL:
			cm_gsy_channel_close_done (channel, gsy);
			break;

		case GSY_OPC_DIAG_INDICATION_PROVIDE:
			cm_gsy_diag_indication (channel, gsy);
			break;

		case GSY_OPC_PRM_INDICATION_PROVIDE:
			cm_gsy_prm_indication (channel, gsy);
			break;

		default:
			CM_GSY_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL,
				"unhandled opcode(%u)",
				CM_GSY_GET_OPCODE (gsy)
				);
			CM_FATAL (); /* should not happen */
			break;
		}
		break;

	case CM_PATH_TYPE_PD:

		switch (CM_GSY_GET_OPCODE (gsy)) {

		case GSY_OPC_PRM_PREPARE:
			cm_pd_gsy_prm_prepare_done (channel, gsy);
			break;

		case GSY_OPC_PRM_WRITE:
			cm_pd_gsy_prm_write_done (channel, gsy);
			break;

		case GSY_OPC_PRM_END:
			cm_pd_gsy_prm_end_done (channel, gsy);
			break;

		case GSY_OPC_PRM_COMMIT:
			cm_pd_gsy_prm_commit_done (channel, gsy);
			break;

		case GSY_OPC_PRM_READ:
			cm_pd_gsy_prm_read_done (channel, gsy);
			break;

		case GSY_OPC_PRM_CHANGE_PORT:
			cm_gsy_pullplug_change_port_done (channel, gsy);
			break;

		case GSY_OPC_MASTER_CONTROL:
			cm_pd_gsy_ptcp_master_control_done (channel, gsy);
			break;

		default:
			CM_GSY_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "unhandled opcode(%u)"
				, CM_GSY_GET_OPCODE (gsy)
				);
			CM_FATAL (); /* should not happen */
			break;
		}
		break;

	default:
		CM_GSY_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "wrong path-type(%u)"
			, channel->path_type
			);
		CM_FATAL (); /* should not happen */
		break;
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ GSY; CM_CFG_USE_GSY=" CM_STRINGIFY(CM_CFG_USE_GSY))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_GSY */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
