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
/*  F i l e               &F: cm_sv25.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Logbook things, R1 reset-to-factory                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7250
#define CM_MODULE_ID       7250

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== LOGBOOK INIT ==========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_logbook_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	dev->logbook.wrt_idx = 0;
	dev->logbook.log_cnt = 0;
}

/*===========================================================================*/
/*=== LOGBOOK UNDO-INIT =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_logbook_undo_init(
	CM_SV_DEVICE_PTR_TYPE dev
) {

	LSA_UNUSED_ARG(dev);
	/* nothing */
}

/*===========================================================================*/
/*=== LOGBOOK WRITE =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_logbook_write(
	CM_AR_GRAPH_CONST_PTR_TYPE argr,
	LSA_UINT32 pnio_err,
	LSA_UINT32 entry_detail
) {
	CM_SV_DEVICE_PTR_TYPE dev = argr->cm.sv.dev;
	CM_SV_LOG_ENTRY_PTR_TYPE log_ptr;

	/***/

	if( dev->logbook.wrt_idx >= CM_CFG_MAX_SV_LOG_ENTRIES ) {
		CM_FATAL(); /* a bug */
	}

	log_ptr = &dev->logbook.buf[dev->logbook.wrt_idx];

	/***/

	{
	LSA_UINT16 ret_val;

	CM_GET_CYCLE_COUNTER(&ret_val, dev->channel->sysptr, &log_ptr->local_time_stamp);

	if( ret_val != LSA_RET_OK ) {

		log_ptr->local_time_stamp = 0;
	}
	}

	log_ptr->ar_uuid      = argr->ar.ar_uuid;
	log_ptr->pnio_status  = pnio_err;
	log_ptr->entry_detail = entry_detail;

	/***/

	++dev->logbook.wrt_idx;

	if( dev->logbook.wrt_idx >= CM_CFG_MAX_SV_LOG_ENTRIES ) { /* wrap around */

		dev->logbook.wrt_idx = 0;
	}

	if( dev->logbook.log_cnt < CM_CFG_MAX_SV_LOG_ENTRIES ) { /* limit */

		++dev->logbook.log_cnt;
	}
}

/*===========================================================================*/
/*=== LOGBOOK HAS ENTRIES ===================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_logbook_has_entries(
	CM_SV_DEVICE_CONST_PTR_TYPE dev
){

	if( dev->logbook.log_cnt > 0 ) {

		return LSA_TRUE;
	}
	else {

		return LSA_FALSE;
	}
}

/*===========================================================================*/
/*=== LOGBOOK MARSHAL =======================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_logbook_marshal(
	CM_SV_DEVICE_PTR_TYPE dev,
	CLRPC_UPPER_MEM_U8_PTR_TYPE data,
	LSA_UINT32 size
) {
	LSA_UINT32 max;
	LSA_UINT32 idx;
	LSA_UINT16 cnt;

	CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) size(%u)"
		, dev->device_nr, size
		);

	CM_ASSERT(size <= 0xFFFF - (1+1 + 8 + 2)); /* see caller */

	max = size / CM_PDU_LOG_ENTRY_SIZE; /* max. number of entries that fit into the buffer */

	if( max > dev->logbook.log_cnt ) {
		max = dev->logbook.log_cnt;
	}

	idx = dev->logbook.wrt_idx - max; /* start-index */

	if( idx > CM_CFG_MAX_SV_LOG_ENTRIES ) {
		idx += CM_CFG_MAX_SV_LOG_ENTRIES; /* handle wrap-around */
	}

	CM_ASSERT(idx < CM_CFG_MAX_SV_LOG_ENTRIES);

	for( cnt = 0; cnt < max; ++cnt ) {

		CM_SV_LOG_ENTRY_PTR_TYPE log_ptr;

		log_ptr = &dev->logbook.buf[idx];

		/***/

		CM_PUT32_HTON(data, CM_PDU_LOG_ENTRY_local_time_stamp_high, (LSA_UINT32)(log_ptr->local_time_stamp >> 32));
		CM_PUT32_HTON(data, CM_PDU_LOG_ENTRY_local_time_stamp_low,  (LSA_UINT32)(log_ptr->local_time_stamp >> 0));

		CM_PUT_UUID_HTON(data, CM_PDU_LOG_ENTRY_ar_uuid, &log_ptr->ar_uuid);

		CM_PUT32_HTON(data, CM_PDU_LOG_ENTRY_pnio_status,  log_ptr->pnio_status);
		CM_PUT32_HTON(data, CM_PDU_LOG_ENTRY_entry_detail, log_ptr->entry_detail);

		/***/

		data += CM_PDU_LOG_ENTRY_SIZE;

		/***/

		++idx;

		if( idx >= CM_CFG_MAX_SV_LOG_ENTRIES ) {
			idx = 0;
		}
	}

	/***/

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "nr_of_entries(%u)"
		, cnt
		);

	return cnt;
}

/*===========================================================================*/
/*=== R1-BRIDGE POST RESET-TO-FACTORY =======================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_post_reset_to_factory(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_ASSERT(dev->R1_enabled);

	if( dev->r1b.bridge_state != CM_SV_DEV_R1B_STATE_ESTABLISHED ) { /* TIA 1737437 */

		/* do nothing */
	}
	else {

		CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix = cm_sv_r1b_prefix_alloc(dev, CM_SV_R1BMSG_SIZE_RESET_TO_FACTORY);

		if( cm_is_null(prefix) ) {

			CM_FATAL(); /* note: unlikely (pre-allocation needs serialization with CM-PD) */
		}
		else {

			CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "dev(%u) R1 sending reset-to-factory to other head"
				, dev->device_nr
				);

			CM_ASSERT(CM_SV_R1BMSG_SIZE_RESET_TO_FACTORY == 0U); /* no parameters */

			cm_sv_r1b_post1(dev, CM_SV_R1BRIDGE_MSG_RESET_TO_FACTORY, CM_SV_R1BMSG_SIZE_RESET_TO_FACTORY, prefix);
		}
	}
}

/*===========================================================================*/
/*=== R1-BRIDGE POSTED RESET-TO-FACTORY =====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_posted_reset_to_factory(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_R1BMSG_PREFIX_PTR_TYPE prefix
) {

	cm_sv_r1b_prefix_free(dev->channel, prefix); /* dyn-allocated in cm_sv_r1b_post_reset_to_factory() */

	/* join-on-broken: see cm_sv_r1b_join_broken() */
}

/*===========================================================================*/
/*=== R1-BRIDGE HANDLE RESET-TO-FACTORY =====================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_r1b_handle_reset_to_factory(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_RQB_PTR_TYPE rb,
	CM_COMMON_MEM_U8_PTR_TYPE message_body, /* note: this is an alias to the RQB */
	LSA_UINT32 message_size
) {
	if( message_size != CM_SV_R1BMSG_SIZE_RESET_TO_FACTORY ) {

		CM_SV_TRACE_02(dev->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "dev(%u) R1 protocol error (wrong message_size(%u))"
			, dev->device_nr, message_size
			);

		CM_FATAL(); /* R1 protocol error */
	}
	else {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) R1 received reset-to-factory from other head"
			, dev->device_nr
			);

		LSA_UNUSED_ARG(message_body); /* no parameters */

		CM_ASSERT(CM_SV_R1BMSG_SIZE_RESET_TO_FACTORY == 0U);

		cm_callback(dev->channel, CM_OK, &rb);

		/***/

		{
		CM_SV_AP_PTR_TYPE ap = cm_sv_ap_lookup(dev, 0/*!*/);

		if( cm_is_not_null(ap) ) {

			CM_SV_SLOT_PTR_TYPE sl;

			for( CmListEach(sl, &ap->slot_list, CM_SV_SLOT_PTR_TYPE) ) {

				CM_SV_SUBSLOT_PTR_TYPE su;

				for( CmListEach(su, &sl->subslot_list, CM_SV_SUBSLOT_PTR_TYPE) ) {

					if( CM_SV_SUBSLOT_IS_PDEV_REMOTE(dev, su->subslot_nr) ) {

						CM_AR_GRAPH_PTR_TYPE owner_argr = (su->owner_ar_nr != 0 ? cm_sv_ar_lookup2(dev, su->owner_ar_nr) : LSA_NULL);

						if( cm_is_not_null(owner_argr) && (owner_argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

							owner_argr->cm.sv.r1_flags |= CM_SV_AR_R1FL_REMOTE_RTF;

							cm_sv_ar_submodule_state_changed(owner_argr, 0/*!*/, sl->slot_nr, su->subslot_nr); /* R1 handle reset-to-factory */
						}
					}
				}
			}
		}
		}

		cm_sv_dev_submodule_or_diag_changed_trigger(dev); /* R1 handle reset-to-factory */
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
