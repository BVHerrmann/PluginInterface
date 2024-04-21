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
/*  F i l e               &F: cm_sv24.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Device things                                                 */
/*                                                                           */
/*  - ar utilities                                                           */
/*  - ar alloc, free                                                         */
/*  - ar get, get-by-xxx                                                     */
/*                                                                           */
/*  - ar abort all                                                           */
/*  - ar link-status changed notification                                    */
/*  - ar send-clock changed notification                                     */
/*  - ar dcp changed notification                                            */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7240
#define CM_MODULE_ID       7240

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_BOOL
cm_sv_all_ARs_gone( /* returns true if all ARs of all devices are gone */
	CM_CHANNEL_CONST_PTR_TYPE channel
);

/*===========================================================================*/
/*=== IMPLICIT-AR INIT / UNDO-INIT ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_implicit_ar_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	CmListInitialize(&dev->implicit_ar.queue);

	dev->implicit_ar.rpc = LSA_NULL;

	dev->implicit_ar.iter.pdev.state = CM_PD_ITER_PDEV_IDLE; /* sic! using the CM-PD enumeration */
}

/*===========================================================================*/

LSA_VOID
cm_sv_implicit_ar_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	CM_ASSERT(CmListIsEmpty(&dev->implicit_ar.queue));

	CM_ASSERT(cm_is_null(dev->implicit_ar.rpc));

	CM_ASSERT(dev->implicit_ar.iter.pdev.state == CM_PD_ITER_PDEV_IDLE); /* sic! using the CM-PD enumeration */

	LSA_UNUSED_ARG(dev);
}

/*===========================================================================*/
/*=== EXPLICIT-AR INIT / UNDO-INIT ==========================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_explicit_ar_init(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 max_ars_IOC,
	LSA_UINT16 max_ars_IOS,
	LSA_UINT16 max_ars_DAC
) {

	dev->explicit_ar.max_ARs_IOC = max_ars_IOC;
	dev->explicit_ar.max_ARs_IOS = max_ars_IOS;
	dev->explicit_ar.max_ARs_DAC = max_ars_DAC;
	dev->explicit_ar.max_ARs_IOX = max_ars_IOC + max_ars_IOS;
	dev->explicit_ar.max_ARs_TOT = max_ars_IOC + max_ars_IOS + max_ars_DAC;

	CM_ASSERT(dev->explicit_ar.max_ARs_TOT >= 1 && dev->explicit_ar.max_ARs_TOT <= CM_CFG_MAX_SV_ARS);

	/***/

	dev->explicit_ar.ar_cnt = 0;

	{
	LSA_UINT16 ar_nr;

	for( ar_nr = 0/*!*/; ar_nr <= CM_CFG_MAX_SV_ARS; ++ar_nr ) {

		dev->explicit_ar.argr_arr[ar_nr] = LSA_NULL;
	}
	}

	/***/

	dev->explicit_ar.sort.cnt = 0;

	/*
	dev->explicit_ar.sort.arr = need not be initialized
	*/

	/***/

	{
	LSA_UINT16 arset_nr;

	for( arset_nr = 1; arset_nr <= CM_CFG_MAX_SV_ARSETS_PER_DEVICE; ++arset_nr ) {

		CM_SV_ARSET_PTR_TYPE arset = &dev->explicit_ar.arset[arset_nr - 1];

		arset->arset_nr = arset_nr;

		arset->is_aborting = LSA_FALSE;

		{
		LSA_UINT16 idx;

		for( idx = 0; idx < CM_SV_ARSET_MAX_ARS; ++idx ) {

			arset->arr[idx] = LSA_NULL;
		}
		}

		arset->abort_rqb = LSA_NULL;

		/***/

		arset->first = LSA_NULL;
		arset->primary = LSA_NULL;

		{
		LSA_UINT16 prio;

		for( prio = 0; prio < 2; ++prio ) {

			arset->alarm[prio].sequence_number = 0;

			CmListInitialize(&arset->alarm[prio].queue);
		}
		}

		/***/

		{ //R1 VV: make a function. maybe make a function for "arset init" too.
		CM_SV_ARSET_R1B_PTR_TYPE r1b = &arset->r1b;
		LSA_UINT16 prio;

		r1b->wait_for_primary_trigger = LSA_FALSE;

		for( prio = 0; prio < 2; ++prio ) {

			CM_SV_R1_ALARM_CURR_PTR_TYPE alarm_curr = &r1b->alarm.curr[prio];

			alarm_curr->is_used = LSA_FALSE;
		}
		}
	}
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_explicit_ar_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	CM_ASSERT(dev->explicit_ar.ar_cnt == 0);

	CM_ASSERT(dev->explicit_ar.sort.cnt == 0);

	LSA_UNUSED_ARG(dev);
}

/*===========================================================================*/
/*=== GET RPC-CLIENT ID =====================================================*/
/*===========================================================================*/

LSA_VOID_PTR_TYPE
cm_sv_get_rpc_client_id(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_VOID_PTR_TYPE clid = argr->cm.sv.dev->res.clid_arr[argr->ar_nr]; /* index = ar_nr, see CM_SV_DEVICE_TYPE */

	if( clid == CM_RPC_INVALID_CLIENT_ID ) {
		CM_FATAL(); /* see pre-allocation */
	}

	return clid;
}

/*===========================================================================*/
/*=== RESOURCE INIT =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_res_init(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 max_ars_IOC,
	LSA_UINT16 max_ars_IOS,
	LSA_UINT16 max_ars_DAC,
	LSA_UINT16 write_multiple_parallelism /* >= 1, see cm_sv_dev_params_ok() */
) {
	LSA_UINT16 max_ars_IOX = max_ars_IOC + max_ars_IOS;

	dev->res.nr_of_event_resources = 1; /* per implicit AR: one record-read.ind */
	dev->res.nr_of_alarm_resources = 0; /* per implicit AR: none */

	if( max_ars_IOX != 0 ) { /* per explicit RT-AR */

		LSA_UINT16 ev = 0;

		ev += 1; /* connect.ind ex-or disconnect.ind */

		ev += 1; /* prm-end.ind ex-or record.ind (disconnect.ind not serialized with record.ind) */

		ev += (write_multiple_parallelism - 1); /* more record.ind */

		ev += 1; /* ownership */

		ev += 1; /* rir.ind ex-or in-data.ind */

		if( dev->channel->usr.sv.enable_DataStatusIndication ) {

			ev += 1; /* data-status */
		}

		if( dev->PDevPrmInd_enabled ) {

			/* none, pdevprm.ind is serialized with prm-end.ind */
		}

		dev->res.nr_of_event_resources += CM_CAST_U16((ev * max_ars_IOX));

		if( dev->channel->usr.sv.enable_AlarmResponder ) {

			dev->res.nr_of_alarm_resources += CM_CAST_U16((2 * max_ars_IOX)); /* low and high priority */
		}
	}

	if( max_ars_DAC != 0 ) { /* per explicit DA-AR */

		LSA_UINT16 ev = 0;

		ev += 1; /* connect.ind ex-or disconnect.ind */
		ev += 1; /* record.ind (disconnect.ind not serialized with record.ind) */

		dev->res.nr_of_event_resources += CM_CAST_U16((ev * max_ars_DAC));

		dev->res.nr_of_alarm_resources += 0; /* none */
	}

	/***/

	dev->res.rpc_rqb = LSA_NULL; /* pre-alloc see caller */

	CmListInitialize(&dev->res.rpc_callQ); /* pre-alloc see caller */

	dev->res.rpc_server_id = CLRPC_INVALID_SERVER_ID;

	CmListInitialize(&dev->res.eventQ);

	CmListInitialize(&dev->res.alarmQ);

	{
	LSA_UINT16 ar_nr;

	for( ar_nr = 0/*!*/; ar_nr <= CM_CFG_MAX_SV_ARS; ++ar_nr ) {

		dev->res.clid_arr[ar_nr] = CM_RPC_INVALID_CLIENT_ID;

#if CM_CFG_MAX_SV_ARS > 0xFF
#error "CM_CFG_MAX_SV_ARS must not be greater than 0xFF"
#endif

		dev->res.skey_arr[ar_nr] = (0x0000 | ar_nr); /* high-bits = session, low-byte = ar_nr */
	}
	}
}

/*===========================================================================*/
/*=== RESOURCE CANCEL =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_res_cancel(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	{
	LSA_UINT16 ar_nr;

	for( ar_nr = 0/*!*/; ar_nr <= CM_CFG_MAX_SV_ARS; ++ar_nr ) {

		CM_ASSERT(dev->res.clid_arr[ar_nr] == CM_RPC_INVALID_CLIENT_ID);
	}
	}

	while( ! CmListIsEmpty(&dev->res.eventQ) ) {

		CM_UPPER_RQB_PTR_TYPE rb = cm_sv_dev_get_event_notnull(dev, LSA_NULL, CM_OPC_SV_DEVICE_PROVIDE_EVENT);

		if( rb->args.sv.ar_event != rb->args.sv.dev_provide_event ) {

			CM_FATAL(); /* actually unreachable, see RQB */
		}
		else {

			cm_callback(dev->channel, CM_OK_CANCELLED, &rb);
		}
	}

	while( ! CmListIsEmpty(&dev->res.alarmQ) ) {

		CM_UPPER_RQB_PTR_TYPE rb = cm_sv_dev_get_alarm_notnull(dev, LSA_NULL, 0/*don't care*/, CM_OPC_SV_DEVICE_PROVIDE_ALARM);

		if( rb->args.sv.ar_alarm_ind != rb->args.sv.dev_provide_alarm ) {

			CM_FATAL(); /* actually unreachable, see RQB */
		}
		else {

			cm_callback(dev->channel, CM_OK_CANCELLED, &rb);
		}
	}

	cm_sv_r1b_send_res_cancel(dev);
}

/*===========================================================================*/
/*=== PROVIDE / REPROVIDE / GET EVENT =======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_provide_event(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.dev_provide_event;
	CM_SV_DEVICE_PTR_TYPE dev;

	dev = cm_sv_dev_get_added(channel, param->device_nr);

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) is not added"
			, param->device_nr
			);

		cm_callback(channel, CM_ERR_SEQUENCE, &rb);
	}
	else if( param != rb->args.sv.ar_event ) {

		CM_FATAL(); /* actually unreachable, see RQB */
	}
	else {

		/* dev->curr == don't care */

		cm_sv_dev_reprovide_event(dev, rb);
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_dev_reprovide_event(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	if( dev->device_nr != rb->args.sv.ar_event->device_nr ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_RQB_SET_OPCODE(rb, CM_OPC_MAX); /* invalidate */

		CmListInsertTail(&dev->res.eventQ, (CM_LIST_ENTRY_PTR_TYPE)rb);
	}
}

/*===========================================================================*/

CM_UPPER_RQB_PTR_TYPE
cm_sv_dev_get_event_notnull(
	CM_SV_DEVICE_CONST_PTR_TYPE dev, /* don't care if argr != LSA_NULL */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_OPCODE_TYPE cm_opcode
) {
	if( cm_is_not_null(argr) ) {

		dev = argr->cm.sv.dev;
	}

	if( cm_is_null(dev) ) {

		CM_FATAL(); /* a bug */

		return LSA_NULL;
	}

	/***/

	if( CmListIsEmpty(&dev->res.eventQ) ) {

		CM_FATAL(); /* a bug */

		return LSA_NULL;
	}
	else {

		CM_UPPER_RQB_PTR_TYPE rb;

		CmListRemoveHead(&dev->res.eventQ, rb, CM_UPPER_RQB_PTR_TYPE);

		CM_RQB_SET_OPCODE(rb, cm_opcode);

		{
		CM_UPPER_EVENT_PTR_TYPE param = rb->args.sv.ar_event;

		/*
		param->device_nr == dev->device_nr, see cm_sv_dev_reprovide_event()
		*/

		if( cm_is_not_null(argr) ) {

			param->session_key = argr->session_key;
		}
		else {

			param->session_key = 0;
		}

		param->ar_nr = CM_SV_SESSION_KEY_TO_AR_NR(param->session_key); /* as documented */
		}

		return rb;
	}
}

/*===========================================================================*/
/*=== PROVIDE / REPROVIDE / GET ALARM =======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_provide_alarm(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.dev_provide_alarm;
	CM_SV_DEVICE_PTR_TYPE dev;

	dev = cm_sv_dev_get_added(channel, param->device_nr);

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) is not added"
			, param->device_nr
			);

		cm_callback(channel, CM_ERR_SEQUENCE, &rb);
	}
	else if( param != rb->args.sv.ar_alarm_ind ) {

		CM_FATAL(); /* actually unreachable, see RQB */
	}
	else {

		/* dev->curr == don't care */

		param->alarm_data_length = 0; /* sanity */
		param->alarm_data = LSA_NULL; /* sanity */

		cm_sv_dev_reprovide_alarm(dev, rb);
	}
}

/*===========================================================================*/

LSA_VOID
cm_sv_dev_reprovide_alarm(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb
) {

	if( dev->device_nr != rb->args.sv.ar_alarm_ind->device_nr ) {

		CM_FATAL(); /* a bug */
	}
	else {

		CM_RQB_SET_OPCODE(rb, CM_OPC_MAX); /* invalidate */

		/* alarm_data_length = 0 and alarm_data = LSA_NULL, see caller */

		CmListInsertTail(&dev->res.alarmQ, (CM_LIST_ENTRY_PTR_TYPE)rb);
	}
}

/*===========================================================================*/

CM_UPPER_RQB_PTR_TYPE
cm_sv_dev_get_alarm_notnull(
	CM_SV_DEVICE_CONST_PTR_TYPE dev, /* don't care if argr != LSA_NULL */
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT16 prio, /* AP01025495 */
	LSA_OPCODE_TYPE cm_opcode
) {

	if( cm_is_not_null(argr) ) {

		dev = argr->cm.sv.dev;
	}

	if( cm_is_null(dev) ) {

		CM_FATAL(); /* a bug */

		return LSA_NULL;
	}

	/***/

	if( CmListIsEmpty(&dev->res.alarmQ) ) {

		CM_FATAL(); /* a bug */

		return LSA_NULL;
	}
	else {

		CM_UPPER_RQB_PTR_TYPE rb;

		CmListRemoveHead(&dev->res.alarmQ, rb, CM_UPPER_RQB_PTR_TYPE);

		CM_RQB_SET_OPCODE(rb, cm_opcode);

		{
		CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.ar_alarm_ind;

		/*
		param->device_nr == dev->device_nr, see cm_sv_dev_reprovide_event()
		*/

		if( cm_is_not_null(argr) ) {

			param->session_key = argr->session_key;
		}
		else {

			param->session_key = 0;
		}

		param->ar_nr = CM_SV_SESSION_KEY_TO_AR_NR(param->session_key); /* as documented */

		param->alarm_priority = prio;
		}

		return rb;
	}
}

/*===========================================================================*/
/*=== CALCULATE PROVIDER SEQUENCE ===========================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_dev_uuid_less_than( /* returns true if a < b */
	CLRPC_UUID_CONST_PTR_TYPE a_host_byte_order,
	CLRPC_UUID_CONST_PTR_TYPE b_host_byte_order
) {
	LSA_UINT8 a_uuid[CM_PDU_UUID_SIZE];
	LSA_UINT8 b_uuid[CM_PDU_UUID_SIZE];

	/*
	 * the ar_uuids are in host byte order
	 * for the mem-compare we must convert them into network byte order
	 * otherwise the result would depend on the endianess
	 */

	CM_PUT_UUID_HTON(a_uuid, 0, a_host_byte_order);
	CM_PUT_UUID_HTON(b_uuid, 0, b_host_byte_order);

	if( CM_MEMCMP(a_uuid, b_uuid, sizeof(a_uuid)) < 0 ) {

		return LSA_TRUE;
	}

	return LSA_FALSE;
}

/*===========================================================================*/

static LSA_VOID
cm_sv_dev_calc_provider_sequence( /* AP00619993 (stable sequence of provider frames) */
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_AR_GRAPH_PTR_TYPE curr_argr
) {
	if( cm_is_iosar_with_device_access(curr_argr) ) { /* easy-supervisor */

		curr_argr->cm.sv.provider_sequence = 0; /* no iocrs */
	}
	else {

		LSA_UINT32 seq_left;
		LSA_UINT32 seq_right;
		LSA_UINT32 seq_curr; /* AP01379821 (16->32 bits) */

		/* sort by ARUUID */

		dev->explicit_ar.sort.cnt = 0;

		{
		LSA_UINT16 ar_nr;

		for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) { /* see cm_sv_ar_alloc() */

			CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

			if( cm_is_not_null(argr) ) {

				if( dev->explicit_ar.sort.cnt >= CM_CFG_MAX_SV_ARS ) {
					CM_FATAL();
				}

				dev->explicit_ar.sort.arr[dev->explicit_ar.sort.cnt] = argr; /* append */

				dev->explicit_ar.sort.cnt += 1;

				{
				LSA_UINT16 idx = dev->explicit_ar.sort.cnt;

				while( --idx > 0 ) { /* bubble sort */

					CM_AR_GRAPH_PTR_TYPE a = dev->explicit_ar.sort.arr[idx - 0];
					CM_AR_GRAPH_PTR_TYPE b = dev->explicit_ar.sort.arr[idx - 1];

					if( cm_sv_dev_uuid_less_than(&a->ar.ar_uuid, &b->ar.ar_uuid) ) { /* if a < b then swap and continue */

						dev->explicit_ar.sort.arr[idx - 0] = b;
						dev->explicit_ar.sort.arr[idx - 1] = a;
					}
					else {

						break;
					}
				}
				}
			}
		}
		}

		/* find the left and right of current */

		{
		LSA_UINT16 idx;

		for( idx = 0; idx < dev->explicit_ar.sort.cnt; ++idx ) {

			if( dev->explicit_ar.sort.arr[idx] == curr_argr ) {

				break;
			}
		}

		if( idx >= dev->explicit_ar.sort.cnt ) {
			CM_FATAL(); /* must be found */
		}

#if CM_CFG_MAX_SERVERS > 1
/* note that there is still only one server per NIC, no 'bits' are needed in the provider sequence */
#endif

#if CM_CFG_MAX_SV_DEVICES > 256
# error "cannot support more than 256 devices. only 8 bits for 'device' in the provider sequence"
#elif CM_CFG_MAX_SV_ARS_RTC123UDP > 16
# error "cannot support more than 16 ARs. only 16 bits for 'AR' in the provider sequence. see hard coded 0xFFFF and 0x10000"
#elif CM_CFG_MAX_SV_ARS_RTC123UDP != 9 /* AP01379821 (8->9 ARs) */
# error "who changed that? AP01379821 requires only 9 ARs (the maximum supported by PNIP)"
#elif CM_AR_IOCR_SEQUENCE_SV_BASE != 0x20000000
# error "who changed that?"
#elif CM_AR_IOCR_SEQUENCE_SV_COUNT != (1UL << 24)
# error "who changed that? we need 24 bits (8 for 'device', 16 for 'AR')"
#endif

		if( idx >= 1 ) {

			seq_left = dev->explicit_ar.sort.arr[idx - 1]->cm.sv.provider_sequence;

			seq_left -= CM_AR_IOCR_SEQUENCE_SV_BASE; /* rebase to zero */

			seq_left &= 0xFFFF; /* the lower 16 bits are the sequence */
		}
		else {

			seq_left = 0;
		}

		if( idx + 1 < dev->explicit_ar.sort.cnt ) {

			seq_right = dev->explicit_ar.sort.arr[idx + 1]->cm.sv.provider_sequence;

			seq_right -= CM_AR_IOCR_SEQUENCE_SV_BASE; /* rebase to zero */

			seq_right &= 0xFFFF; /* the lower 16 bits are the sequence */
		}
		else {

			seq_right = 0x10000; /* 0x10000 = 2^16 */
		}
		}

		dev->explicit_ar.sort.cnt = 0; /* sanity */

		/* calculate */

		seq_curr = (seq_left + seq_right) / 2;

		if( seq_curr == 0 ) { /* may happen if more than CM_CFG_MAX_SV_ARS_RTC123UDP come and go! */

			seq_curr = 1;
		}

		if( seq_curr > 0xFFFF ) {

			CM_FATAL(); /* unreachable, see integer division above */
		}

		/* construct: (device << 16) + sequence + base, 8 bits for device, 16 bits for sequence */

		seq_curr += (((dev->device_nr - 1) & 0xFFUL) << 16);

		seq_curr += CM_AR_IOCR_SEQUENCE_SV_BASE;

		curr_argr->cm.sv.provider_sequence = seq_curr;
	}
}

/*===========================================================================*/
/*=== AR ALLOC ==============================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_ar_alloc(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_SV_ARSET_PTR_TYPE arset,
	CM_AR_GRAPH_PTR_TYPE argr
) {
	LSA_UINT16 free_ar_nr = 0;

	CM_ASSERT(dev->state == CM_SV_DEV_STATE_ADDED);

	CM_ASSERT(argr->device_nr == dev->device_nr);
	CM_ASSERT(argr->ar_nr == 0);
	CM_ASSERT(argr->session_key == 0);

	/***/

	if( ! cm_is_iosar_with_device_access(argr) ) {

		if( ! dev->gsd_SharedDeviceSupported ) {

			/* note: by design not incorporated into loop below; search for "IO_SupervisorSupported" (semantics of max_ars_IOS) */

			LSA_UINT16 ar_nr;

			for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX/*!*/; ++ar_nr ) {

				CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

				if( cm_is_not_null(temp_argr) ) { /* another AR exists */

					if( cm_is_not_null(temp_argr->cm.sv.arset) && temp_argr->cm.sv.arset == arset ) {

						/* ok, this AR is the second AR of an AR-set */
					}
					else {

						CM_SV_TRACE_00(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
							, "no free ar-resource (SharedDeviceSupported false and an AR exists)"
							);

						return LSA_FALSE;
					}
				}
			}
		}
	}

	/*
	 * allocation: explicit_ar.argr_arr[0|<- IOCARs ->|<- IOSARs ->|<- DevAccARs ->]
	 */

	{
	LSA_UINT16 from;
	LSA_UINT16 to;

	if( cm_is_iosar_with_device_access(argr) ) { /* easy-supervisor */

		from = dev->explicit_ar.max_ARs_IOX + 1;
		to   = dev->explicit_ar.max_ARs_TOT;
	}
	else { /* RTClass 1/2/3/UDP */

		if( argr->ar.ar_type == CM_AR_TYPE_SUPERVISOR ) { /* IOSAR */

			from = dev->explicit_ar.max_ARs_IOC + 1;
			to   = dev->explicit_ar.max_ARs_IOX;
		}
		else { /* IOCAR or IOCARSR */

			from = 1;
			to   = dev->explicit_ar.max_ARs_IOC;
		}
	}

	/***/

	{
	LSA_UINT16 fr_cnt = 0;
	LSA_UINT16 sr_cnt = 0;
	LSA_UINT16 ar_nr;

	for( ar_nr = from; ar_nr <= to; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE temp_argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_null(temp_argr) ) { /* free */

			if( free_ar_nr == 0 ) {

				free_ar_nr = ar_nr; /* use the first free ar_nr */
			}

			fr_cnt += 1;
		}
		else if( cm_is_not_null(temp_argr->cm.sv.arset) ) {

#if CM_CFG_MAX_SV_ARSETS_PER_DEVICE != 1
#error "must change alloc/free, session_key etc."
#endif

#if CM_SV_ARSET_MAX_ARS != 2
#error "who changed that?"
#endif

			sr_cnt += 1;
		}
	}

	/***/

	if( free_ar_nr != 0 && from == 1 ) { /* IOCAR or IOCARSR */

		/* note: if SRD_enabled == CM_SV_DEVICE_SRD_ENABLED_YES then max_ARs_IOC >= 2 */
		/* note: all SR-ARs have ARUUID::Selector with "two ARs" */

		if( cm_is_not_null(arset) ) {

			if( sr_cnt == 0 ) {

				if( fr_cnt < 2 ) {

					free_ar_nr = 0; /* don't have "two ARs" left (cannot accept first) */
				}
			}
			else if( sr_cnt == 1 ) {

				if( fr_cnt < 1 ) {

					free_ar_nr = 0; /* unreachable (see below) */
				}
			}
			else {

				free_ar_nr = 0; /* unreachable */
			}
		}
		else {

			if( sr_cnt == 0 ) {

				/* ok */
			}
			else if( sr_cnt == 1 ) {

				if( fr_cnt < 2 ) {

					free_ar_nr = 0; /* cannot accept the IO-AR (nothing left for the other SR-AR) */
				}
			}
			else {

				/* ok */
			}
		}
	}
	}
	}

	/***/

	if( free_ar_nr == 0 ) {

		/* note: traced in caller */

		return LSA_FALSE;
	}
	else {

		if( dev->explicit_ar.ar_cnt >= dev->explicit_ar.max_ARs_TOT ) {
			CM_FATAL(); /* a bug */
		}

		dev->explicit_ar.ar_cnt += 1;

		/***/

		CM_ASSERT(CM_SV_SESSION_KEY_TO_AR_NR(0xFFFF) == 0x00FF); /* note: cannot #if ... because of typecast in macro */

		CM_ASSERT(CM_SV_SESSION_KEY_TO_ARSET_NR(0xFFFF) == 0x0003); /* note: cannot #if ... because of typecast in macro */

		dev->res.skey_arr[free_ar_nr] += 0x0400; /* high-bits = session, index = ar_nr, see CM_SV_DEVICE_TYPE */

		{
		LSA_UINT16 session_key = dev->res.skey_arr[free_ar_nr];

		if( CM_SV_SESSION_KEY_TO_AR_NR(session_key) != free_ar_nr ) {
			CM_FATAL();
		}

		if( CM_SV_SESSION_KEY_TO_ARSET_NR(session_key) != 0 ) {
			CM_FATAL();
		}

		argr->session_key = session_key; /* assign (always != zero) */

		argr->ar_nr = CM_SV_SESSION_KEY_TO_AR_NR(argr->session_key); /* sic! */
		}

		/***/

		if( dev->R1_enabled ) { //R1 marker (session_key left/right)

			/* problem: the user erronously passes a session_key on to other side and uses it there - difficult to track down */
			/* solution: highest bit of session_key (all kinds of ARs) encodes left/right */

			if( (dev->val_8i00 & 0xF000) == 0x9000 ) {
				argr->session_key |= 0x8000; /* 0x9jpp: highest bit set */
			}
			else {
				argr->session_key &= 0x7FFF; /* 0x8ipp: highest bit cleared */
			}
		}

		if( cm_is_not_null(arset) ) {

			if( arset->arset_nr != 1 ) {
				CM_FATAL();
			}

			argr->session_key |= 0x0100; /* AR-set #1 */

			argr->cm.sv.arset = arset; /* assign */

			if( cm_is_null(arset->arr[0]) ) {

				if( cm_is_not_null(arset->first) || cm_is_not_null(arset->primary) ) {
					CM_FATAL();
				}

				arset->arr[0] = argr;
				arset->first  = argr;

				if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) { //R1 marker
					arset->first = LSA_NULL; /* the representative AR is never "first AR" (does not PRM) */
				}
			}
			else if( cm_is_null(arset->arr[1]) ) {

				arset->arr[1] = argr;
			}
			else {

				CM_FATAL();
			}
		}

		/***/

		dev->explicit_ar.argr_arr[free_ar_nr] = argr; /* index = ar_nr, see CM_SV_DEVICE_TYPE */

		/***/

		cm_sv_dev_calc_provider_sequence(dev, argr); /* after inserting into explicit_ar.argr_arr[] */

		/***/

		CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev/ar(%u/%u) argr(%08X) sk(0x%x) provider_sequence(%u) ARBlockReq::SessionKey(%u)"
			, argr->device_nr, argr->ar_nr
			, argr, argr->session_key, argr->cm.sv.provider_sequence, argr->cm.sv.session_key_connect
			);

		return LSA_TRUE;
	}
}

/*===========================================================================*/
/*=== ABORT ARs WITH "SC-PROBLEM" IN ALL DEVICES ============================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_abort_sc_problem(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 pdev_send_clock
) {

	if( pdev_send_clock != 0 ) {

		LSA_UINT16 device_nr;

		for( device_nr = 1; device_nr <= CM_CFG_MAX_SV_DEVICES; ++device_nr ) {

			CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

			if( cm_is_not_null(dev) ) {

				LSA_UINT16 ar_nr;

				for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

					CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

					if( cm_is_not_null(argr) ) {

						if( argr->ar_com.ar_send_clock != pdev_send_clock ) {

							cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, CM_AR_REASON_PDEV, 0);
							cm_ar_com_down(argr, CM_AR_REASON_PDEV); /* AP00914416 */
						}
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== AR FREE ===============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_free(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT32 pnio_err,
	LSA_UINT32 entry_detail
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	LSA_UINT16 ar_nr = argr->ar_nr; /* save */
	LSA_UINT16 pdev_send_clock;

	CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
		, "dev/ar(%u/%u) AR gone, pnio_err(0x%x), entry_detail(0x%x), ar_cnt(%u)"
		, argr->device_nr, argr->ar_nr
		, pnio_err, entry_detail
		, dev->explicit_ar.ar_cnt
		);

	/***/

	if( entry_detail != CM_AR_REASON_NONE ) {

		cm_sv_logbook_write(argr, pnio_err, entry_detail); /* note: the ar_uuid may be NIL */
	}

	/***/

	if( cm_is_not_null(argr->cm.sv.arset) ) {

		CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

		if( arset->first == argr ) {
			arset->first = LSA_NULL;
		}

		if( arset->primary == argr ) { /* always false */
			arset->primary = LSA_NULL;
		}

		if( arset->arr[0] == argr ) {

			arset->arr[0] = arset->arr[1]; /* shift down */
			arset->arr[1] = LSA_NULL;
		}
		else if( arset->arr[1] == argr ) {

			arset->arr[1] = LSA_NULL;
		}
		else {

			CM_FATAL();
		}

		/***/

		if( argr->session_key == CM_GET_PRIMARY_AR(arset->arset_nr, dev->channel->sysptr) ) { /* note: testcode to find a user-bug earlier */

			CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
				, "dev/ar(%u/%u) user did not update the 'PrimaryARID' (via eddx_ProviderBufferRequest_SetDS) on disconnect!"
				, argr->device_nr, argr->ar_nr
				);

			CM_FATAL();
		}

		/***/

		if( cm_is_null(arset->arr[0]) ) { /* AR-set is empty, cleanup */

			arset->is_aborting = LSA_FALSE;

			if( cm_is_not_null(arset->abort_rqb) ) {

				cm_callback(dev->channel, CM_OK, &arset->abort_rqb);
			}

			arset->first = LSA_NULL;
			arset->primary = LSA_NULL;

			{
			LSA_UINT16 prio;

			for( prio = 0; prio < 2; ++prio ) {

				arset->alarm[prio].sequence_number = 0;

				if( ! CmListIsEmpty(&arset->alarm[prio].queue) ) {
					CM_FATAL();
				}
			}
			}

			/***/

			{ //R1 VV: make a function. maybe too for normal AR-set.
			CM_SV_ARSET_R1B_PTR_TYPE r1b = &arset->r1b;
			LSA_UINT16 prio;

			r1b->wait_for_primary_trigger = LSA_FALSE;

			for( prio = 0; prio < 2; ++prio ) {

				r1b->alarm.curr[prio].is_used = LSA_FALSE; //R1 VV: invalidate when AR-set aborts and assert LSA_FALSE here
			}

			/***/

			{ //R1 VV: the AR-set seems not to be the right place for "pd_alarm" (wf-posted is not related to ARs)
			CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE r1pd_alarm = &r1b->pd_alarm;

			if( cm_is_not_null(r1pd_alarm->orig) ) { /* TIA 2253099 */

				if( r1pd_alarm->wf_set_posted ) { /* TIA 3282384 (V6.1i2.22), TIA 3282383 (V7.0i3.10), TIA 3282382 (V7.1i2.7) */

					r1pd_alarm->orig_is_completed = LSA_TRUE; /* cm_sv_r1b_posted_pdev_alarm_mirror_set() will return "orig" to CM-PD */
				}
				else if( r1pd_alarm->wf_clr_posted ) {

					/* cm_sv_r1b_posted_pdev_alarm_mirror_clr() will return "orig" to CM-PD */
				}
				else {

					r1pd_alarm->wf_clr_posted = LSA_TRUE;

					cm_sv_r1b_posted_pdev_alarm_mirror_clr(dev, r1pd_alarm->msg_mirror_clr); /* fake */
				}
			}

			if( r1pd_alarm->wf_set_posted || r1pd_alarm->wf_clr_posted ) { /* TIA 2253099 */
				/* not: CM_FATAL() -- cm_sv_r1b_pdev_alarm_mirror_join_done() returns "orig" to CM-PD */
				//R1 VV: wait explicitly
			}

			if( cm_is_not_null(r1pd_alarm->mirr) ) { /* pre-allocated if R1 (not allocated if S2) */

				if( CM_RQB_GET_OPCODE(r1pd_alarm->mirr) != 0/*unused*/ ) { /* TIA 2253099 */

					CM_RQB_SET_OPCODE(r1pd_alarm->mirr, 0/*unused*/);

					r1pd_alarm->mirr_is_completed = LSA_FALSE; /* reset */
				}
			}

			if( r1pd_alarm->mirr_is_completed ) {
				CM_FATAL(); /* a bug, see cm_sv_r1b_handle_pdev_alarm_mirror_clr() */
			}

			if( r1pd_alarm->wf_cancel_posted ) { /* TIA 2253099 */
				/* not: CM_FATAL() -- cm_sv_r1b_posted_pdev_alarm_mirror_cancel() just resets the flag */
				//R1 VV: wait explicitly
			}
			}
			}
		}
	}

	/***/

	pdev_send_clock = cm_sv_argr_undo_init_and_free(argr);

	/***/

	if( ar_nr != 0 ) { /* see cm_sv_ar_alloc() */

		CM_CHANNEL_PTR_TYPE channel = dev->channel;

		/***/

		if( ! (ar_nr >= 1 && ar_nr <= dev->explicit_ar.max_ARs_TOT) ) {
			CM_FATAL();
		}

		if( dev->explicit_ar.argr_arr[ar_nr] != argr ) { /* note: "argr" is freed here */
			CM_FATAL(); /* a bug */
		}

		dev->explicit_ar.argr_arr[ar_nr] = LSA_NULL; /* index = ar_nr, see CM_SV_DEVICE_TYPE */

		/***/

		if( dev->explicit_ar.ar_cnt < 1 ) {
			CM_FATAL(); /* a bug */
		}

		dev->explicit_ar.ar_cnt -= 1;

		/***/

		cm_sv_abort_sc_problem(channel, pdev_send_clock);

		/***/

		if( dev->explicit_ar.ar_cnt == 0 ) { /* last AR gone */

			cm_sv_dev_done(dev); /* trigger: all explicit ARs gone */

			/***/

			if( channel->usr.sv.tell_all_ARs_gone ) {

				if( cm_sv_all_ARs_gone(channel) ) {

					CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_PD);

					channel->usr.sv.tell_all_ARs_gone = LSA_FALSE;

					cm_pd_rtf_nos_ip_changed_done(pd_channel, CM_PD_RTF_WF__SV_DOWN);
				}
			}
		}

		if( dev->R1_enabled ) { //R1 marker (special case because of representative-AR)

			/* note: SetNoS/SetIP does not abort the representative-AR (i.e.: dev->explicit_ar.ar_cnt != 0) */

			if( channel->usr.sv.tell_all_ARs_gone ) {

				if( cm_sv_all_ARs_gone(channel) ) {

					CM_CHANNEL_PTR_TYPE pd_channel = cm_peer_channel_get(channel, CM_PATH_TYPE_PD);

					channel->usr.sv.tell_all_ARs_gone = LSA_FALSE;

					cm_pd_rtf_nos_ip_changed_done(pd_channel, CM_PD_RTF_WF__SV_DOWN);
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== AR LOOKUP1 ============================================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup1(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr
) {
	CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

	if( cm_is_not_null(dev) ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		return argr;
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== AR LOOKUP2 ============================================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup2(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT16 ar_nr
) {

	if( ar_nr >= 1 && ar_nr <= dev->explicit_ar.max_ARs_TOT ) {

		CM_AR_GRAPH_PTR_TYPE argr = dev->explicit_ar.argr_arr[ar_nr]; /* index = ar_nr, see CM_SV_DEVICE_TYPE */

		return argr;
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== AR LOOKUP3 ============================================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup3(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 session_key,
	LSA_UINT32 cn_flag /* 0 or CM_SV_AR_CNFL_EXP_USER_REQ, CM_SV_AR_CNFL_WF_CONN_RSP, ... */
) {
	CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

	if( cm_is_not_null(dev) ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, CM_SV_SESSION_KEY_TO_AR_NR(session_key));

		if( cm_is_not_null(argr) && argr->session_key == session_key ) {

			if( cn_flag == 0 || (argr->cm.sv.cn_flags & cn_flag) != 0 ) {

				return argr;
			}
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== AR LOOKUP BY ARUUID ===================================================*/
/*===========================================================================*/

CM_AR_GRAPH_PTR_TYPE
cm_sv_ar_lookup_by_aruuid(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CLRPC_UUID_CONST_PTR_TYPE uuid
) {
	LSA_UINT16 ar_nr;

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_TOT; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(argr) ) {

			if( cm_uuid_equal(uuid, &argr->ar.ar_uuid) ) {

				return argr;
			}
		}
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== ABORT ALL ARS OF A DEVICE =============================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_ar_abort_all(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT8 reason
) {
	LSA_UINT16 ar_nr;

	CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar_cnt(%u) reason(%u)"
		, dev->device_nr, dev->explicit_ar.ar_cnt, reason
		);

	for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_TOT; ++ar_nr ) {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

		if( cm_is_not_null(argr) ) {

			CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
				, "dev(%u) aborting ar(%u), reason(%u)"
				, argr->device_nr, argr->ar_nr, reason
				);

			argr = cm_sv_ar_unpark(argr, reason); /* case: before cm_ar_com_ready() */

			if( cm_is_not_null(argr) ) {

				cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, reason, 0);
				cm_ar_com_down(argr, reason);
			}
		}
	}

	cm_sv_dev_done(dev); /* trigger: abort-all (actually a testcase only) */
}

/*===========================================================================*/
/*=== SENDCLOCK-CHANGED TO ALL ARS OF ALL DEVICES ===========================*/
/*===========================================================================*/

LSA_VOID
cm_sv_sendclock_changed(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 send_clock /* 0 if failed */
) {
	CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "send_clock(%u)", send_clock
		);

	if( channel->state != CM_SV_CREATED ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "channel-state(%u) not created, ignored"
			, channel->state
			);
	}
	else {

		LSA_UINT16 device_nr;

		for( device_nr = 1; device_nr <= CM_CFG_MAX_SV_DEVICES; ++device_nr ) {

			CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

			if( cm_is_not_null(dev) ) {

				LSA_UINT16 ar_nr;

				/* must call the trigger too if passivating/removing or the ARs will hang (AP00606815) */

				for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_IOX; ++ar_nr ) {

					CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

					if( cm_is_not_null(argr) ) {

						cm_sv_ar_sendclock_changed(argr, send_clock);
					}
				}
			}
		}
	}
}

/*===========================================================================*/
/*=== ALL ARs GONE ==========================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_all_ARs_gone( /* returns true if all ARs of all devices are gone */
	CM_CHANNEL_CONST_PTR_TYPE channel
) {
	LSA_UINT16 device_nr;

	for( device_nr = 1; device_nr <= CM_CFG_MAX_SV_DEVICES; ++device_nr ) {

		CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

		if( cm_is_not_null(dev) ) {

			if( dev->R1_enabled ) { //R1 marker (special case because of representative-AR)

				/* note: cm_sv_pdev_Rtf_NoS_IpSuite() skips the representative-AR, thus dev->explicit_ar.ar_cnt != 0 */

				LSA_UINT16 ar_nr;

				for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_TOT; ++ar_nr ) {

					CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

					if( cm_is_not_null(argr) ) {

						if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {
							continue; /* skip, see cm_sv_pdev_Rtf_NoS_IpSuite() */
						}

						return LSA_FALSE; /* not all gone */
					}
				}
			}
			else {

				if( dev->explicit_ar.ar_cnt != 0 ) {

					return LSA_FALSE; /* not all gone */
				}
			}
		}
	}

	return LSA_TRUE; /* all gone */
}

/*===========================================================================*/
/*=== RESET-TO-FACTORY OR NAME-OF-STATION OR IP-SUITE CHANGED ===============*/
/*===========================================================================*/

LSA_BOOL
cm_sv_pdev_Rtf_NoS_IpSuite(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8 reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
) {
	if( channel->state != CM_SV_CREATED ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "channel-state(%u) not created, ignored"
			, channel->state
			);

		return LSA_TRUE; /* all ARs gone */
	}
	else {

		{
		LSA_UINT16 device_nr;

		for( device_nr = 1; device_nr <= CM_CFG_MAX_SV_DEVICES; ++device_nr ) {

			CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

			if( cm_is_not_null(dev) ) {

				LSA_UINT16 ar_nr;

				for( ar_nr = 1; ar_nr <= dev->explicit_ar.max_ARs_TOT; ++ar_nr ) {

					CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, ar_nr);

					if( cm_is_not_null(argr) ) {

						if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) { //R1 marker (special case because of representative-AR)
							/* note: must not abort the representative-AR (only original-AR and "bridge-broken" may kill it) */
							continue; /* skip, see too cm_sv_all_ARs_gone() */
						}

						CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
							, "dev(%u) aborting ar(%u), reason(%u)"
							, argr->device_nr, argr->ar_nr, reason
							);

						cm_ar_com_down_debug_data_fmt(argr, CM_MODULE_ID, __LINE__, reason, 0);
						cm_ar_com_down(argr, reason);
					}
				}

				/***/

				if( dev->R1_enabled && reason == CM_AR_REASON_DCP_RESET ) { //R1 marker (signal RTF to other side)

					/* the effects of RTF are specified in TIA 1240352.
					 * as of november 2015 the common profile system redundancy is under review.
					 *
					 * note: shutting down the ARs may take a while. thus it is a possible case
					 * that pd-prm-begin from the other side comes while RTF is still running.
					 * CM-PD will queue the request and execute it after completion of RTF.
					 *
					 * see to the comments in cm_pd_config_start() for other szenarios.
					 */

					cm_sv_r1b_post_reset_to_factory(dev);
				}
			}
		}
		}

		/***/

		if( channel->usr.sv.tell_all_ARs_gone ) {

			CM_FATAL(); /* a bug (CM-PD did not wait) */
		}

		if( cm_sv_all_ARs_gone(channel) ) {

			return LSA_TRUE; /* all ARs gone */
		}
		else {

			channel->usr.sv.tell_all_ARs_gone = LSA_TRUE;

			return LSA_FALSE; /* not all ARs gone */
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
