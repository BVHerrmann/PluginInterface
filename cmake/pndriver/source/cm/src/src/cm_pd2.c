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
/*  F i l e               &F: cm_pd2.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-PhysicalDevice things                                                 */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  802
#define CM_MODULE_ID       802

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/
/*=== DEFINES AND FORWARDS ==================================================*/
/*===========================================================================*/

static LSA_VOID
cm_pd_prm_write_multiple_next (
	CM_CHANNEL_PTR_TYPE  channel
);

static  LSA_UINT16
cm_pd_prm_write_check_ownership (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_CONST_PTR_TYPE  rb,
	LSA_BOOL is_server /* true if CM_OPC_PD_PRM_WRITE_SERVER */
);

static  LSA_UINT16
cm_pd_prm_write_internal (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

static  LSA_UINT32
cm_pd_record_index_classify_adapt ( /* TIA 1167600 */
	LSA_UINT32  cls,
	CM_PD_PORT_PTR_TYPE  port
);

/*===========================================================================*/
/*=== PORT FUNCTIONS ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_port_init (
	CM_PD_PORT_PTR_TYPE port
) {
	port->addr.slot_nr = 0;
	port->addr.subslot_nr = 0;	/* format: 0x8ipp/0x9jpp */
	port->addr.pdev_properties = 0;
	port->addr.im0_bits = 0;

	port->addr.mod_ident = 0;
	port->addr.sub_ident = 0;

	port->diag.update_allowed = LSA_TRUE;
	port->diag.server_in_sync = LSA_TRUE;

	CmListInitialize (& port->diag.diag_list);
	CmListInitialize (& port->diag.diag_list_server);
	CmListInitialize (& port->diag.diag_list_user);
	CmListInitialize (& port->diag.diag_list_prm_sc);

	port->misc.sync_led_info           = CM_PD_SYNC_LED_INFO_NOT_CONFIGURED;
	port->misc.sync_led__is_rate_valid = LSA_FALSE;
	port->misc.sync_led__rcv_sync_prio = 0;
	port->misc.sync_led__local_prio    = 0;
	port->misc.sync_led__pll_state     = CM_PD_PLL_STATE_UNKNOWN;

	port->misc.MRPRingPort = EDD_MRP_NO_RING_PORT;

	port->misc.link_status_ind = LSA_NULL;
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_port_undo_init (
	CM_PD_PORT_CONST_PTR_TYPE port
) {
	cm_pd_diag_list_clear (& port->diag.diag_list, 0);
	cm_pd_diag_list_clear (& port->diag.diag_list_server, 0);
	cm_pd_diag_list_clear (& port->diag.diag_list_user, 0);
	cm_pd_diag_list_clear (& port->diag.diag_list_prm_sc, 0);

	CM_ASSERT (cm_is_null (port->misc.link_status_ind));
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE
cm_pd_port_if__nn (
	CM_CHANNEL_CONST_PTR_TYPE channel
	)
{
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id (channel, 0/*interface*/);
	if (cm_is_null (port))
	{
		CM_FATAL();
	}
	return port;
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE
cm_pd_port_from_id (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 port_id /* 0..interface   n..port 1 .. port $n */
	)
{
	if (port_id <= channel->usr.pd.port_count)
	{
		CM_PD_PORT_PTR_TYPE  port = & channel->usr.pd.port_array_ptr[port_id];
		return port;
	}
	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE
cm_pd_port_from_id__nn (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 port_id
) {
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id (channel, port_id);
	if (cm_is_null (port))
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "port(%u) out of range", port_id
			);
		CM_FATAL(); /* should not happen */
		return LSA_NULL; /* not reached */
	}
	return port;
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE
cm_pd_port_from_addr (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	LSA_UINT16  i;

	for (i = 0; i <= channel->usr.pd.port_count; ++i)
	{
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (channel, i);

		if (port->addr.slot_nr == slot_nr  &&  port->addr.subslot_nr == subslot_nr)
		{
			return port;
		}
	}

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
		, "addr-mapping not found, slot(%u) subslot(0x%x)"
		, slot_nr, subslot_nr
		);
	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/
CM_PD_PORT_PTR_TYPE
cm_pd_port_from_addr__nn (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
)
{
	LSA_UINT16  i;

	for (i = 0; i <= channel->usr.pd.port_count; ++i)
	{
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_id__nn (channel, i);

		if (port->addr.slot_nr == slot_nr  &&  port->addr.subslot_nr == subslot_nr)
		{
			return port;
		}
	}

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
		, "addr-mapping not found, slot(%u) subslot(0x%x)"
		, slot_nr, subslot_nr
		);
	CM_FATAL();
	return LSA_NULL;
}


/*===========================================================================*/
/*=== EVENT FUNCTIONS =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_pd_event_provide (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_ASSERT (CM_RQB_GET_OPCODE(rb) == CM_OPC_PD_PROVIDE_EVENT);

	CmListInsertTail (& channel->usr.pd.req.eventQ, (CM_LIST_ENTRY_PTR_TYPE)rb);
}

/*----------------------------------------------------------------------------*/

CM_UPPER_RQB_PTR_TYPE
cm_pd_event_get (
	CM_CHANNEL_CONST_PTR_TYPE  channel
	)
{
	if (CmListIsEmpty (& channel->usr.pd.req.eventQ))
	{
		return LSA_NULL;
	}
	else
	{
		CM_UPPER_RQB_PTR_TYPE  rb;

		CmListRemoveHead (& channel->usr.pd.req.eventQ, rb, CM_UPPER_RQB_PTR_TYPE);
		CM_ASSERT (cm_is_not_null (rb));

		return rb;
	}
}




/*----------------------------------------------------------------------------*/
static LSA_BOOL
cm_pd_set_send_clock_allowed (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  send_clock
) {
	if ( cm_edd_can_set_send_clock (channel) )
	{
		return LSA_TRUE;
	}
	else if ( channel->usr.pd.clock.send_clock == send_clock )
	{
		return LSA_TRUE;
	}
	else
	{
		return LSA_FALSE;
	}
}


/*===========================================================================*/
/*=== PD-PRM callback-functions "cm_cmpd_prm_xxx" ===========================*/
/*===========================================================================*/


/*----------------------------------------------------------------------------*/
LSA_UINT16
cm_cmpd_prm_write (
	CM_PD_PRM_CONST_PTR_TYPE  pdprm,
	CM_PD_PORT_CONST_PTR_TYPE  port,
	CM_UPPER_PD_PRM_RDWR_CONST_PTR_TYPE  prm_rdwr
) {
	CM_PD_RECLIST_PTR_TYPE  reclist = & pdprm->channel->usr.pd.reclist;

	LSA_UINT32  record_data_length = prm_rdwr->record_data_length;
	CM_COMMON_MEM_U8_PTR_TYPE  record_data = prm_rdwr->record_data;

	CM_ASSERT (cm_is_not_null (port));

	if (cm_is_null (record_data)) {
		return CM_ERR_PARAM; /* should not happen */
	}

	switch (prm_rdwr->record_index)
	{
	case 0x802C: /* PDIRData */
		if (port->port_id == 0)
		{
			reclist->prm_data.do_irt = LSA_TRUE; /* note: needed because "at least one port is an irt-port" is signalled delayed by link.ind */
			reclist->prm_data.ir_data_uuid = cm_uuid_nil_uuid;

			if (record_data_length > 0UL + 2+2+1+1 +1+1 +2+2 + (2+2+1+1 +1+1 +16)) /* more check is done by subcomponent */
			{
				LSA_UINT16 block_version_ir, block_version_global;
				/*	PDIRData[VersionLow == 1]  ::=
				//		BlockHeader, Padding, Padding, SlotNumber, SubslotNumber, PDIRGlobalData, ...
				//
				//	PDIRGlobalData[VersionLow == 1 or VersionLow == 2]  ::=
				//		BlockHeader, Padding, Padding, IRDataUUID, MaxBridgeDelay, ...
				*/
				block_version_ir		= CM_GET16_NTOH (record_data,  4);
				block_version_global	= CM_GET16_NTOH (record_data, 16);
				if (block_version_ir == 0x0101
				&& (block_version_global == 0x0101 || block_version_global == 0x0102))
				{
					CM_GET_UUID_NTOH (& reclist->prm_data.ir_data_uuid, record_data, 20);
				}
			}
			else
			{
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "record_data_length(%u)", record_data_length);
				return CM_ERR_PRM_DATA;
			}
		}
		break;

	case 0x00010000: /* SendClock */
		if (port->port_id != 0)
		{
			return CM_ERR_PRM_PORTID;
		}
		else
		{
			LSA_UINT16 block_type, block_length, block_version, send_clock, sendclock_prop,	reserved;

				/* SendClock ::= BlockHeader, SendClockFactor, SendClockProperties, Reserved, [Data*] */
			if( record_data_length < 0UL + 2+2+1+1 + 2+2+2 ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "record_data_length(%u)", record_data_length);
				return CM_ERR_PRM_DATA;
			}

			block_type		= CM_GET16_NTOH (record_data, 0);
			block_length	= CM_GET16_NTOH (record_data, 2);
			block_version	= CM_GET16_NTOH (record_data, 4);
			send_clock		= CM_GET16_NTOH (record_data, 6);
			sendclock_prop	= CM_GET16_NTOH (record_data, 8);
			reserved		= CM_GET16_NTOH (record_data, 10);

			if( 0UL + 2+2 + block_length != record_data_length ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "block_length(%u)", block_length);
				return CM_ERR_PRM_DATA;
			}
			if( block_type != CM_BLOCK_TYPE_SEND_CLOCK ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "block_type(0x%x)", block_type);
				return CM_ERR_PRM_BLOCK;
			}
			if( block_version != 0x0100 ) {
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "block_version(0x%x)", block_version);
				return CM_ERR_PRM_BLOCK;
			}
			switch (sendclock_prop) {
			case CM_SENDCLOCK_PROP_SC_VAR_RR_VAR:
			case CM_SENDCLOCK_PROP_SC_VAR_RR_FIX:
			case CM_SENDCLOCK_PROP_SC_FIX_RR_VAR:
			case CM_SENDCLOCK_PROP_SC_FIX_RR_FIX:
				/* we don't check for cm_edd_can_set_send_clock() here or else the user has to know which EDD is used */
				/* see the "correction" in cm_pd_sendclock_prop_sc_is_fixed() */
				break;

			default:
				CM_PD_TRACE_01 (pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "unknown sendclock_prop(0x%x)", sendclock_prop
					);
				return CM_ERR_PRM_BLOCK;
			}

			if( reserved != 0x0000 ) {
				CM_PD_TRACE_00(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP, "reserved not zero");
				return CM_ERR_PRM_BLOCK;
			}

			if( ! cm_edd_send_clock_range_ok_hw(pdprm->channel, send_clock) ) {

				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "unsupported sendclock(%u)"
					, send_clock
					);
				return CM_ERR_PRM_BLOCK;
			}

			if ( ! cm_pd_set_send_clock_allowed (pdprm->channel, send_clock) ) /* TIA 1157276 */
			{
				CM_PD_TRACE_01(pdprm->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "setting sendclock(%u) not allowed"
					, send_clock
					);
				return CM_ERR_PRM_BLOCK;
			}

			/* note: may be written several times in one prm-phase (semantics of RPC idempotent-rerun) */

			reclist->prm_data.scr_send_clock     = send_clock;
			reclist->prm_data.scr_sendclock_prop = sendclock_prop;
		}
		break;

	default:
		/* not scanned */
		break;
	}

#if CM_CFG_USE_MRP
	if ((prm_rdwr->cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_MRP)
	{
		reclist->prm_data.do_mrp_record = LSA_TRUE;
	}
#endif

	return CM_OK;
}


/*===========================================================================*/
/*=== PD-SendClock ==========================================================*/
/*===========================================================================*/

LSA_UINT16
cm_pd_sendclock_prop_get (
	CM_CHANNEL_CONST_PTR_TYPE  channel
) {
	return channel->usr.pd.clock.sendclock_prop;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pd_sendclock_prop_sc_is_fixed (
	CM_CHANNEL_PTR_TYPE  channel
) {
	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

	/*
	 * note: cm_cmpd_prm_write() does not check against cm_edd_can_set_send_clock(channel)
	 */

	if( cm_edd_can_set_send_clock(channel) ) { /* use SendClockProperties */

		switch (channel->usr.pd.clock.sendclock_prop) {
		case CM_SENDCLOCK_PROP_SC_FIX_RR_VAR:
		case CM_SENDCLOCK_PROP_SC_FIX_RR_FIX:
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "send_clock(%u) is fixed", channel->usr.pd.clock.send_clock
				);
			return LSA_TRUE;

		case CM_SENDCLOCK_PROP_SC_VAR_RR_VAR:
		case CM_SENDCLOCK_PROP_SC_VAR_RR_FIX:
		default:
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "send_clock(%u) is variable", channel->usr.pd.clock.send_clock
				);
			return LSA_FALSE;
		}
	}
	else { /* no matter what the SendClockProperties say, if edd cannot...*/

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "send_clock(%u) is fixed (because edd cannot set send-clock)", channel->usr.pd.clock.send_clock
			);
		return LSA_TRUE;
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL
cm_pd_sendclock_prop_rr_is_fixed (
	CM_CHANNEL_CONST_PTR_TYPE  channel
) {
	CM_ASSERT (channel->path_type == CM_PATH_TYPE_PD);

	switch (channel->usr.pd.clock.sendclock_prop) {
	case CM_SENDCLOCK_PROP_SC_FIX_RR_FIX:
	case CM_SENDCLOCK_PROP_SC_VAR_RR_FIX:
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "reduction_ratio is fixed"
			);
		return LSA_TRUE;

	case CM_SENDCLOCK_PROP_SC_FIX_RR_VAR:
	case CM_SENDCLOCK_PROP_SC_VAR_RR_VAR:
	default:
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "reduction_ratio is variable"
			);
		return LSA_FALSE;
	}
}


/*===========================================================================*/
/*=== iterator ==============================================================*/
/*===========================================================================*/

LSA_UINT32
cm_pd_queue_iterator_next(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE pd
) {
	/*lint --e{801} Use of goto is deprecated - request 1502507*/
	CM_PD_ITER_PTR_TYPE iter = &channel->usr.pd.req.iter;

	/*
	 * NOTE: copy/paste/adapt from cm_sv_record_read_pdev_next()
	 */

	if( iter->pdev.state == CM_PD_ITER_PDEV_IDLE ) {
		CM_FATAL();
	}

	if( cm_is_null(pd) ) { /* first call */

		/* refactor! saving/restoring the original request in iter->pdev.org is difficult to understand */

		pd = channel->usr.pd.req.current;

		if( iter->pdev.offs_tot != 0 ) {
			CM_FATAL(); /* see caller */
		}

		iter->pdev.offs_tot = 0;
	}
	else { /* confirmation (callback from cm-pd) */

		CM_ASSERT(pd == channel->usr.pd.req.current);

		goto _confirmation;
	}

	/***/

	for( iter->port_id = 0; iter->port_id <= channel->usr.pd.port_count; ++iter->port_id ) {

		{ /* block for easier diffing */
			{ /* block for easier diffing */

				{
				CM_PD_PORT_PTR_TYPE port = cm_pd_port_from_id__nn (channel, iter->port_id);

				if (CM_PDEV_STATE_IS_PULLED (port->addr.pdev_properties)) {
					continue;
				}

				iter->api = 0/*!*/;
				iter->slot_nr = port->addr.slot_nr;
				iter->subslot_nr = port->addr.subslot_nr;
				}

				for(;;) { /* for each block */

					CM_PD_TRACE_06(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "pdev-iter: ap/sl/su(%u/%u/%u) state(%u) offs/len(%u/%u)"
						, iter->api, iter->slot_nr, iter->subslot_nr
						, iter->pdev.state, iter->pdev.offs_tot, iter->pdev.org.record_data_length
						);

					{
					LSA_UINT32 pnio_err = CM_PNIO_ERR_NONE; /* assume ok */
					LSA_UINT16 index;

					switch( iter->pdev.state ) {

					/* PDevData (0xF831) */

					case CM_PD_ITER_PDEV_BlockHeaderPDev:
						{
						CLRPC_UPPER_MEM_U8_PTR_TYPE data = iter->pdev.org.record_data;
						/*lint --e{713} Loss of precision request 1524437*/
						LSA_UINT32 offs = iter->pdev.offs_tot;

						if( offs + 2+2+1+1 + 1+1 > iter->pdev.org.record_data_length ) {

							CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.org.record_data_length, iter->pdev.state
								);

							pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(11);
							index = 0; /* don't care */
							break;
						}

						iter->pdev.offs_blk = offs;

						CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_PD_PDEVDATA);
						offs += 2;
						CM_PUT16_HTON(data, offs, 0); /* length is written below */
						offs += 2;
						CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
						offs += 1+1;

						CM_PUT16_HTON(data, offs, 0); /* Padding, Padding */
						offs += 1+1;

						iter->pdev.offs_tot = offs;
						}

						iter->pdev.state += 1; /* next */
						continue; /* not break */

					case CM_PD_ITER_PDEV_IRData:
						index = 0x802C;
						break;

					case CM_PD_ITER_PDEV_PDSyncDataClock:
						index = 0x802D;
						break;

					case CM_PD_ITER_PDEV_PDIRSubframeData:
						index = 0x8020;
						break;

					/* Header for PDRealData and PDExpectedData */

					case CM_PD_ITER_PDEV_MultipleBlockHeaderReal:
					case CM_PD_ITER_PDEV_MultipleBlockHeaderExpected:
						{
						CLRPC_UPPER_MEM_U8_PTR_TYPE data = iter->pdev.org.record_data;
						/*lint --e{713} Loss of precision request 1524437*/
						LSA_UINT32 offs = iter->pdev.offs_tot;

						if( offs + 2+2+1+1 + 1+1 + 4+2+2 > iter->pdev.org.record_data_length ) {

							CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.org.record_data_length, iter->pdev.state
								);

							pnio_err = CM_PNIO_FAULTY_Record__BufferTooSmall(11);
							index = 0; /* don't care */
							break;
						}

						iter->pdev.offs_blk = offs;

						CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_MULTIPLE_BLOCK_HEADER);
						offs += 2;
						CM_PUT16_HTON(data, offs, 0); /* patched below */
						offs += 2;
						CM_PUT16_HTON(data, offs, 0x0100/*V1.0*/);
						offs += 1+1;

						CM_PUT16_HTON(data, offs, 0); /* Padding, Padding */
						offs += 1+1;

						CM_PUT32_HTON(data, offs, iter->api);
						offs += 4;
						CM_PUT16_HTON(data, offs, iter->slot_nr);
						offs += 2;
						CM_PUT16_HTON(data, offs, iter->subslot_nr);
						offs += 2;

						iter->pdev.offs_tot = offs;
						}

						iter->pdev.state += 1; /* next */
						continue; /* not break */

					/* PDRealData (0xF841) */

					case CM_PD_ITER_PDEV_PDPortDataReal:
						index = 0x802A;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataReal:
						index = 0x8050;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpDataReal:
						index = 0x8054;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataReal:
						index = 0x8060; /* note: special handling removed with TIA 1337858 (V6.0i2.xx) */
						break;
					case CM_PD_ITER_PDEV_PDInterfaceDataReal:
						index = 0x8080;
						break;
					case CM_PD_ITER_PDEV_PDPortStatistic:
						index = 0x8072; /* note: PDPortStatistic is for the interface too */
						break;
#if 0 /* TIA 2704139 - MRP Interconnection not fully supported yet */
					case CM_PD_ITER_PDEV_PDPortMrpIcDataReal:
						index = 0x8057;
						break;
#endif

					/* PDExpectedData (0xF842) */

					case CM_PD_ITER_PDEV_PDPortDataCheck:
						index = 0x802B;
						break;
					case CM_PD_ITER_PDEV_PDPortDataAdjust:
						index = 0x802F;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataAdjust:
						index = 0x8052;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceMrpDataCheck:
						index = 0x8051;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpDataAdjust:
						index = 0x8053;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataAdjust:
						index = 0x8062;
						break;
					case CM_PD_ITER_PDEV_PDPortFODataCheck:
						index = 0x8061;
						break;
					case CM_PD_ITER_PDEV_PDNCDataCheck:
						index = 0x8070;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceFSUDataAdjust:
						index = 0x8090;
						break;
					case CM_PD_ITER_PDEV_PDInterfaceAdjust:
						index = 0x8071;
						break;
#if 0 /* TIA 2704139 - MRP Interconnection not fully supported yet */
					case CM_PD_ITER_PDEV_PDPortMrpIcDataAdjust:
						index = 0x8055;
						break;
					case CM_PD_ITER_PDEV_PDPortMrpIcDataCheck:
						index = 0x8056;
						break;
#endif

					/***/

					default:
						CM_FATAL();
						return 0;
					}

					/***/

					if( pnio_err == CM_PNIO_ERR_NONE ) {

						LSA_UINT32 cls = cm_record_index_classify (channel, 0/*!*/, index, CM_VENDOR_ID_DONTCARE);

						/* AP00431325, avoid reads that will fail anyway */

						if ((cls & CM_REC_IDX_READ) != CM_REC_IDX_READ) {

							goto _next; /* e.g. POF not configured */
						}

						if( (iter->subslot_nr & 0x00FF) == 0x0000 ) { /* is an interface-submodule, see cm_pdsv_range_ok() */

							if( (cls & CM_REC_IDX_INTERFACE) == 0 ) {

								goto _next;
							}
						}
						else { /* is a port-submodule */

							if( (cls & CM_REC_IDX_PORT) == 0 ) {

								goto _next;
							}
						}

						/***/

						{
						CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = pd->args.pd.prm_rdwr;
						LSA_UINT16 rc;

						prm_rdwr->slot_nr = iter->slot_nr;
						prm_rdwr->subslot_nr = iter->subslot_nr;

						prm_rdwr->record_index = index;

						prm_rdwr->record_data = iter->pdev.org.record_data + iter->pdev.offs_tot;
						prm_rdwr->record_data_length = iter->pdev.org.record_data_length - iter->pdev.offs_tot;

						prm_rdwr->is_local = CM_PD_IS_LOCAL_YES;
						prm_rdwr->cls = cls;

						/***/

						rc = cm_pd_prm_sc_read (& channel->usr.pd.pdprm, pd, iter->port_id);

						if (rc != CM_OK_PENDING) {

							goto _confirmation; /* note: cm_pd_prm_sc_read() did CM_RQB_SET_RESPONSE() */
						}
						}
					}

					if( pnio_err != CM_PNIO_ERR_NONE ) { /* abort iteration */

						iter->pdev.state = CM_PD_ITER_PDEV_IDLE;
					}

					return pnio_err; /* CM_PNIO_ERR_NONE .. wait for confirmation */
					}

_confirmation: /* confirmation is back from cm-pd */

					if( CM_RQB_GET_RESPONSE(pd) != CM_OK ) {

						CM_PD_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "reading index(0x%x) from sl/su(%u/%u) failed, pd-rsp(%u)"
							, pd->args.pd.prm_rdwr->record_index
							, pd->args.pd.prm_rdwr->slot_nr
							, pd->args.pd.prm_rdwr->subslot_nr
							, CM_RQB_GET_RESPONSE(pd)
							);

						if( CM_RQB_GET_RESPONSE(pd) == CM_ERR_PRM_DATA ) { /* data-length too short */

							CM_PD_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
								, "buffer too small, len(%u) state(%u)"
								, iter->pdev.org.record_data_length, iter->pdev.state
								);

							iter->pdev.state = CM_PD_ITER_PDEV_IDLE; /* abort iteration */

							return CM_PNIO_FAULTY_Record__BufferTooSmall(11);
						}
						else {

							/* ignored (e.g. index cannot be read from a port, ...) */
						}
					}
					else {

						iter->pdev.offs_tot += pd->args.pd.prm_rdwr->record_data_length;

						if( iter->pdev.offs_tot > iter->pdev.org.record_data_length ) {
							CM_FATAL(); /* a bug */
						}
					}

					/***/

_next:
					iter->pdev.state += 1; /* next */

					switch( iter->pdev.state ) {

					case CM_PD_ITER_PDEV_PDevDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {
							/*lint --e{713} Loss of precision request 1524437*/
							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									iter->pdev.org.record_data,
									iter->pdev.offs_blk + 2,
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_BlockHeaderPDev; /* setup for next subslot */
						break;

					case CM_PD_ITER_PDEV_PDRealDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 + 4+2+2 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									iter->pdev.org.record_data,
									(iter->pdev.offs_blk + 2),
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderReal; /* setup for next subslot */
						break;

					case CM_PD_ITER_PDEV_PDExpectedDataComplete:

						if( iter->pdev.offs_blk + 2+2+1+1 + 1+1 + 4+2+2 == iter->pdev.offs_tot ) { /* no embedded blocks */

							iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
						}
						else {

							LSA_UINT32 blklen = iter->pdev.offs_tot - iter->pdev.offs_blk - (2+2);

							if( blklen > 0xFFFF ) { /* very unlikely */

								iter->pdev.offs_tot = iter->pdev.offs_blk; /* rollback */
							}
							else { /* patch length */

								CM_PUT16_HTON(
									iter->pdev.org.record_data,
									(iter->pdev.offs_blk + 2),
									CM_CAST_U16(blklen)
									);
							}
						}

						iter->pdev.state = CM_PD_ITER_PDEV_MultipleBlockHeaderExpected; /* setup for next subslot */
						break;

					default:
						continue; /* read next index */
					}

					break; /* forever-loop */
				}
			}
		}
	}

	/***/

	iter->pdev.state = CM_PD_ITER_PDEV_IDLE;

	iter->pdev.org.record_data_length = iter->pdev.offs_tot; /* update */

	*channel->usr.pd.req.current->args.pd.prm_rdwr = iter->pdev.org; /* restore */

	/***/

	CM_ASSERT (pd == channel->usr.pd.req.current);

	cm_pd_req_callback (channel, CM_OK, pd);

	return CM_PNIO_ERR_NONE; /* completed */
}

static LSA_VOID
cm_pd_queue_iterator_first(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb,
	LSA_UINT16  first_state
) {
	CM_PD_ITER_PTR_TYPE iter = &channel->usr.pd.req.iter;
	LSA_UINT32 pnio_err;

	if( iter->pdev.state != CM_PD_ITER_PDEV_IDLE ) {
		CM_FATAL();
	}

	iter->pdev.state = first_state;

	iter->pdev.org = *rb->args.pd.prm_rdwr; /* save original request */

	iter->pdev.offs_tot = 0; /* sanity */

	pnio_err = cm_pd_queue_iterator_next(channel, LSA_NULL);

	if( pnio_err != CM_PNIO_ERR_NONE ) {

		LSA_UINT16 response;

		if( iter->pdev.state != CM_PD_ITER_PDEV_IDLE ) {
			CM_FATAL();
		}

		iter->pdev.org.record_data_length = 0; /* update */

		*channel->usr.pd.req.current->args.pd.prm_rdwr = iter->pdev.org; /* restore */

		/***/

		if( (pnio_err & 0xFFFFFF00) == CM_PNIO_FAULTY_Record__BufferTooSmall(0) ) {

			response = CM_ERR_PRM_DATA;
		}
		else {

			response = CM_ERR_PRM_OTHER;
		}

		/***/

		CM_ASSERT (rb == channel->usr.pd.req.current);

		cm_pd_req_callback (channel, response, rb);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_read_internal (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr);

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;

	if (cm_is_null (port))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
			);
		cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
		return;
	}

	if (CM_PDEV_STATE_IS_PULLED(port->addr.pdev_properties))
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PD-read: port_id(%u) pulled"
			, port->port_id
			);
		cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE, rb);
		return;
	}

	if (prm_rdwr->cls & (CM_REC_IDX_INTERFACE | CM_REC_IDX_PORT))
	{
		if (port->port_id == 0/*interface*/  &&  (prm_rdwr->cls & CM_REC_IDX_INTERFACE) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from interface, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}

		if (port->port_id != 0/*any port*/  &&  (prm_rdwr->cls & CM_REC_IDX_PORT) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from port, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}
	}

	/* CMSV-RD */
	prm_rdwr->cls = cm_pd_record_index_classify_adapt (prm_rdwr->cls, port);

	if ((prm_rdwr->cls & CM_REC_IDX_READ) != CM_REC_IDX_READ)
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PD-read: index(0x%x) not readable, response(%u)"
			, prm_rdwr->record_index, CM_ERR_PRM_INDEX
			);
		cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
		return;
	}

	{
		LSA_UINT16  rc = cm_pd_prm_sc_read (& channel->usr.pd.pdprm, rb, port->port_id);

		if (rc != CM_OK_PENDING)
		{
			cm_pd_req_callback (channel, rc, rb);
			return;
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_read_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr);

	if( channel->usr.pd.req.iter.pdev.state != CM_PD_ITER_PDEV_IDLE ) {
		CM_FATAL();
	}

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;

	if (cm_is_null (prm_rdwr->record_data))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-read: record_data == NULL");
		cm_pd_req_callback (channel, CM_ERR_PARAM, rb);
		return;
	}

	if (cm_is_not_null (port))
	{
		/* CMPD-RD & CMCL-RD */
		prm_rdwr->cls = cm_pd_record_index_classify_adapt (prm_rdwr->cls, port);

		if (CM_PDEV_STATE_IS_PULLED(port->addr.pdev_properties))
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "PD-read: port_id(%u) pulled"
				, port->port_id
				);
			cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE, rb);
			return;
		}
	}

	if ((prm_rdwr->cls & (CM_REC_IDX_INTERFACE | CM_REC_IDX_PORT)) != 0)
	{
		if (cm_is_null (port))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
				);
			cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
			return;
		}

		if (port->port_id == 0/*interface*/  &&  (prm_rdwr->cls & CM_REC_IDX_INTERFACE) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from interface, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}

		if (port->port_id != 0/*any port*/  &&  (prm_rdwr->cls & CM_REC_IDX_PORT) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: record_index(0x%x) not readable from port, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_PORTID, rb);
			return;
		}
	}

	if (! ((prm_rdwr->cls & CM_REC_IDX_READ) == CM_REC_IDX_READ)
	&&  ! (prm_rdwr->is_local == CM_PD_IS_LOCAL_YES && (prm_rdwr->cls & CM_REC_IDX_READ_LOCAL) == CM_REC_IDX_READ_LOCAL))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "PD-read: index(0x%x) not readable, response(%u)"
			, prm_rdwr->record_index, CM_ERR_PRM_INDEX
			);
		cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
		return;
	}

	if (prm_rdwr->cls & CM_REC_IDX_DIAG)
	{
		if (prm_rdwr->cls & CM_REC_IDX_8000)
		{
			if (cm_is_null (port))
			{
				CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
					);
				cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
				return;
			}

			{
				LSA_UINT32 offs = 0;
				LSA_BOOL ok;

				ok = cm_pd_diag_marshal_8000 (prm_rdwr->slot_nr, prm_rdwr->subslot_nr
					, prm_rdwr->record_data, prm_rdwr->record_data_length
					, &offs, &port->diag.diag_list, CM_CAST_U16(prm_rdwr->record_index)
					);

				if( ! ok )
				{
					cm_pd_req_callback (channel, CM_ERR_PRM_DATA, rb);
				}
				else
				{
					prm_rdwr->record_data_length = offs; /* update */
					cm_pd_req_callback (channel, CM_OK, rb);
				}
			}
		}
		else if ((prm_rdwr->cls & CM_REC_IDX_F000) || (prm_rdwr->cls & CM_REC_IDX_F800))
		{
			LSA_UINT32 offs = 0;
			LSA_BOOL ok = LSA_TRUE;
			LSA_UINT16 port_id;

			/*AP00383492*/
			for( port_id = 0; port_id <= channel->usr.pd.port_count; ++port_id )
			{
				CM_PD_PORT_PTR_TYPE iter_port = cm_pd_port_from_id__nn (channel, port_id);

				ok = cm_pd_diag_marshal_8000 (iter_port->addr.slot_nr, iter_port->addr.subslot_nr
					, prm_rdwr->record_data, prm_rdwr->record_data_length
					, &offs, &iter_port->diag.diag_list, CM_CAST_U16(prm_rdwr->record_index)
					);

				if( ! ok )
				{
					break;
				}
			}

			if( ! ok )
			{
				cm_pd_req_callback (channel, CM_ERR_PRM_DATA, rb);
			}
			else
			{
				prm_rdwr->record_data_length = offs; /* update */
				cm_pd_req_callback (channel, CM_OK, rb);
			}
		}
		else /* CM_REC_IDX_C000, CM_REC_IDX_E000 */
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: index(0x%x) unknown to CMPD", prm_rdwr->record_index
				);
			cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
		}
	}
	else if (prm_rdwr->cls & CM_REC_IDX_NORM)
	{
		switch (prm_rdwr->record_index)
		{
		case 0xF831: /* PDevData */
			cm_pd_queue_iterator_first (channel, rb, CM_PD_ITER_PDEV_BlockHeaderPDev);
			break;

		case 0xF841: /* PDRealData */
			cm_pd_queue_iterator_first (channel, rb, CM_PD_ITER_PDEV_MultipleBlockHeaderReal);
			break;

		case 0xF842: /* PDExpectedData */
			cm_pd_queue_iterator_first (channel, rb, CM_PD_ITER_PDEV_MultipleBlockHeaderExpected);
			break;

		default:
			cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
			break;
		}
	}
	else if (prm_rdwr->cls & CM_REC_IDX_PDEV)
	{
		if ((prm_rdwr->cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_NONE)
		{
			CM_FATAL(); /* see see cm_record_index_classify() */
		}

		if (cm_is_null (port))
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-read: no mapping, slot(%u) subslot(0x%x)", prm_rdwr->slot_nr, prm_rdwr->subslot_nr
				);
			cm_pd_req_callback (channel, CM_ERR_NOT_ACCESSIBLE/*!*/, rb);
			return;
		}

		{
			LSA_UINT16  rc = cm_pd_prm_sc_read (& channel->usr.pd.pdprm, rb, port->port_id);

			if (rc != CM_OK_PENDING)
			{
				cm_pd_req_callback (channel, rc, rb);
				return;
			}
		}
	}
	else
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-read: index(0x%x) unknown to CMPD", prm_rdwr->record_index
			);
		cm_pd_req_callback (channel, CM_ERR_PRM_INDEX, rb);
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_begin_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
) {
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	if (pdprm->occupant != CM_PD_OCCUPANT_NOBODY )
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-begin failed, PDev already occupied by (%u)", pdprm->occupant
			);
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

#if CM_CFG_MAX_CLIENTS
	{
	CM_CHANNEL_PTR_TYPE  cl_channel = cm_cl_channel_get (channel);

	if (cm_is_not_null (cl_channel))
	{
		CM_CL_DEVICE_PTR_TYPE  dev = cm_cl_dev_first(cl_channel);

		if (cm_is_not_null (dev))
		{
			CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-begin failed, devices already added"
				);
			cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
			return;
		}
	}
	}
#endif

	if (CmListIsEmpty(& channel->usr.pd.req.eventQ))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-begin failed, no indication resources");
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	if (! channel->usr.pd.reclist.is_valid)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-begin failed, reclist not valid");
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	pdprm->occupant = CM_PD_OCCUPANT_USER;

	cm_pd_reclist_prm_begin (channel, 0, 0);

	cm_pd_reclist_clear (channel, CM_PD_RECLIST_MIXIN); /* clear mixin records */

	cm_pd_req_callback (channel, CM_OK, rb);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_write_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb,
	LSA_BOOL is_server /* true if CM_OPC_PD_PRM_WRITE_SERVER */
	)
{
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	LSA_UINT16  rsp;

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;

	rsp = cm_pd_prm_write_check_ownership (channel, rb, is_server);

	if (CM_OK == rsp && prm_rdwr->mk_remanent != CM_PD_MK_REMANENT_NO && prm_rdwr->mk_remanent != CM_PD_MK_REMANENT_YES)
	{
		CM_PD_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "prm_write_user: !mk_remanent(%u)", prm_rdwr->mk_remanent);

		rsp = CM_ERR_PARAM;
	}

	if (rsp == CM_OK)
	{

		if ((prm_rdwr->cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_BBUF) /* TIA 1193013 */
		{
			/* note: prm_rdwr->record_data may be LSA_NULL */

			rsp = cm_pd_reclist_delete (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr, prm_rdwr->record_index);
		}
		else
		{
			rsp = cm_pd_prm_write_internal (channel, rb);
		}

		if (rsp == CM_OK_PENDING)
		{
			return;
		}

		if (rsp != CM_OK  &&  (prm_rdwr->cls & CM_REC_IDX_NONPRM) == 0)
		{
			CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr);

			if (cm_is_not_null (port))
			{
				CM_PD_PRM_DIAG_TYPE  prm_diag;

				CM_PD_PRM_DIAG_SET_WRITE_FAULT(&prm_diag, port->port_id, prm_rdwr->record_index, 0);

				cm_pd_prm_diag_add (&channel->usr.pd.pdprm, &prm_diag);
			}

			cm_pd_reclist_set_error (channel);
		}
	}

	cm_pd_req_callback (channel, rsp, rb);
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16
cm_pd_prm_write_check_ownership (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_CONST_PTR_TYPE  rb,
	LSA_BOOL is_server /* true if CM_OPC_PD_PRM_WRITE_SERVER */
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;

	if ((prm_rdwr->cls & CM_REC_IDX_NONPRM) == 0 && (prm_rdwr->cls & (CM_REC_IDX_PDEV | CM_REC_IDX_READ)) == 0) /* prm-records need ownership */
	{
		if (is_server)
		{
			LSA_UINT16 ar_nr = CM_EDD_GET_USER_ID_AR(rb); /* see cm_sv_pdev_request_done() */

			if(
				pdprm->occupant != CM_PD_OCCUPANT_SV
				|| ar_nr == 0
			) {

				CM_FATAL();
			}
			else {

				if( ar_nr != pdprm->sv_ar_nr ) {

					/* late, do nothing */
					return CM_ERR_PRM_OTHER;
				}

				if( channel->usr.pd.reclist.is_valid ) {

					/* not between prm-begin and prm-end (unreachable, see checks in CM-SV) */
					return CM_ERR_PRM_OTHER;
				}
			}
		}
		else
		{
			if (pdprm->occupant != CM_PD_OCCUPANT_USER)
			{
				CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PD-write failed, occupant(%u) is invalid, record_index(0x%x), only NONPRM index allowed"
					, pdprm->occupant, prm_rdwr->record_index
					);
				return CM_ERR_SEQUENCE;
			}

			if (channel->usr.pd.reclist.is_valid)
			{
				CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PD-write failed, reclist_valid incorrect, need PD-begin first"
					);
				return CM_ERR_SEQUENCE;
			}
		}
	}

	return CM_OK;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT32
cm_pd_record_index_classify_adapt ( /* TIA 1167600 */
	LSA_UINT32  cls,
	CM_PD_PORT_PTR_TYPE  port
	)
{
	/* see cm_record_index_classify() */

	switch (cls & CM_REC_IDX_CMPD_MASK)
	{
#if CM_CFG_USE_MRP
	case CM_REC_IDX_CMPD_MRP:
		if (port->misc.MRPRingPort == EDD_MRP_NO_RING_PORT)
		{
			cls &= ~(CM_REC_IDX_WRITE | CM_REC_IDX_WRITE_LOCAL | CM_REC_IDX_READ | CM_REC_IDX_READ_LOCAL);
		}
		return cls;
#endif

	default:
		LSA_UNUSED_ARG (port);
		return cls;
	}
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT16
cm_pd_prm_write_internal (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE  prm_rdwr = rb->args.pd.prm_rdwr;
	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, prm_rdwr->slot_nr, prm_rdwr->subslot_nr);

	if (cm_is_null (prm_rdwr->record_data) && (prm_rdwr->record_index != 0xB08F/*StartupStatisticsData*/))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-write failed, record_data == NULL");
		return CM_ERR_PARAM;
	}

	if (cm_is_null (port))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-write failed, port not mapped, slot(%u) subslot(0x%x)"
			, prm_rdwr->slot_nr, prm_rdwr->subslot_nr
			);
		return CM_ERR_NOT_ACCESSIBLE/*!*/;
	}

	/* CMPD-WR & CMSV-WR */
	prm_rdwr->cls = cm_pd_record_index_classify_adapt (prm_rdwr->cls, port);

	if (prm_rdwr->cls & (CM_REC_IDX_INTERFACE | CM_REC_IDX_PORT))
	{
		if (port->port_id == 0/*interface*/  &&  (prm_rdwr->cls & CM_REC_IDX_INTERFACE) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, record_index(0x%x) not writeable to interface, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_PORTID;
		}

		if (port->port_id != 0/*any port*/  &&  (prm_rdwr->cls & CM_REC_IDX_PORT) == 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, record_index(0x%x) not writeable to port, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_PORTID;
		}
	}

	if (! ((prm_rdwr->cls & CM_REC_IDX_WRITE) == CM_REC_IDX_WRITE)
	&&  ! (prm_rdwr->is_local == CM_PD_IS_LOCAL_YES && (prm_rdwr->cls & CM_REC_IDX_WRITE_LOCAL) == CM_REC_IDX_WRITE_LOCAL))
	{
		if (port->port_id == 0/*interface*/  &&  (prm_rdwr->cls & CM_REC_IDX_INTERFACE) != 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, record_index(0x%x) is only readable from interface, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_ACCESS_DENIED;
		}
		else if (port->port_id != 0/*any port*/  &&  (prm_rdwr->cls & CM_REC_IDX_PORT) != 0)
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, record_index(0x%x) is only readable from port, cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_ACCESS_DENIED;
		}
		else
		{
			CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PD-write failed, wrong record_index(0x%x), cls(0x%x)"
				, prm_rdwr->record_index, prm_rdwr->cls
				);
			return CM_ERR_PRM_INDEX;
		}
	}

	if ((prm_rdwr->cls & CM_REC_IDX_CMPD_MASK) == CM_REC_IDX_CMPD_NONE)
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PD-write failed, wrong record_index(0x%x), unknown subcomponend_id, cls(0x%x)"
			, prm_rdwr->record_index, prm_rdwr->cls
			);
		return CM_ERR_PRM_INDEX;
	}

	if (prm_rdwr->cls & CM_REC_IDX_NONPRM) /* write non prm directly */
	{
		if (prm_rdwr->record_index == 0xB080/*PDInstanceTailorData*/)
		{
#if CM_CFG_MAX_CLIENTS
			cm_pd_prm_write_PDInstanceTailorData (channel, rb);
#else
			return CM_ERR_PRM_INDEX;
#endif
		}
		else if (prm_rdwr->record_index == 0xB08F/*StartupStatisticsData*/)
		{
#if CM_CFG_MAX_CLIENTS
			return cm_pd_prm_write_StartupStatisticsData (channel);
#else
			return CM_ERR_PRM_INDEX;
#endif
		}
		else
		{
			cm_pd_prm_write_nonprm (pdprm, rb);
		}
		return CM_OK_PENDING;
	}

	if (channel->usr.pd.reclist.is_valid)
	{
		return CM_ERR_SEQUENCE; /* unreachable, see cm_pd_prm_write_check_ownership */
	}

#if CM_CFG_MAX_CLIENTS
	if (prm_rdwr->record_index == 0x00017081/*PDMasterTailorData*/)
	{
		if (cm_is_not_null (channel->usr.pd.reclist.mtd)) /* mtd already created, keep only latest record, see cm_pd_reclist_write() */
		{
			cm_pd_mtd_free (& channel->usr.pd.reclist.mtd);
		}

		channel->usr.pd.reclist.mtd = cm_pd_mtd_parse (channel, 0/*PDev*/, prm_rdwr->record_data_length, prm_rdwr->record_data);

		if (cm_is_null (channel->usr.pd.reclist.mtd))
		{
			return CM_ERR_PRM_DATA;
		}
		/* no return, keep all records, because of remanence */
	}
#endif

		/* sniff some data */
	if (cm_cmpd_prm_write (pdprm, port, prm_rdwr) != CM_OK)
	{
		return CM_ERR_PRM_DATA;
	}

	if ((prm_rdwr->cls & CM_REC_IDX_MIXIN) == CM_REC_IDX_MIXIN)
	{
		if (prm_rdwr->record_index == 0x00010003 /* PDNRTFeedInLoadLimitation */)
		{
			prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_NO;
		}
		else
		{
			prm_rdwr->mk_remanent = CM_PD_MK_REMANENT_YES;
		}

		if (cm_pd_reclist_write (channel, CM_PD_RECLIST_MIXIN, rb) != CM_OK)
		{
			return CM_ERR_PRM_DATA;
		}
	}
	else
	{
		if (cm_pd_reclist_write (channel, CM_PD_RECLIST_REGULAR, rb) != CM_OK)
		{
			return CM_ERR_PRM_DATA;
		}
	}

	return CM_OK;
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_write_nonprm_done (
	CM_PD_PRM_CONST_PTR_TYPE  pdprm,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	cm_pd_req_callback (pdprm->channel, CM_RQB_GET_RESPONSE(rb), rb);
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_write_multiple (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr = & channel->usr.pd.req.multi_write;
	CM_UPPER_PD_PRM_RDWR_PTR_TYPE prm_rdwr = rb->args.pd.prm_rdwr;

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;
	prm_rdwr->ret_val_1 = 0;
	prm_rdwr->ret_val_2 = 0;

	if (cm_is_null (prm_rdwr->record_data))
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-write failed, record_data == NULL");
		cm_pd_req_callback (channel, CM_ERR_PARAM, rb);
		return;
	}

	if (prm_rdwr->mk_remanent != CM_PD_MK_REMANENT_NO && prm_rdwr->mk_remanent != CM_PD_MK_REMANENT_YES) {

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "prm_write_multiple: !mk_remanent(%u)", prm_rdwr->mk_remanent);
		cm_pd_req_callback(channel, CM_ERR_PARAM, rb);
		return;
	}

	/* startup multiple write */
	if (mwr->is_running) {
		CM_FATAL1 (mwr->data_offs);
	}

	prm_rdwr->cm_pnio_err = CM_PNIO_ERR_NONE;

	mwr->is_running			= LSA_TRUE;
	mwr->all_records_done	= LSA_FALSE;
	mwr->mk_remanent		= prm_rdwr->mk_remanent == CM_PD_MK_REMANENT_YES ? LSA_TRUE : LSA_FALSE;
	mwr->is_local			= prm_rdwr->is_local == CM_PD_IS_LOCAL_YES ? LSA_TRUE : LSA_FALSE;
	mwr->data				= prm_rdwr->record_data;
	mwr->data_offs			= 0;
	mwr->data_length		= prm_rdwr->record_data_length;
	mwr->rsp				= CM_OK;

	mwr->internal_rqb		= cm_upper_alloc_rqb (channel);

	if (cm_is_null (mwr->internal_rqb))
	{
		mwr->rsp = CM_ERR_ALLOC_UPPER;

		mwr->is_running = LSA_FALSE;
		mwr->all_records_done = LSA_TRUE;
		cm_pd_req_callback (channel, mwr->rsp, rb);
		return;
	}

	CM_RQB_SET_OPCODE (mwr->internal_rqb, CM_OPC_PD_PRM_WRITE);

	cm_pd_prm_write_multiple_next (channel);
}

static LSA_VOID
cm_pd_multi_write_parse (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr
	)
{
#define cm_pd_mwr_left(mwr_)  ((mwr_)->data_length - (mwr_)->data_offs)

	if (cm_pd_mwr_left(mwr) == 0)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "CMPD-MultiWrite: parsing data_length(%u) is finish", mwr->data_length
			);
		mwr->all_records_done = LSA_TRUE;
		return;
	}


	if (cm_pd_mwr_left(mwr) < 4+4+2+2+4+2+2)
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "CMPD-MultiWrite: left(%u) is less then RemaWriteHeader(%u)"
			, cm_pd_mwr_left(mwr), 4+4+2+2+4+2+2
			);
		mwr->rsp = CM_ERR_PRM_DATA;
		mwr->all_records_done = LSA_TRUE;
		return;
	}

	{
	CM_UPPER_RQB_PTR_TYPE  rb = mwr->internal_rqb;
	CM_UPPER_MEM_U8_PTR_TYPE  dp = mwr->data + mwr->data_offs;
	LSA_UINT32  index, api, length;
	LSA_UINT16  slot, subslot;
	LSA_UINT32  padding;

	/*
	//	RemaWriteMultiple ::=
	//		(RemaWriteHeader, RemaRecordData, [Padding*] a)*
	//
	//		a) the number of padding octets shall be adapted to make the block Unsigned32 aligned.
	//
	//	RemaWriteHeader ::=
	//		RemaRecordIndex(u32), API(u32), SlotNumber(u16), SubslotNumber(u16)
	//		, RemaRecordDataLength(u32), RemaCompID(u16), Reserved(u16)
	*/

	index		= CM_GET32_NTOH (dp, 0);
	api			= CM_GET32_NTOH (dp, 4);
	slot		= CM_GET16_NTOH (dp, 4+4);
	subslot		= CM_GET16_NTOH (dp, 4+4+2);
	length		= CM_GET32_NTOH (dp, 4+4+2+2);
/*
//	lsa_comp_id	= CM_GET16_NTOH (dp, 4+4+2+2+4);		// reserved with AP00916017
//	reserved	= CM_GET16_NTOH (dp, 4+4+2+2+4+2);		// reserved: set 0, do not check
*/
	mwr->data_offs += 4+4+2+2+4+2+2;

	dp = mwr->data + mwr->data_offs;

	if (length > cm_pd_mwr_left(mwr))
	{
		CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "CMPD-MultiWrite: not enough data for next record, current-offset(%u) wanted-length(%u) left(%u)"
			, mwr->data_offs, length, cm_pd_mwr_left(mwr)
			);
		mwr->rsp = CM_ERR_PRM_DATA;
		mwr->all_records_done = LSA_TRUE;
		return;
	}

	if (api != 0)	/* api for PDEV always 0 */
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "CMPD-MultiWrite: non-zero, current-offset(%u) api (0x%x)"
			, mwr->data_offs, api
			);
		mwr->rsp = CM_ERR_PRM_DATA;
		mwr->all_records_done = LSA_TRUE;
		return;
	}

	rb->args.pd.prm_rdwr->slot_nr			= slot;
	rb->args.pd.prm_rdwr->subslot_nr		= subslot;
	rb->args.pd.prm_rdwr->record_index		= index;
	rb->args.pd.prm_rdwr->record_data_length= length;
	rb->args.pd.prm_rdwr->record_data		= dp;

	rb->args.pd.prm_rdwr->mk_remanent		= mwr->mk_remanent ? CM_PD_MK_REMANENT_YES : CM_PD_MK_REMANENT_NO;
	rb->args.pd.prm_rdwr->is_local			= mwr->is_local ? CM_PD_IS_LOCAL_YES : CM_PD_IS_LOCAL_NO;

	rb->args.pd.prm_rdwr->cls				= cm_record_index_classify (channel, 0/*!*/, index, CM_VENDOR_ID_SIEMENS/*!*/);

	mwr->data_offs += length;

	padding = (4 - (mwr->data_offs % 4)) % 4;

	if (padding <= cm_pd_mwr_left(mwr))
	{
		mwr->data_offs += padding;
	}
	}
#undef cm_pd_mwr_left
}

static LSA_VOID
cm_pd_prm_write_multiple_next (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr = & channel->usr.pd.req.multi_write;

	cm_pd_multi_write_parse (channel, mwr);

	if (mwr->all_records_done)
	{
		cm_upper_free_rqb (channel, mwr->internal_rqb);

		if (mwr->rsp != CM_OK)
		{
			CM_PD_PRM_DIAG_TYPE prm_diag;

			CM_PD_PRM_DIAG_SET_WRITE_FAULT (&prm_diag, 0 /* Interface */, 0xF8000000, 0);
			cm_pd_prm_diag_add (&channel->usr.pd.pdprm, &prm_diag);
			cm_pd_reclist_set_error (channel);
		}

		mwr->is_running = LSA_FALSE;
		cm_pd_req_callback (channel, mwr->rsp, channel->usr.pd.req.current);
	}
	else
	{
		/* call single prm_write with internal rqb */
		cm_pd_prm_write_user (channel, mwr->internal_rqb, LSA_FALSE);
	}
}

LSA_VOID
cm_pd_prm_write_multiple_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_RESPONSE_TYPE  response
	)
{
	CM_PD_MULTI_WRITE_PTR_TYPE  mwr = & channel->usr.pd.req.multi_write;

	if (response != CM_OK  &&  mwr->rsp == CM_OK)
	{
		mwr->rsp = response;
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "WriteMultiple, got first bad response(%u)", response
			);
	}

	/* one of the single write is done, check if the multiple block is done, else cnf write_multiple */

	cm_pd_prm_write_multiple_next (channel);
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_prm_end_user (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & channel->usr.pd.pdprm;

	if (pdprm->occupant != CM_PD_OCCUPANT_USER)
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-end error, owner(%u) is invalid", pdprm->occupant);
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	if (channel->usr.pd.reclist.is_valid)
	{
		CM_PD_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "PD-end error, reclist_valid incorrect");
		cm_pd_req_callback (channel, CM_ERR_SEQUENCE, rb);
		return;
	}

	if (rb->args.pd.prm_end->set_mrp_off != CM_SET_MRP_OFF_NO && rb->args.pd.prm_end->set_mrp_off != CM_SET_MRP_OFF_YES) {

		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "prm_end_user: !set_mrp_off(%u)", rb->args.pd.prm_end->set_mrp_off);
		cm_pd_req_callback(channel, CM_ERR_PARAM, rb);
		return;
	}

#if CM_CFG_MAX_CLIENTS
	if (cm_is_not_null (channel->usr.pd.reclist.mtd))
	{
		CM_PD_PRM_DIAG_TYPE  prm_diag;

		LSA_BOOL rec_ok = cm_pd_reclist_validate_and_prealloc_tailoring_records (channel, channel->usr.pd.reclist.mtd, &prm_diag);

		if (! rec_ok)
		{
			CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "PD-end failed, record with index(0x%x) failed", prm_diag.err_index);

			cm_pd_prm_diag_add (&channel->usr.pd.pdprm, &prm_diag);
			cm_pd_reclist_set_error (channel);
		}
	}
#endif

	if (cm_pd_reclist_add_default_rec (channel, pdprm, LSA_FALSE, rb->args.pd.prm_end->set_mrp_off) != CM_OK) /* must not fail! */
	{
		CM_FATAL();
		return;
	}

	if (cm_pd_reclist_add_ep_rec (channel) != CM_OK) /* must not fail! */
	{
		CM_FATAL();
		return;
	}

	cm_pd_alarm_user_enable (pdprm->channel, LSA_FALSE);

	channel->usr.pd.reclist.is_valid = LSA_TRUE;

	rb->args.pd.prm_end->is_master_project = cm_pdusr_is_masterproject (channel) ? CM_IS_MASTER_PROJECT_YES : CM_IS_MASTER_PROJECT_NO;

	cm_pd_req_callback (channel, CM_OK, rb);

	cm_pd_config_start (channel, CM_PD_CONFIG_PRM_USR);
}


/*----------------------------------------------------------------------------*/
/* callback */
LSA_VOID
cm_pd_config_done (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  start_cfg,
	LSA_UINT16  rsp
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & pd_channel->usr.pd.pdprm;

	switch (start_cfg)
	{
	case CM_PD_CONFIG_PRM_USR:
		/* finish ownership of pdprm */
		if (pdprm->occupant == CM_PD_OCCUPANT_USER)
		{
			pdprm->occupant = CM_PD_OCCUPANT_NOBODY;
		}
		cm_pd_callback_appl_ready (pd_channel, rsp);
		break;

	case CM_PD_CONFIG_NOS_IP:
	case CM_PD_CONFIG_RESET_TO_FACTORY:
	case CM_PD_CONFIG_CL_DEVICE_ADD:
	case CM_PD_CONFIG_MACHINE_TAILOR:
		cm_pd_callback_appl_ready (pd_channel, rsp);
		break;

#if CM_CFG_MAX_SERVERS
	case CM_PD_CONFIG_PRM_SV:
		{
			CM_UPPER_RQB_PTR_TYPE  end = pdprm->end;
			CM_ASSERT(cm_is_not_null(end));
			pdprm->end = LSA_NULL;

			{
			CM_PD_AR_PRM_END_PTR_TYPE prm_end_args = (CM_PD_AR_PRM_END_PTR_TYPE)end->args.pd.prm_end->sv.void_ptr; /* exists, see cm_pd_prm_end_server() */
			LSA_UINT16 max_alarm_data_length = prm_end_args->max_alarm_data_length;

			if (max_alarm_data_length == 0)
			{
				CM_FATAL();
			}

			if (max_alarm_data_length > CM_ALARM_DATA_LENGTH_MAX) /* see cm_pd_almi_init() */
			{
				max_alarm_data_length = CM_ALARM_DATA_LENGTH_MAX;
			}

			pd_channel->usr.pd.almi.server_max_alarm_data_length = max_alarm_data_length;
			}

			pd_channel->usr.pd.almi.server_allowed = LSA_TRUE;

			if (cm_is_not_null (pdprm->port)) /* Plug-Port; Plug-IF; PrmUpdate-IF  */
			{
				cm_pd_diag_sv_copy_to_server_diag (pd_channel, pdprm->port);
				pdprm->port = LSA_NULL;

				cm_pd_req_callback (pd_channel, rsp, end);

				cm_pd_diag_sv_update_check (pd_channel);

				cm_pd_alarm_user_enable (pdprm->channel, LSA_TRUE);
			}
			else /* Connect; PrmUpdate-AR */
			{
				cm_pd_diag_sv_copy_to_server_diag (pd_channel, LSA_NULL);

				/* first: confirm SV */
				cm_pd_req_callback (pd_channel, rsp, end);

				cm_pd_diag_sv_update_check (pd_channel);

				/* second: inform user */
				cm_pd_callback_appl_ready (pd_channel, rsp);
			}
		}
		break;
#endif

	default:
		CM_FATAL();
		break;
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID
cm_pd_callback_appl_ready (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_RESPONSE_TYPE rsp
	)
{
	CM_PD_PRM_PTR_TYPE  pdprm = & pd_channel->usr.pd.pdprm;
	CM_UPPER_RQB_PTR_TYPE  appl_ready_ind = cm_pd_event_get (pd_channel);

	if (cm_is_null (appl_ready_ind))
	{
		pd_channel->usr.pd.almi.appl_ready_ind_pending = rsp; /* see cm_pd_opc_appl_ready_rsp */
		return;
	}

	CM_RQB_SET_OPCODE (appl_ready_ind, CM_OPC_PD_EVENT_APPL_READY_IND);
	cm_pd_prepare_appl_ready (pdprm, rsp, appl_ready_ind);

	pd_channel->usr.pd.almi.appl_ready_rsp_pending = LSA_TRUE;

	if (rsp == CM_OK  ||  rsp == CM_ERR_PRM_CONSISTENCY) /* since Diagnosis instead ARP: ERR_CONSISTENCY becomes OK */
	{
		cm_pd_req_callback (pdprm->channel, CM_OK, appl_ready_ind);
		cm_pd_alarm_user_enable (pdprm->channel, LSA_TRUE);
	}
	else if (rsp == CM_OK_ADAPTION_NEEDED)
	{
		cm_pd_req_callback (pdprm->channel, rsp, appl_ready_ind);
		cm_pd_alarm_user_enable (pdprm->channel, LSA_TRUE);
	}
	else /* CM_OK_RESET_TO_FACTORY */
	{
		cm_pd_req_callback (pdprm->channel, rsp, appl_ready_ind);
		cm_pd_alarm_user_enable (pdprm->channel, LSA_FALSE);
	}
}


/*------------------------------------------------------------------------------
// marshal EXTENDED CHANNEL DIAGNOSIS
//----------------------------------------------------------------------------*/

LSA_BOOL
cm_pd_diag_marshal_8000(
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr,
	CM_COMMON_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size,
	LSA_UINT32 *offs_ptr,
	CM_LIST_ENTRY_CONST_PTR_TYPE ext_phys_diag_list,
	LSA_UINT16 index
) {
	/*lint --e{713} Loss of precision request 1524437*/
	LSA_UINT32 offs;

	/*
	 * NOTE: copy/paste/adapt from cm_sv_record_marshal_diag()
	 *       what seems to be written in a strange way eases diff-ing!
	 */

	/***/

	offs = *offs_ptr;

	{ /* ap */
	LSA_UINT32 api = 0; /* see PNIO-Spec */
		{ /* slot */
			{ /* subslot */

				/*
				 * EXTENDED CHANNEL DIAGNOSIS
				 */

				{
				CM_ASSERT(cm_is_not_null(ext_phys_diag_list));

				if( CM_IS_PDEV_SUBSLOT_NR(/*iter->subslot->*/subslot_nr) ) {

					if( CmListIsEmpty(ext_phys_diag_list) ) {

						ext_phys_diag_list = LSA_NULL;
					}
				}
				else {
					CM_FATAL(); /* a bug */
				}

				/***/

				if(
					cm_is_not_null(ext_phys_diag_list)
				) {

					LSA_UINT32 offs_block;
					LSA_UINT32 offs_chpr_sum;

					/*
					 * ExtChannelDiagnosis ::=
					 *   SlotNumber, SubslotNumber,
					 *   ChannelNumber(0x8000), ChannelProperties a), UserStructureIdentifier(0x8002),
					 *   ExtChannelDiagnosisData *
					 *
					 * a) The field ChannelProperties.Type, the field ChannelProperties.Direction,
					 *    the field ChannelProperties.Maintenance shall be set to zero.
					 *    The field ChannelProperties.Specifier shall be set to appear if at least
					 *    one ChannelProperties.Specifier in the ExtChannelDiagnosisData is set to
					 *    appear in conjunction with ChannelProperties.Maintenance(=diagnosis).
					 *    Else, the field ChannelProperties.Specifier shall be set to disappear.
					 */

					if( offs + 2+2+1+1 + 4+2+2 + 2+2+2 > size ) {
						CM_PD_TRACE_00 (0, LSA_TRACE_LEVEL_NOTE_HIGH, "buffer too small");
						return LSA_FALSE;
					}

					offs_block = offs;

					CM_PUT16_HTON(data, offs, CM_BLOCK_TYPE_DIAGNOSIS);
					offs += 2;
					CM_PUT16_HTON(data, offs, 0); /* BlockLength is written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x0101/*V1.1*/);
					offs += 1+1;
					CM_PUT32_HTON(data, offs, /*iter->ap->*/api);
					offs += 4;

					CM_PUT16_HTON(data, offs, /*iter->slot->*/slot_nr);
					offs += 2;
					CM_PUT16_HTON(data, offs, /*iter->subslot->*/subslot_nr);
					offs += 2;

					CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
					offs += 2;
					offs_chpr_sum = offs; /* ChannelProperties are written below */
					offs += 2;
					CM_PUT16_HTON(data, offs, 0x8002/*ExtChannelDiagnosis*/);
					offs += 2;

					/***/

					{
					LSA_UINT32 diag_cnt;
					LSA_UINT16 chpr_sum;

					diag_cnt = 0;
					chpr_sum = CM_CHPR_SPEC_DIS;

					/*
					 * ExtChannelDiagnosisData ::=
					 *   ChannelNumber, ChannelProperties, ChannelErrorType,
					 *   ExtChannelErrorType, ExtChannelAddValue
					 */

					if( cm_is_not_null(ext_phys_diag_list) ) {

						CM_PD_DIAG_PTR_TYPE diag;

						for( CmListEach(diag, ext_phys_diag_list, CM_PD_DIAG_PTR_TYPE) ) {

							if( cm_record_marshal_diag_yes(index, LSA_FALSE, diag->channel_properties) ) {

								if( offs + 2+2+2 + 2+4 > size ) {
									CM_PD_TRACE_00 (0, LSA_TRACE_LEVEL_NOTE_HIGH, "buffer too small");
									return LSA_FALSE;
								}

								CM_PUT16_HTON(data, offs, 0x8000/*Submodule*/);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_properties);
								offs += 2;
								CM_PUT16_HTON(data, offs, diag->channel_error_type);
								offs += 2;

								CM_PUT16_HTON(data, offs, diag->ext_channel_error_type);
								offs += 2;
								CM_PUT32_HTON(data, offs, diag->ext_channel_add_value);
								offs += 4;

								/***/

								diag_cnt += 1;

								if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

									if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

										chpr_sum = CM_CHPR_SPEC_APP; /* see note a) above */
									}
								}
							}
						}
					}

					/* finally */

					CM_PUT16_HTON(data, offs_chpr_sum, chpr_sum);

					/***/

					if( diag_cnt != 0 ) {

						LSA_UINT32 blklen = offs - (offs_block + (2+2));

						if( blklen > 0xFFFF ) {
							CM_PD_TRACE_01 (0, LSA_TRACE_LEVEL_NOTE_HIGH, "cannot store blklen(%u)", blklen);
#ifdef CM_MESSAGE
/*#pragma CM_MESSAGE("VV: need to limit on add-diag!")*/
#endif /* CM_MESSAGE */
							return LSA_FALSE;
						}

						CM_PUT16_HTON(data, offs_block + 2, (LSA_UINT16)blklen);
					}
					else { /* rollback */

						offs = offs_block;
					}
					}
				}
				}
			}
		}
	}

	/***/

	if( offs > size ) {
		CM_FATAL(); /* a bug */
	}

	*offs_ptr = offs; /* update */

	return LSA_TRUE; /* ok */
}


/*===========================================================================*/
/*=== CONTROL PDEV SUBMODULE ==============================================*/
/*===========================================================================*/

/*------------------------------------------------------------------------------
// handle opcode CM_OPC_PD_SUBMODULE_CONTROL
//----------------------------------------------------------------------------*/

static LSA_UINT16
cm_pd_submodule_control_check (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_CONST_PTR_TYPE  rb
) {
	CM_UPPER_PD_SUBMODULE_CONTROL_PTR_TYPE  param = rb->args.pd.submodule_control;

	LSA_UINT16  slot_nr = param->slot_nr;
	LSA_UINT16  subslot_nr = param->subslot_nr;
	LSA_UINT32  mod_ident = param->mod_ident;
	LSA_UINT8   pdev_properties = param->pdev_properties;
	LSA_UINT8   im0_bits = param->im0_bits;

	CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr (channel, slot_nr, subslot_nr);
	LSA_UINT16  port_id;

	if (cm_is_null (port))
	{
		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "port for slot_nr(%u) subslot_nr(0x%x) not found"
			, slot_nr, subslot_nr
			);
		return CM_ERR_PARAM;
	}

	port_id = port->port_id;

	if ((pdev_properties & CM_PDEV_PROP_RESERVED_CONTROL_MASK) != 0)
	{
		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) pdev_properties(0x%x), using reserved pdev_properties bits"
			, port_id, slot_nr, subslot_nr, pdev_properties
			);
		return CM_ERR_PARAM;
	}

	if (pdev_properties == CM_PDEV_PROP_CHANGE_PROFIENERGY)
	{
		if (CM_PDEV_STATE_IS_PULLED ((port->addr.pdev_properties)))
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) is not plugged, changing profienergy not allowed"
				, port_id, slot_nr, subslot_nr
				);
			return CM_ERR_SEQUENCE;
		}

#if CM_CFG_MAX_SERVERS
		if ((im0_bits & ~CM_SV_IM0_BITS_PROFIENERGY) != 0)
#else
		if (im0_bits != 0)
#endif
		{
			CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "port_id(%u) slot_nr(%u) subslot_nr(0x%x), change profienergy using invalid im0_bits(0x%x)"
				, port_id, slot_nr, subslot_nr, im0_bits
				);
			return CM_ERR_PARAM;
		}
	}
	else if (pdev_properties == CM_PDEV_PROP_STATE_PULLED  ||  pdev_properties == CM_PDEV_PROP_STATE_PLUGGED)
	{
		if (CM_PDEV_TYPE_IS_COMPACT (port->addr.pdev_properties))
		{
			CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) is not modular, pull/plug not allowed"
				, port_id, slot_nr, subslot_nr
				);
			return CM_ERR_PARAM;
		}

		if (CM_PDEV_STATE_IS_PULLED (pdev_properties))
		{
			if (CM_PDEV_STATE_IS_PULLED (port->addr.pdev_properties))
			{
				CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) allready pulled"
					, port_id, slot_nr, subslot_nr
					);
				return CM_ERR_SEQUENCE;
			}
		}
		else
		{
			if (CM_PDEV_STATE_IS_PLUGGED (port->addr.pdev_properties))
			{
				CM_PD_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) allready plugged"
					, port_id, slot_nr, subslot_nr
					);
				return CM_ERR_SEQUENCE;
			}
			else
			{
				LSA_UINT16  port2_id;

				if (! cm_pd_im0_bits_ok (im0_bits, cm_pdusr_exists_im5_record (channel)))
				{
					CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) plug using invalid im0_bits(0x%x)"
						, port_id, slot_nr, subslot_nr, im0_bits
						);
					return CM_ERR_PARAM;
				}

				for (port2_id = 0; port2_id <= channel->usr.pd.port_count; ++port2_id)
				{
					CM_PD_PORT_PTR_TYPE  port2 = cm_pd_port_from_id__nn (channel, port2_id);

					if (port2->addr.slot_nr == slot_nr
						&&  port2->addr.subslot_nr != subslot_nr
						&&  CM_PDEV_STATE_IS_PLUGGED (port2->addr.pdev_properties)
						&&  port2->addr.mod_ident != mod_ident)
					{
						CM_PD_TRACE_08 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
							, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) and port_id(%u) slot_nr(%u) subslot_nr(0x%x) differ in mod_ident (0x%x) != (0x%x)"
							, port_id, slot_nr, subslot_nr, mod_ident
							, port2_id, port2->addr.slot_nr, port2->addr.subslot_nr, port2->addr.mod_ident
							);
						return CM_ERR_PARAM;
					}
				}
			}
		}
	}
	else
	{
		CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "port_id(%u) slot_nr(%u) subslot_nr(0x%x) pdev_properties(0x%x), invalid pdev_properties"
			, port_id, slot_nr, subslot_nr, pdev_properties
			);
		return CM_ERR_PARAM;
	}

	return CM_OK;
}

/*------------------------------------------------------------------------------*/
static CM_UPPER_RQB_PTR_TYPE
cm_pd_pullplug_alarm_build (
	CM_CHANNEL_PTR_TYPE channel
	)
{
	CM_PD_PORT_PTR_TYPE  port = channel->usr.pd.pullplug.port;
	CM_UPPER_RQB_PTR_TYPE  alarm  = channel->usr.pd.pullplug.alarm;
	CM_UPPER_ALARM_PTR_TYPE  param = alarm->args.pd.alarm;
	CM_UPPER_MEM_U8_PTR_TYPE  alarm_data;
	LSA_UINT16  alarm_data_len;
	LSA_UINT16 alarm_specifier;

	CM_ASSERT (cm_is_not_null (param));

	CM_ALLOC_UPPER_MEM (CM_CAST_UPPER_MEM_PTR_PTR (& param->alarm_data)
		, cm_null_user_id, channel->acp.max_alarm_data_length, channel->sysptr
	);

	if (cm_is_null (param->alarm_data))
	{
		CM_PD_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "no alarm_mem, size(%u)", channel->acp.max_alarm_data_length);

		CM_FATAL();
	}

	CM_RQB_SET_OPCODE (alarm, CM_OPC_PD_ALARM_IND);

	alarm_data = param->alarm_data;

	param->device_nr              = 0;  /* user: don't evaluate */
	param->ar_nr                  = 0;  /* user: don't evaluate */
	param->session_key            = 0;  /* user: don't evaluate */

	param->alarm_priority         = CM_ALARM_PRIORITY_LOW;

	param->api                    = 0;
	param->slot_nr                = port->addr.slot_nr;
	param->subslot_nr             = port->addr.subslot_nr;

	if (CM_PDEV_STATE_IS_PLUGGED (port->addr.pdev_properties))
	{
		param->alarm_type             = CM_ALARM_TYPE_PLUG;

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "slot_nr(%u) subslot_nr(0x%x) plug alarm"
			, param->slot_nr, param->subslot_nr
			);
	}
	else
	{
		param->alarm_type             = CM_ALARM_TYPE_PULL;

		CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "slot_nr(%u) subslot_nr(0x%x) pull alarm"
			, param->slot_nr, param->subslot_nr
			);
	}

	param->alarm_sequence         = 0;			/* don't care */
	param->diag_channel_available = CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO;		/* channel diagnosis available */
	param->diag_generic_available = CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO;		/* don't care */
	param->diag_submod_available  = CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO;	/* diagnosis available*/
	param->ar_diagnosis_state     = CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO;			/* don't care */
	param->reserved               = 0;			/* don't care */

	param->mod_ident              = port->addr.mod_ident;
	param->sub_ident              = port->addr.sub_ident;

	param->cm_pnio_err            = 0;			/* don't care */
	param->maintenance_status     = 0;

	param->alarm_tag = 0;

	param->alarm_sequence = channel->usr.pd.almi.user_sequence_nr;
	CM_ALARM_SEQUENCE_NR_INCREMENT (channel->usr.pd.almi.user_sequence_nr);

	ACP_SET_ALARM_SPECIFIER(& alarm_specifier
		, param->alarm_sequence
		, (param->diag_channel_available == CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES)
		, (param->diag_generic_available == CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES)
		, (param->diag_submod_available == CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES)
		, (param->ar_diagnosis_state == CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES)
	);

	alarm_data_len = (2+2+2 + 2 + 4+2+2 + 4+4 + 2);
	param->alarm_data_length      = alarm_data_len;

	if (alarm_data_len > channel->acp.max_alarm_data_length) {
		CM_FATAL1 (alarm_data_len); /* caller must check */
	}

	CM_PUT16_HTON (alarm_data, 0, 0x0002); /* ACP_ALARM_BLOCK_REQUEST_LOW */
	CM_PUT16_HTON (alarm_data, 2, alarm_data_len - (2+2)); /* block without (block_type,block_length) */
	CM_PUT16_HTON (alarm_data, 4, 0x0100); /* ACP_ALARM_BLOCK_VERSION */

	CM_PUT16_HTON (alarm_data, 6, param->alarm_type);

	CM_PUT32_HTON (alarm_data, 8, param->api);
	CM_PUT16_HTON (alarm_data, 12, param->slot_nr);
	CM_PUT16_HTON (alarm_data, 14, param->subslot_nr);

	CM_PUT32_HTON (alarm_data, 16, param->mod_ident);
	CM_PUT32_HTON (alarm_data, 20, param->sub_ident);

	CM_PUT16_HTON (alarm_data, 24, alarm_specifier);

	channel->usr.pd.pullplug.alarm = LSA_NULL;

	return alarm;
}

/*------------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_pullplug_sc_pull (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_PD_PULLPLUG_PTR_TYPE  pullplug = & channel->usr.pd.pullplug;

	pullplug->wait_sc = CM_PD_SCBF_EDD;
	cm_edd_pullplug_change_port (channel, EDD_PRM_PORTMODULE_PULLED, 0);

#if CM_CFG_USE_MRP
	pullplug->wait_sc |= CM_PD_SCBF_MRP;
	cm_mrp_pullplug_change_port (channel, MRP_PRM_PORTMODULE_PULLED, 0);
#endif

#if CM_CFG_USE_GSY
	pullplug->wait_sc |= CM_PD_SCBF_GSY;
	cm_gsy_pullplug_change_port (channel, GSY_PRM_PORTMODULE_PULLED, 0);
#endif

#if CM_CFG_USE_POF
	pullplug->wait_sc |= CM_PD_SCBF_POF;
	cm_pof_pullplug_change_port (channel, POF_PRM_PORTMODULE_PULLED, 0);
#endif

	pullplug->wait_sc |= CM_PD_SCBF_OHA;
	cm_oha_pullplug_change_port (channel, OHA_PRM_PORTMODULE_PULLED, 0);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_pullplug_sc_plug_prepare (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_PD_PULLPLUG_PTR_TYPE  pullplug = & channel->usr.pd.pullplug;

	pullplug->port->addr.pdev_properties &= ~CM_PDEV_PROP_DEFAULT_ENABLED;

	pullplug->wait_sc = CM_PD_SCBF_EDD;
	cm_edd_pullplug_change_port (channel, EDD_PRM_PORTMODULE_PLUG_PREPARE, 0);

#if CM_CFG_USE_MRP
	pullplug->wait_sc |= CM_PD_SCBF_MRP;
	cm_mrp_pullplug_change_port (channel, MRP_PRM_PORTMODULE_PLUG_PREPARE, 0);
#endif

#if CM_CFG_USE_GSY
	pullplug->wait_sc |= CM_PD_SCBF_GSY;
	cm_gsy_pullplug_change_port (channel, GSY_PRM_PORTMODULE_PLUG_PREPARE, 0);
#endif

#if CM_CFG_USE_POF
	pullplug->wait_sc |= CM_PD_SCBF_POF;
	cm_pof_pullplug_change_port (channel, POF_PRM_PORTMODULE_PLUG_PREPARE, 0);
#endif

	pullplug->wait_sc |= CM_PD_SCBF_OHA;
	cm_oha_pullplug_change_port (channel, OHA_PRM_PORTMODULE_PLUG_PREPARE, 0);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_pd_pullplug_sc_plug_commit (
	CM_CHANNEL_PTR_TYPE  channel
	)
{
	CM_PD_PULLPLUG_PTR_TYPE  pullplug = & channel->usr.pd.pullplug;
	LSA_UINT8  apply_default_portparams = 0;

	if (pullplug->port->addr.pdev_properties & CM_PDEV_PROP_DEFAULT_ENABLED)
	{
		apply_default_portparams = 1;
	}

	pullplug->wait_sc = CM_PD_SCBF_EDD;
	cm_edd_pullplug_change_port (channel, EDD_PRM_PORTMODULE_PLUG_COMMIT, apply_default_portparams);

#if CM_CFG_USE_MRP
	pullplug->wait_sc |= CM_PD_SCBF_MRP;
	cm_mrp_pullplug_change_port (channel, MRP_PRM_PORTMODULE_PLUG_COMMIT, apply_default_portparams);
#endif

#if CM_CFG_USE_GSY
	pullplug->wait_sc |= CM_PD_SCBF_GSY;
	cm_gsy_pullplug_change_port (channel, GSY_PRM_PORTMODULE_PLUG_COMMIT, apply_default_portparams);
#endif

#if CM_CFG_USE_POF
	pullplug->wait_sc |= CM_PD_SCBF_POF;
	cm_pof_pullplug_change_port (channel, POF_PRM_PORTMODULE_PLUG_COMMIT, apply_default_portparams);
#endif

	pullplug->wait_sc |= CM_PD_SCBF_OHA;
	cm_oha_pullplug_change_port (channel, OHA_PRM_PORTMODULE_PLUG_COMMIT, apply_default_portparams);
}

/*------------------------------------------------------------------------------*/
LSA_VOID
cm_pd_pullplug_sm_start (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_PD_PORT_PTR_TYPE  port,
	LSA_UINT8  action
)
{
	CM_PD_PULLPLUG_PTR_TYPE  pullplug = &channel->usr.pd.pullplug;

	CM_PD_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PULLPLUG_SM_TRIGGER: action(0x%x) port_id(0x%x)"
		, action, port->port_id
		);

	pullplug->wait_sc = 0;
	pullplug->wait_done = 0;
	pullplug->port = port;

	switch (action)
	{
	case CM_PD_PULLPLUG__PLUG:
		pullplug->wait_for = CM_PD_PULLPLUG__DO_PLUG_PREPARE | CM_PD_PULLPLUG__DO_PLUG_COMMIT;
		break;

	case CM_PD_PULLPLUG__PULL:
		pullplug->wait_for = CM_PD_PULLPLUG__DO_PULL;
		break;

	case CM_PD_PULLPLUG__MAP:
		pullplug->wait_for = CM_PD_PULLPLUG__DO_PULL | CM_PD_PULLPLUG__MAP;
		break;

	case CM_PD_PULLPLUG__REMAP:
		pullplug->wait_for = CM_PD_PULLPLUG__DO_PULL | CM_PD_PULLPLUG__REMAP;
		break;

	default:
		CM_FATAL(); /* a bug*/
		break;
	}


	cm_pd_pullplug_sm_trigger (channel, 0, 0);
}

/*------------------------------------------------------------------------------*/
LSA_VOID
cm_pd_pullplug_sm_trigger (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  wait_sc_done,
	LSA_UINT8  portparams_not_applicable
	)
{
	CM_PD_PULLPLUG_PTR_TYPE  pullplug = &channel->usr.pd.pullplug;

	CM_PD_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "PULLPLUG_SM_TRIGGER: wait_for(0x%x) wait_done(0x%x) wait_sc(0x%x) wait_sc_done(0x%x)"
		, pullplug->wait_for, pullplug->wait_done, pullplug->wait_sc, wait_sc_done
		);

	CM_ASSERT ((pullplug->wait_sc & wait_sc_done) == wait_sc_done);
	pullplug->wait_sc &= ~wait_sc_done;

	if (pullplug->wait_done == CM_PD_PULLPLUG__DO_PLUG_PREPARE)
	{
		if (portparams_not_applicable == 1)
		{
			pullplug->port->addr.pdev_properties |= CM_PDEV_PROP_DEFAULT_ENABLED;
		}
		else
		{
			CM_ASSERT (portparams_not_applicable == 0);
		}
	}

	if (pullplug->wait_sc != 0)
	{
		/* wait.. */
		return;
	}

	if (pullplug->wait_done == CM_PD_PULLPLUG__DO_PULL)
	{ /* TIA 2569164 */
		cm_pd_diag_list_clear (& pullplug->port->diag.diag_list, CM_PD_DIAG_TAG_PDUSER);

		cm_pd_diag_list_clear (& pullplug->port->diag.diag_list, CM_PD_DIAG_TAG_SUBCOMP);

		cm_pd_diag_list_clear (& pullplug->port->diag.diag_list_user, 0/*any*/);
	}

	pullplug->wait_for &= ~pullplug->wait_done;

	if (pullplug->wait_for & CM_PD_PULLPLUG__DO_PLUG_PREPARE)
	{
		pullplug->wait_done = CM_PD_PULLPLUG__DO_PLUG_PREPARE;
		cm_pd_pullplug_sc_plug_prepare (channel);
		return;
	}

	if (pullplug->wait_for & CM_PD_PULLPLUG__DO_PLUG_COMMIT)
	{
		CM_UPPER_PD_SUBMODULE_CONTROL_PTR_TYPE  args = channel->usr.pd.req.current->args.pd.submodule_control;

		pullplug->port->addr.pdev_properties &= ~CM_PDEV_PROP_STATE_MASK;
		pullplug->port->addr.pdev_properties |= CM_PDEV_PROP_STATE_PLUGGED;

		pullplug->port->addr.im0_bits  = args->im0_bits;
		pullplug->port->addr.mod_ident = args->mod_ident;
		pullplug->port->addr.sub_ident = args->sub_ident;

		pullplug->wait_done = CM_PD_PULLPLUG__DO_PLUG_COMMIT;
		cm_pd_pullplug_sc_plug_commit (channel);
		return;
	}

	if (pullplug->wait_for & CM_PD_PULLPLUG__DO_PULL)
	{
		pullplug->wait_done = CM_PD_PULLPLUG__DO_PULL;
		cm_pd_pullplug_sc_pull (channel);
		return;
	}

	if (pullplug->wait_for & CM_PD_PULLPLUG__MAP || pullplug->wait_for & CM_PD_PULLPLUG__REMAP)
	{
		pullplug->wait_done = pullplug->wait_for;
		pullplug->wait_for &= ~pullplug->wait_done;
		cm_pd_pullplug_map_remap (channel, pullplug->wait_done, pullplug->port->port_id);
		return;
	}

	CM_ASSERT (pullplug->wait_for == 0);
	pullplug->wait_done = 0;

	if (pullplug->alarm) /* see cm_pd_submodule_control() */
	{
		CM_UPPER_RQB_PTR_TYPE  alarm = cm_pd_pullplug_alarm_build (channel);
		channel->usr.pd.req.alarm_ind_count += 1;

		cm_pd_req_callback(channel, CM_OK, channel->usr.pd.req.current);

		cm_callback (channel, CM_OK, &alarm);
	}
	else
	{
		CM_ASSERT (CM_RQB_GET_OPCODE (channel->usr.pd.req.current) != CM_OPC_PD_SUBMODULE_CONTROL);

		cm_pd_req_callback(channel, CM_OK, channel->usr.pd.req.current);
	}
}


/*------------------------------------------------------------------------------*/
LSA_VOID
cm_pd_submodule_control (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	LSA_UINT16 rsp = cm_pd_submodule_control_check(channel, rb);

	if (rsp != CM_OK)
	{
		cm_pd_req_callback(channel, rsp, rb);
	}
	else
	{
		CM_UPPER_PD_SUBMODULE_CONTROL_PTR_TYPE  param = rb->args.pd.submodule_control;
		CM_PD_PORT_PTR_TYPE  port = cm_pd_port_from_addr__nn(channel, param->slot_nr, param->subslot_nr);

#if CM_CFG_MAX_SERVERS
		CM_CHANNEL_PTR_TYPE sv_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_SERVER);
#endif

		if (param->pdev_properties == CM_PDEV_PROP_CHANGE_PROFIENERGY)
		{
#if CM_CFG_MAX_SERVERS
			port->addr.im0_bits &= ~CM_SV_IM0_BITS_PROFIENERGY;
			port->addr.im0_bits |= param->im0_bits;

			if (cm_is_not_null (sv_channel))
			{
				cm_sv_pdev_set_im0_bits (sv_channel, param->slot_nr, param->subslot_nr, port->addr.im0_bits);
			}
#endif

			cm_pd_req_callback(channel, CM_OK, rb);
		}
		else
		{
			if (channel->usr.pd.pullplug.wait_for != 0)
			{
				CM_FATAL(); /* a bug, sm busy */
			}

			channel->usr.pd.pullplug.alarm = cm_pd_alarm_get (channel);

			if (cm_is_null (channel->usr.pd.pullplug.alarm))
			{
				channel->usr.pd.req.wait_for |= CM_PD_WF_PULLPLUG_ALARM_RES;
			}
			else
			{
				channel->usr.pd.pullplug.port = port;

				if (CM_PDEV_STATE_IS_PLUGGED (param->pdev_properties))
				{
					CM_ASSERT (CmListIsEmpty (& port->diag.diag_list_user));

#if CM_CFG_MAX_SERVERS
					if (cm_is_not_null (sv_channel))
					{
						cm_sv_pdev_plug_submodule (sv_channel, param->slot_nr, param->subslot_nr, param->mod_ident, param->sub_ident, param->im0_bits);
					}
#endif
					cm_pd_pullplug_sm_start (channel, port, CM_PD_PULLPLUG__PLUG);
				}
				else
				{
					port->addr.pdev_properties &= ~CM_PDEV_PROP_STATE_MASK;
					port->addr.pdev_properties |= CM_PDEV_PROP_STATE_PULLED;

					port->addr.im0_bits  = 0;
					port->addr.mod_ident = 0;
					port->addr.sub_ident = 0;

#if CM_CFG_MAX_SERVERS
					if (cm_is_not_null (sv_channel))
					{
						cm_sv_pdev_pull_submodule(sv_channel, param->slot_nr, param->subslot_nr);
					}
#endif

					cm_pd_pullplug_sm_start (channel, port, CM_PD_PULLPLUG__PULL);
				}
			}
		}
	}
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
