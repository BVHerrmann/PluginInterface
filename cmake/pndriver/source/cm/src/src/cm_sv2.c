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
/*  F i l e               &F: cm_sv2.c                                  :F&  */
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
/*  - device get, lookup, etc...                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7200
#define CM_MODULE_ID       7200

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

/*===========================================================================*/
/*=== DEVICE LOOKUP =========================================================*/
/*===========================================================================*/

CM_SV_DEVICE_PTR_TYPE
cm_sv_dev_lookup(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr
) {

	if( device_nr >= 1 && device_nr <= CM_CFG_MAX_SV_DEVICES ) {

		return channel->usr.sv.dev[device_nr]; /* index = device_nr, see CM_SV_CHANNEL_TYPE */
	}

	return LSA_NULL;
}

/*===========================================================================*/
/*=== DEVICE GET/ADDED ======================================================*/
/*===========================================================================*/

CM_SV_DEVICE_PTR_TYPE
cm_sv_dev_get_added(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	LSA_UINT16 device_nr
) {

	CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_lookup(channel, device_nr);

	if( cm_is_null(dev) || dev->state != CM_SV_DEV_STATE_ADDED ) {

		/* note: traced in caller where helpful */

		return LSA_NULL;
	}

	return dev;
}

/*===========================================================================*/
/*=== DEVICE ALLOC-AND-LINK =================================================*/
/*===========================================================================*/

CM_SV_DEVICE_PTR_TYPE
cm_sv_dev_alloc_and_link(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_SV_DEVICE_ADD_CONST_PTR_TYPE param
) {
	CM_LOCAL_MEM_PTR_TYPE local_mem;
	CM_SV_DEVICE_PTR_TYPE dev;

	CM_ALLOC_LOCAL_MEM(&local_mem, sizeof(*dev));
	dev = (CM_SV_DEVICE_PTR_TYPE)local_mem;

	if( cm_is_null(dev) ) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "cannot alloc local-mem"
			);
	}
	else {

		dev->channel = channel; /* backlink to the channel */

		dev->trace_idx = channel->trace_idx; /* a copy to avoid the indirection */

		dev->device_nr = param->device_nr;

		dev->state = CM_SV_DEV_STATE_ADDING;

		dev->is_active = LSA_FALSE;

		dev->is_parking = (param->parking_mode == CM_SV_PARKING_MODE_YES ? LSA_TRUE : LSA_FALSE); /* AP00654135, AP00975630 */

		dev->curr = LSA_NULL;

		dev->read = LSA_NULL;

		/***/

		dev->vendor_id = param->vendor_id;
		dev->device_id = param->device_id;
		dev->instance  = param->instance;

		dev->max_slot_nr        = param->max_slot_nr;
		dev->max_subslot_nr     = param->max_subslot_nr;
		dev->max_nr_of_subslots = param->max_nr_of_subslots;

		if( param->alarm_send_max_length < (CM_ALARM_DATA_LENGTH_MIN - CM_ALARM_OFFSET_DATA) ) {

			dev->alarm_send_max_length = (CM_ALARM_DATA_LENGTH_MIN - CM_ALARM_OFFSET_DATA);
		}
		else {

			dev->alarm_send_max_length = param->alarm_send_max_length;
		}

		if( param->contains_pdev == CM_SV_CONTAINS_PDEV_YES ) { /* AP00995373 */

			LSA_UINT16 map_count  = 0;
			LSA_UINT16 subslot_nr = cm_pdsv_get_interface_subslot_nr(channel, &map_count);

			if(
				map_count < 2 /* interface and at least 1 port */
				|| (param->R1_enabled == CM_SV_DEVICE_R1_ENABLED_YES ? ((subslot_nr & 0xE0FF) != 0x8000) : ((subslot_nr & 0xF0FF) != 0x8000))
			) {
				CM_FATAL(); /* a bug in CM-PD */
			}

			dev->val_8i00 = subslot_nr; /* 0x8i00 or 0x9j00 */

			dev->nr_of_pdev_mappings = map_count;

			CM_ASSERT(CM_SV_DEV_IS_PDEV_DEVICE(dev));
		}
		else {

			dev->val_8i00 = 0; /* is not the pdev-device */

			dev->nr_of_pdev_mappings = 0;
		}

		dev->central_pdev = param->central_pdev == CM_SV_CENTRAL_PDEV_YES ? LSA_TRUE : LSA_FALSE; /* AP01066848 */

		dev->pdev_owner_ar_nr = 0;

		dev->write_multiple_parallelism = param->write_multiple_parallelism; /* AP00786836 */

		dev->SRD_enabled = param->SRD_enabled == CM_SV_DEVICE_SRD_ENABLED_YES ? LSA_TRUE : LSA_FALSE;

		dev->CIR_enabled = param->CIR_enabled == CM_SV_DEVICE_CIR_ENABLED_YES ? LSA_TRUE : LSA_FALSE;

		dev->R1_enabled  = param->R1_enabled == CM_SV_DEVICE_R1_ENABLED_YES ? LSA_TRUE : LSA_FALSE;

		dev->R1_bridge_max = param->R1_bridge_max;

		dev->PDevPrmInd_enabled = param->PDevPrmInd_enabled == CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_YES ? LSA_TRUE : LSA_FALSE; /* TIA 1193013 */

		dev->skip_check_100Mbps_FDX = param->skip_check_100Mbps_FDX == CM_SV_DEVICER_SKIP_CHECK_100MBPS_FDX_YES ? LSA_TRUE : LSA_FALSE; /* TIA 1963788 */

		dev->min_device_interval_31250ns = param->min_device_interval_31250ns; /* AP01451871 */

		/* TIA 1453731 */
		dev->gsd_MaxInputLength              = param->gsd_MaxInputLength;
		dev->gsd_MaxOutputLength             = param->gsd_MaxOutputLength;
		dev->gsd_MaxDataLength               = param->gsd_MaxDataLength;
		dev->gsd_NumberOfInputCR             = param->gsd_NumberOfInputCR;
		dev->gsd_NumberOfOutputCR            = param->gsd_NumberOfOutputCR;
		dev->gsd_NumberOfAdditionalInputCR   = param->gsd_NumberOfAdditionalInputCR;
		dev->gsd_NumberOfAdditionalOutputCR  = param->gsd_NumberOfAdditionalOutputCR;
		dev->gsd_NumberOfMulticastProviderCR = param->gsd_NumberOfMulticastProviderCR;
		dev->gsd_NumberOfMulticastConsumerCR = param->gsd_NumberOfMulticastConsumerCR;

		dev->gsd_SharedDeviceSupported = param->gsd_SharedDeviceSupported == CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_YES ? LSA_TRUE : LSA_FALSE; /* TIA 1587591 */

		dev->gsd_ReportingSystemSupported = param->gsd_ReportingSystemSupported == CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_YES ? LSA_TRUE : LSA_FALSE; /* TIA 1645525 */

		dev->gsd_AssetManagementSupported = param->gsd_AssetManagementSupported == CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_YES ? LSA_TRUE : LSA_FALSE; /* TIA 1722142 */

		cm_sv_dev_res_init(dev, param->max_ars_IOC, param->max_ars_IOS, param->max_ars_DAC, param->write_multiple_parallelism);

		/***/

		CmListInitialize(&dev->ap_list);

		cm_sv_explicit_ar_init(dev, param->max_ars_IOC, param->max_ars_IOS, param->max_ars_DAC);

		cm_sv_implicit_ar_init(dev);

		cm_sv_logbook_init(dev);

		CmListInitialize(&dev->led.info_list);

		cm_sv_dev_r1b_init(dev);

		/***/

		if( cm_is_not_null(channel->usr.sv.dev[dev->device_nr]) ) {
			CM_FATAL(); /* a bug */
		}

		channel->usr.sv.dev[dev->device_nr] = dev; /* index = device_nr, see CM_SV_CHANNEL_TYPE */

		/***/

		if( CM_SV_DEV_IS_PDEV_DEVICE(dev) ) {

			CM_ASSERT(channel->usr.sv.pdev_device_nr == 0);

			channel->usr.sv.pdev_device_nr = dev->device_nr;
		}

		/***/

		{
		LSA_BOOL no_mem = LSA_FALSE;

		dev->res.rpc_rqb = cm_rpc_alloc_rqb(channel);

		if( cm_is_null(dev->res.rpc_rqb) ) {

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "cannot alloc rpc-rqb"
				);

			no_mem = LSA_TRUE;
		}

		if( ! no_mem ) {

			LSA_UINT16 max = channel->detailptr->u.sv.rpc.call_resource_count;
			LSA_UINT16 i;

			for( i = 0; i < max; ++i ) {

				CM_RPC_LOWER_RQB_PTR_TYPE rpc;

				rpc = cm_rpc_alloc_rqb(channel);

				if( cm_is_null(rpc) ) {

					CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "cannot alloc rpc-rqb"
						);

					no_mem = LSA_TRUE;

					break;
				}
				else {

					CmListInsertTail(&dev->res.rpc_callQ, (CM_LIST_ENTRY_PTR_TYPE)rpc);
				}
			}
		}

		if( ! no_mem && dev->R1_enabled ) {

			if( ! cm_sv_dev_r1b_alloc(dev) ) {

				no_mem = LSA_TRUE;
			}
		}

		/***/

		if( no_mem ) {

			cm_sv_dev_unlink_and_free(dev);

			dev = LSA_NULL;
		}
		}
	}

	/***/

	return dev;
}

/*===========================================================================*/
/*=== DEVICE UNLINK-AND-FREE ================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_dev_unlink_and_free(
	CM_SV_DEVICE_PTR_TYPE dev
) {
	CM_CHANNEL_PTR_TYPE channel = dev->channel;

	/***/

	CM_ASSERT(dev->pdev_owner_ar_nr == 0);

	if( CM_SV_DEV_IS_PDEV_DEVICE(dev) ) {

		CM_ASSERT(channel->usr.sv.pdev_device_nr == dev->device_nr);

		channel->usr.sv.pdev_device_nr = 0;
	}

	/***/

	if( channel->usr.sv.dev[dev->device_nr] != dev ) {
		CM_FATAL(); /* a bug */
	}

	channel->usr.sv.dev[dev->device_nr] = LSA_NULL; /* index = device_nr, see CM_SV_CHANNEL_TYPE */

	/***/

	CM_ASSERT(CmListIsEmpty(&dev->ap_list));

	cm_sv_explicit_ar_undo_init(dev);

	cm_sv_implicit_ar_undo_init(dev);

	cm_sv_logbook_undo_init(dev);

	CM_ASSERT(CmListIsEmpty(&dev->led.info_list));

	cm_sv_dev_r1b_undo_init(dev);

	/***/

	while( ! CmListIsEmpty(&dev->res.rpc_callQ) ) {

		CM_RPC_LOWER_RQB_PTR_TYPE rpc;

		CmListRemoveHead(&dev->res.rpc_callQ, rpc, CM_RPC_LOWER_RQB_PTR_TYPE);

		cm_rpc_free_rqb(channel, rpc);
	}

	if( cm_is_not_null(dev->res.rpc_rqb) ) {

		cm_rpc_free_rqb(channel, dev->res.rpc_rqb);
	}

	/***/

	CM_ASSERT(cm_is_null(dev->read));

	/***/

	{
	LSA_UINT16 ret_val;
	CM_FREE_LOCAL_MEM(&ret_val, dev);
	CM_ASSERT(ret_val == LSA_RET_OK);
	LSA_UNUSED_ARG(ret_val);
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
