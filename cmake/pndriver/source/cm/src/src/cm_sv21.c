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
/*  F i l e               &F: cm_sv21.c                                 :F&  */
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
/*  - device add                                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7210
#define CM_MODULE_ID       7210

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_bind(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
); /* forward */

static LSA_VOID
cm_sv_rpc_register_if_obj(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
); /* forward */

/*===========================================================================*/
/*=== DEVICE PARAMETER CHECK ================================================*/
/*===========================================================================*/

/* Lint Warning 866 : Unusual use of 'String' in argument to sizeof - Request 1502507*/
#define CM_SV_SIZ(ptr) sizeof(*(ptr))
#define CM_SV_ARR(ptr) sizeof((ptr)->elem)
#define CM_SV_ONE(ptr) sizeof((ptr)->elem[0])
#define CM_SV_TOT(ptr) ( /*lint -e(866) */ ((0xFFFF - CM_SV_SIZ(ptr)) / CM_SV_ONE(ptr)) + (CM_SV_ARR(ptr) / CM_SV_ONE(ptr)) )

static LSA_BOOL
cm_sv_dev_params_ok(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_SV_DEVICE_ADD_CONST_PTR_TYPE param
) {

	if( ! (param->device_nr >= 1 && param->device_nr <= CM_CFG_MAX_SV_DEVICES) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "device_nr(%u) out of range"
			, param->device_nr
			);

		return LSA_FALSE;
	}

	/***/

	{ /* note: must not use strlen(), may access bytes behind the buffer */
	LSA_UINT16 idx;

	for( idx = 0; idx < sizeof(param->annotation); ++idx ) {

		if( param->annotation[idx] == 0 ) {

			break;
		}
	}

	if( idx >= sizeof(param->annotation) ) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "annotation not zero-terminated"
			);

		return LSA_FALSE;
	}
	}

	/***/

#if CM_CFG_MAX_SV_ARS > 255
# error "CM_CFG_MAX_SV_ARS must be <= 255, see CM_EDD_SET_USER_ID_HANDLE_DEV_AR, etc."
#elif CM_CFG_MAX_SV_ARS_RTC123UDP > CM_CFG_MAX_SV_ARS
/* this is ok, see cm_sv_dev_calc_provider_sequence() */
#endif

	if( param->max_ars_IOC > CM_CFG_MAX_SV_ARS || param->max_ars_IOC > CM_CFG_MAX_SV_ARS_RTC123UDP ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "max_ars_IOC(%u) not within 0..CM_CFG_MAX_SV_ARS(%u)/CM_CFG_MAX_SV_ARS_RTC123UDP(%u)"
			, param->max_ars_IOC, CM_CFG_MAX_SV_ARS, CM_CFG_MAX_SV_ARS_RTC123UDP
			);

		return LSA_FALSE;
	}

	if( param->max_ars_IOS != 0 ) { /* unsupported because of AP01034486 */

		/* important: IOS-functionality needs review. Ownership things changed with AP00914416. Alarm cancellation changed with AP01280175. */
		/* important: GSDML IO_SupervisorSupported has different semantics: "... number of Supervisor ARs is included in ... NumberOfAR." */

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "max_ars_IOS(%u) not 0 (IOS not yet supported)"
			, param->max_ars_IOS
			);

		return LSA_FALSE;
	}

	if( param->max_ars_IOS > CM_CFG_MAX_SV_ARS || param->max_ars_IOS > CM_CFG_MAX_SV_ARS_RTC123UDP ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "max_ars_IOS(%u) not within 0..CM_CFG_MAX_SV_ARS(%u)/CM_CFG_MAX_SV_ARS_RTC123UDP(%u)"
			, param->max_ars_IOS, CM_CFG_MAX_SV_ARS, CM_CFG_MAX_SV_ARS_RTC123UDP
			);

		return LSA_FALSE;
	}

	if( param->max_ars_DAC > CM_CFG_MAX_SV_ARS ) {

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "max_ars_DAC(%u) not within 0..CM_CFG_MAX_SV_ARS(%u)"
			, param->max_ars_DAC, CM_CFG_MAX_SV_ARS
			);

		return LSA_FALSE;
	}

	if(
		(param->max_ars_IOC + param->max_ars_IOS + param->max_ars_DAC) < 1
		|| (param->max_ars_IOC + param->max_ars_IOS + param->max_ars_DAC) > CM_CFG_MAX_SV_ARS
	) {

		CM_SV_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "max_ars_IOC(%u) + max_ars_IOS(%u) + max_ars_DAC(%u) not within 1..CM_CFG_MAX_SV_ARS(%u)"
			, param->max_ars_IOC, param->max_ars_IOS, param->max_ars_DAC, CM_CFG_MAX_SV_ARS
			);

		return LSA_FALSE;
	}

	/***/

	if( param->max_slot_nr > 0x7FFF ) { /* see cm_sv_slot_range_ok() */

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "max_slot_nr(%u) out of range"
			, param->max_slot_nr
			);

		return LSA_FALSE;
	}

	if( param->max_subslot_nr > 0x7FFF ) { /* note: 0x8ipp/0x9jpp is allowed too, see cm_sv_subslot_range_ok() */

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "max_subslot_nr(%u) out of range"
			, param->max_subslot_nr
			);

		return LSA_FALSE;
	}

	if( param->max_nr_of_subslots < 1 ) {

		/* we allow that too (for a device that has only device-access ARs) */
	}
	else {

		/* CM_ALLOC_UPPER_MEM cannot allocate more than 0xFFFF bytes */

		LSA_UINT32 oshp = CM_SV_TOT((CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE)0);
		LSA_UINT32 pend = CM_SV_TOT((CM_UPPER_SV_AR_PRM_END_PTR_TYPE)0);
		LSA_UINT32 idat = CM_SV_TOT((CM_UPPER_SV_AR_IN_DATA_PTR_TYPE)0);
		LSA_UINT32 ardy = CM_SV_TOT((CM_UPPER_SV_AR_APPL_READY_PTR_TYPE)0);

		if(
			param->max_nr_of_subslots > oshp
			|| param->max_nr_of_subslots > pend
			|| param->max_nr_of_subslots > idat
			|| param->max_nr_of_subslots > ardy
		) {

			CM_SV_TRACE_05(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "max_nr_of_subslots(%u) > min(%u, %u, %u, %u)"
				, param->max_nr_of_subslots
				, oshp, pend, idat, ardy
				);

			return LSA_FALSE;
		}
	}

#if CM_ALARM_PAYLOAD_MAX != (2 + CM_ALARM_PAYLOAD_MAX_woUSI) || CM_ALARM_PAYLOAD_MAX_woUSI != (CM_ALARM_DATA_LENGTH_MAX - CM_ALARM_OFFSET_DATA)
#error "check this!"
#endif

	if( param->alarm_send_max_length > (CM_ALARM_DATA_LENGTH_MAX - CM_ALARM_OFFSET_DATA) ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "alarm_send_max_length(%u) out of range"
			, param->alarm_send_max_length
			);

		return LSA_FALSE;
	}

	/***/

	if (param->contains_pdev != CM_SV_CONTAINS_PDEV_NO && param->contains_pdev != CM_SV_CONTAINS_PDEV_YES) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "contains_pdev(%u) out of range"
			, param->central_pdev
		);

		return LSA_FALSE;
	}

	if( param->contains_pdev == CM_SV_CONTAINS_PDEV_YES && channel->usr.sv.pdev_device_nr != 0 ) {

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) the pdev is already in sv.pdev_device_nr(%u)"
			, param->device_nr
			, channel->usr.sv.pdev_device_nr
			);

		return LSA_FALSE;
	}

	/***/

	if (param->central_pdev != CM_SV_CENTRAL_PDEV_NO && param->central_pdev != CM_SV_CENTRAL_PDEV_YES) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "central_pdev(%u) out of range"
			, param->central_pdev
		);

		return LSA_FALSE;
	}

	/***/

	if (param->parking_mode != CM_SV_PARKING_MODE_NO && param->parking_mode != CM_SV_PARKING_MODE_YES) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "parking_mode(%u) out of range"
			, param->parking_mode
		);

		return LSA_FALSE;
	}

	/***/

	if( param->write_multiple_parallelism < 1 ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "write_multiple_parallelism(%u) < 1"
			, param->write_multiple_parallelism
			);

		return LSA_FALSE;
	}

	/***/

	if (param->SRD_enabled != CM_SV_DEVICE_SRD_ENABLED_NO && param->SRD_enabled != CM_SV_DEVICE_SRD_ENABLED_YES) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "SRD_enabled(%u) out of range"
			, param->SRD_enabled
		);

		return LSA_FALSE;
	}

	if( param->SRD_enabled == CM_SV_DEVICE_SRD_ENABLED_YES ) {

		if( param->max_ars_IOC < CM_SV_ARSET_MAX_ARS ) {

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "SRD_enabled but max_ars_IOC < CM_SV_ARSET_MAX_ARS"
				);

			return LSA_FALSE;
		}

		if( ! (param->contains_pdev == CM_SV_CONTAINS_PDEV_YES) ) {

			/* problem: the AR-set number is currently global, things work only for 1 device */
			/* problem: unclear if ARData::SRLData present or not (if R-NAP) */

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "SRD_enabled but not the pdev-device"
				);

			return LSA_FALSE;
		}

#ifndef EDD_CFG_MAX_NR_ARSETS
		{
		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "SRD_enabled but EDD_CFG_MAX_NR_ARSETS not defined"
			);
		return LSA_FALSE;
		}
#elif CM_CFG_MAX_SV_ARSETS_PER_DEVICE > EDD_CFG_MAX_NR_ARSETS
		{
		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "SRD_enabled but CM_CFG_MAX_SV_ARSETS_PER_DEVICE > EDD_CFG_MAX_NR_ARSETS"
			);
		return LSA_FALSE;
		}
#endif
	}

	if (param->CIR_enabled != CM_SV_DEVICE_CIR_ENABLED_NO && param->CIR_enabled != CM_SV_DEVICE_CIR_ENABLED_YES) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "CIR_enabled(%u) out of range"
			, param->CIR_enabled
		);

		return LSA_FALSE;
	}

	if( param->CIR_enabled == CM_SV_DEVICE_CIR_ENABLED_YES ) { /* note: CIR supported since V6.0iXX (TIA 906437) */

		if( ! (param->SRD_enabled == CM_SV_DEVICE_SRD_ENABLED_YES) ) {

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "CIR_enabled but not SRD_enabled"
				);

			return LSA_FALSE;
		}
	}

	if (param->R1_enabled != CM_SV_DEVICE_R1_ENABLED_NO && param->R1_enabled != CM_SV_DEVICE_R1_ENABLED_YES) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "R1_enabled(%u) out of range"
			, param->R1_enabled
		);

		return LSA_FALSE;
	}

	if( param->R1_enabled == CM_SV_DEVICE_R1_ENABLED_YES ) {

		if( ! (param->SRD_enabled == CM_SV_DEVICE_SRD_ENABLED_YES) ) {

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "R1_enabled but not SRD_enabled"
				);

			return LSA_FALSE;
		}

		if( param->CIR_enabled == CM_SV_DEVICE_CIR_ENABLED_YES ) {

			/* ok, CIR is supported */
		}

		if( param->parking_mode == CM_SV_PARKING_MODE_YES ) {

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "R1_enabled, parking_mode=CM_SV_PARKING_MODE_YES currently not supported"
				);

			return LSA_FALSE;
		}

		if( channel->usr.sv.enable_AlarmResponder ) {

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "R1_enabled, AlarmResponder currently not supported"
				);

			return LSA_FALSE;
		}

		if( param->R1_bridge_max < 0xFF80 ) {

			/* note: 0xFF80 is the documented(!) value for RPC settings "GSD MaxSupportedRecordSize 0xFF00" */

			return LSA_FALSE; /* note: because not tested with smaller values */
		}
		else {

			/*
			 * the maximum size of an RPC is CM_DETAIL_TYPE::rpc.alloc_len (applies to all "opnums")
			 * from this value the maximum size of a connect-PDU, the maximum size of a write-request-PDU
			 * and the maximum size of a read-response-PDU can be derived.
			 *
			 * the R1-bridge adds its header to transport the connect, write-payload and read-payload.
			 *
			 * note: the R1-bridge overhead is documented as 64 bytes!
			 */

			LSA_UINT32 conn_max = channel->detailptr->u.sv.rpc.alloc_len;
			LSA_UINT32 rdwr_max;

			if( conn_max < CLRPC_PNIO_OFFSET ) {
				return LSA_FALSE; /* unreachable */
			}

			conn_max -= CLRPC_PNIO_OFFSET; /* i.e. maximum length of a connect */

			rdwr_max = conn_max; /* i.e. maximum length of a record read/write (pnio-header + data) */

			if( rdwr_max < CM_RECORD_HEADER_SIZE ) {
				return LSA_FALSE; /* unreachable */
			}

			rdwr_max -= CM_RECORD_HEADER_SIZE; /* i.e. maximum length of a record read/write (only data) */

			/***/

			if( (CM_SV_R1BMSG_SIZE_HEADER + CM_SV_R1BMSG_SIZE_CONNECT(1 + param->gsd_NumberOfAdditionalOutputCR) + conn_max) > param->R1_bridge_max ) {
				return LSA_FALSE; /* cannot transport connect over bridge */
			}

			if( (CM_SV_R1BMSG_SIZE_HEADER + CM_SV_R1BMSG_SIZE_SUB_STATE + param->max_nr_of_subslots * CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM) > param->R1_bridge_max ) {
				return LSA_FALSE; /* cannot transport submodule-state over bridge */
			}

			if( (CM_SV_R1BMSG_SIZE_HEADER + CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE + rdwr_max) > param->R1_bridge_max ) {
				return LSA_FALSE; /* cannot transport record-read result over bridge (from B to A) */
			}

			if( (CM_SV_R1BMSG_SIZE_HEADER + CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE + rdwr_max) > param->R1_bridge_max ) {
				return LSA_FALSE; /* cannot transport record-write request over bridge (from A to B) */
			}
		}
	}

	/***/

	if( param->PDevPrmInd_enabled != CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_NO && param->PDevPrmInd_enabled != CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_YES ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "PDevPrmInd_enabled(%u) out of range"
			, param->PDevPrmInd_enabled
			);

		return LSA_FALSE;
	}

	if( param->PDevPrmInd_enabled == CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_YES ) {

		if( ! (param->contains_pdev == CM_SV_CONTAINS_PDEV_YES) || param->central_pdev == CM_SV_CENTRAL_PDEV_YES ) {

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "PDevPrmInd_enabled but device contains no pdev or is central pdev"
				);

			return LSA_FALSE;
		}

		if( param->SRD_enabled == CM_SV_DEVICE_SRD_ENABLED_YES ) {

			if( param->R1_enabled == CM_SV_DEVICE_R1_ENABLED_YES ) {

				CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "PDevPrmInd_enabled and R1_enabled is not supported"
					);

				/* see too cm_sv_opc_ar_pdevprm_rdwr() */

				return LSA_FALSE;
			}
			else {

				/* supported since V7.0i2.05 (TIA 2227906) */
			}
		}
	}

	/***/

	if( param->skip_check_100Mbps_FDX != CM_SV_DEVICER_SKIP_CHECK_100MBPS_FDX_NO && param->skip_check_100Mbps_FDX != CM_SV_DEVICER_SKIP_CHECK_100MBPS_FDX_YES ) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "skip_check_100Mbps_FDX out of range"
			);

		return LSA_FALSE;
	}

	/***/

	if( param->gsd_SharedDeviceSupported != CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_NO && param->gsd_SharedDeviceSupported != CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_YES ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "gsd_SharedDeviceSupported(%u) out of range"
			, param->gsd_SharedDeviceSupported
			);

		return LSA_FALSE;
	}

	if( param->gsd_ReportingSystemSupported != CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_NO && param->gsd_ReportingSystemSupported != CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_YES ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "gsd_ReportingSystemSupported(%u) out of range"
			, param->gsd_ReportingSystemSupported
			);

		return LSA_FALSE;
	}

	if( param->gsd_AssetManagementSupported != CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_NO && param->gsd_AssetManagementSupported != CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_YES ) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "gsd_AssetManagementSupported(%u) out of range"
			, param->gsd_AssetManagementSupported
			);

		return LSA_FALSE;
	}

	if( param->gsd_NumberOfAdditionalInputCR != 0 || param->gsd_NumberOfAdditionalOutputCR != 0 ) { /* functionality N-IOCRs not yet released, see TIA 1994542 */

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "gsd_NumberOfAdditionalInputCR(%d) or gsd_NumberOfAdditionalOutputCR(%d) is not 0"
			, param->gsd_NumberOfAdditionalInputCR
			, param->gsd_NumberOfAdditionalOutputCR
			);

		return LSA_FALSE;
	}

	if( param->gsd_NumberOfInputCR < 1 + param->gsd_NumberOfAdditionalInputCR
		|| param->gsd_NumberOfOutputCR < 1 + param->gsd_NumberOfAdditionalOutputCR
		|| param->gsd_NumberOfInputCR < param->max_ars_IOC
		|| param->gsd_NumberOfOutputCR < param->max_ars_IOC
	){
		CM_SV_TRACE_07(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "gsd_NumberOfInputCR(%d)/gsd_NumberOfOutputCR(%d) less than 1+gsd_NumberOfAdditionalInputCR(%d)/1+gsd_NumberOfAdditionalOutputCR(%d) or gsd_NumberOfInputCR(%d)/gsd_NumberOfOutputCR(%d) less than max_ars_IOC(%d)"
			, param->gsd_NumberOfInputCR, param->gsd_NumberOfOutputCR
			, param->gsd_NumberOfAdditionalInputCR, param->gsd_NumberOfAdditionalOutputCR
			, param->gsd_NumberOfInputCR, param->gsd_NumberOfOutputCR
			, param->max_ars_IOC
			);

		return LSA_FALSE;
	}

	if(param->gsd_MaxInputLength > CM_PNIO_C_SDU_MAX_LENGTH || param->gsd_MaxOutputLength > CM_PNIO_C_SDU_MAX_LENGTH) {

		CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "gsd_MaxInputLength or gsd_MaxOutputLength greater %u"
			, CM_PNIO_C_SDU_MAX_LENGTH
			);

		return LSA_FALSE;
	}

	if(param->gsd_MaxDataLength < param->gsd_MaxInputLength || param->gsd_MaxDataLength < param->gsd_MaxOutputLength) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "gsd_MaxDataLength less than gsd_MaxInputLength or gsd_MaxOutputLength"
			);

		return LSA_FALSE;
	}

	if(param->gsd_NumberOfMulticastProviderCR != 0 ||param->gsd_NumberOfMulticastConsumerCR != 0) {

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "Multicast provider or consumer CRs not supported!"
			);

		return LSA_FALSE;
	}

	/***/

	if( param->gsd_SharedDeviceSupported == CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_YES && param->max_ars_IOC < 2 ) { /* TIA 1587591 */

		CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "SharedDeviceSupported but max_ars_IOC < 2 (invalid configuration)"
			);

		return LSA_FALSE;
	}

	/***/

	if( param->gsd_ReportingSystemSupported == CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_YES ) { /* TIA 1645525 */

		if( ! cm_edd_can_time_sync_slave(channel) ) {

			/* note: EDDP implements "slave" which implies TimeASE and EDDI implements "master" without TimeASE */

			CM_SV_TRACE_00(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "ReportingSystemSupported but EDD cannot time-sync slave"
				);

			return LSA_FALSE;
		}
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== DEVICE ADD ============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_device_add(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_SV_DEVICE_ADD_PTR_TYPE param = rb->args.sv.dev_add;
	CM_SV_DEVICE_PTR_TYPE dev = LSA_NULL;
	LSA_UINT16 rsp;

	CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u)"
		, param->device_nr
		);

	/***/

	if( ! cm_sv_dev_params_ok(channel, param) ) {

		rsp = CM_ERR_PARAM;
	}
	else {

		dev = cm_sv_dev_lookup(channel, param->device_nr);

		if( cm_is_not_null(dev) ) {

			CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "dev(%u) already exists"
				, param->device_nr
				);

			rsp = CM_ERR_SEQUENCE;
		}
		else {

			dev = cm_sv_dev_alloc_and_link(channel, param);

			if( cm_is_null(dev) ) {

				rsp = CM_ERR_ALLOC_LOCAL;
			}
			else {

				CM_SV_AP_PTR_TYPE ap = cm_sv_ap_attach(dev, 0, LSA_TRUE); /* add API 0 */

				if( cm_is_null(ap) ) {

					CM_SV_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "dev(%u) cannot cm_sv_ap_attach(API 0)"
						, dev->device_nr
						);

					cm_sv_dev_unlink_and_free(dev);

					rsp = CM_ERR_ALLOC_LOCAL;
				}
				else {

					param->alarm_send_max_length = dev->alarm_send_max_length; /* update! */
					param->nr_of_event_resources = dev->res.nr_of_event_resources; /* out! */
					param->nr_of_alarm_resources = dev->res.nr_of_alarm_resources; /* out! */

					rsp = CM_OK;
				}
			}
		}
	}

	/***/

	if( rsp != CM_OK || cm_is_null(dev) ) {

		cm_callback(channel, rsp, &rb);
	}
	else {

		CM_ASSERT(dev->state == CM_SV_DEV_STATE_ADDING);

		dev->curr = rb; /* pending */

		/***/

		if( dev->explicit_ar.max_ARs_IOX != 0 ) {

			cm_sv_rpc_bind(dev, 1, dev->res.rpc_rqb); /* pre-allocate a rpc-client for every AR */
		}
		else { /* TIA 1063199, TIA 1063197 (V5.4) */

			cm_sv_rpc_register_if_obj(dev, dev->res.rpc_rqb);
		}
	}
}

/*===========================================================================*/
/*=== BIND ==================================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_bind(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CLRPC_UPPER_CL_BIND_PTR_TYPE param = rpc->args.cl.bind;

	CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-bind(%u/%u)"
		, dev->device_nr
		, ar_nr, dev->explicit_ar.max_ARs_IOX
		);

	param->client_id        = CM_RPC_INVALID_CLIENT_ID; /* sanity */
	param->host_ip          = CM_HTON32(0x7F000001); /* anything, 127.0.0.1 */
	param->udp_port         = 0; /* anything */
	param->if_uuid          = cm_uuid_nil_uuid;
	param->if_version_major = 0;
	param->if_version_minor = 0;
	param->object_uuid      = cm_uuid_nil_uuid;

	cm_rpc_request_lower(dev->channel, rpc, CLRPC_OPC_CL_BIND, dev->device_nr, ar_nr);
}

/*===========================================================================*/
/*=== BIND DONE =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_bind_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	LSA_UINT16 ar_nr,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-rsp(%u) rpc-bind(%u/%u)"
		, dev->device_nr, CM_RPC_GET_RESPONSE(rpc)
		, ar_nr, dev->explicit_ar.max_ARs_IOX
		);

	if(
		rpc != dev->res.rpc_rqb
		|| CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK
		|| rpc->args.cl.bind->client_id == CM_RPC_INVALID_CLIENT_ID
		|| ! (ar_nr >= 1 && ar_nr <= dev->explicit_ar.max_ARs_IOX)
	) {

		CM_FATAL(); /* should not happen (check CLRPC_CFG_MAX_CLIENTS) */
	}
	else {

		dev->res.clid_arr[ar_nr] = rpc->args.cl.bind->client_id; /* index = ar_nr, see CM_SV_DEVICE_TYPE */

		if( ar_nr < dev->explicit_ar.max_ARs_IOX ) { /* see cm_sv_ar_alloc() */

			ar_nr += 1;

			cm_sv_rpc_bind(dev, ar_nr, rpc);
		}
		else {

			cm_sv_rpc_register_if_obj(dev, rpc);
		}
	}
}

/*===========================================================================*/
/*=== RPC REGISTER ==========================================================*/
/*===========================================================================*/

static LSA_VOID
cm_sv_rpc_register_if_obj(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_PTR_TYPE rpc
) {
	CM_CHANNEL_PTR_TYPE channel = dev->channel;
	CLRPC_UPPER_SV_REGISTER_IF_OBJ_PTR_TYPE param = rpc->args.sv.reg_if_obj;

	CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-register"
		, dev->device_nr
		);

	param->server_id = CLRPC_INVALID_SERVER_ID; /* sanity, out-parameter */

	param->usr_id = 0; /* not used */

	cm_rpc_get_IPNIO_Device(&param->if_uuid, &param->if_version_major, &param->if_version_minor, &param->opnum_max, &param->pnio_signature);

	param->udp_port            = 0; /* AP00619205: not from channel-details */
	param->alloc_len           = channel->detailptr->u.sv.rpc.alloc_len;
	param->recv_resource_count = channel->detailptr->u.sv.rpc.recv_resource_count;
	param->send_resource_count = channel->detailptr->u.sv.rpc.send_resource_count;

	cm_rpc_make_object_uuid(&param->object_uuid, dev->vendor_id, dev->device_id, dev->instance);

	CM_MEMCPY(param->annotation, dev->curr->args.sv.dev_add->annotation, sizeof(param->annotation));
	param->annotation[sizeof(param->annotation) - 1] = 0; /* make it zero-terminated */

	cm_rpc_request_lower(channel, rpc, CLRPC_OPC_SV_REGISTER_IF_OBJ, dev->device_nr, 0);
}

/*===========================================================================*/
/*=== RPC REGISTER DONE =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_rpc_register_if_obj_done(
	CM_SV_DEVICE_PTR_TYPE dev,
	CM_RPC_LOWER_RQB_CONST_PTR_TYPE rpc
) {
	CM_CHANNEL_PTR_TYPE channel = dev->channel;

	CM_SV_TRACE_03(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) rpc-rsp(%u) server_id(%u)"
		, dev->device_nr
		, CM_RPC_GET_RESPONSE(rpc), rpc->args.sv.reg_if_obj->server_id
		);

	if(
		rpc != dev->res.rpc_rqb
		|| CM_RPC_GET_RESPONSE(rpc) != CLRPC_OK
		|| rpc->args.sv.reg_if_obj->server_id == CLRPC_INVALID_SERVER_ID
	) {

		CM_FATAL(); /* should not happen (check CLRPC_CFG_MAX_SERVERS) */
	}
	else {

		CM_ASSERT(dev->state == CM_SV_DEV_STATE_ADDING);

		dev->res.rpc_server_id = rpc->args.sv.reg_if_obj->server_id;

		dev->state = CM_SV_DEV_STATE_ADDED;

		/***/

		if( CM_SV_DEV_IS_PDEV_DEVICE(dev) ) {

			/* note: must do this in state ADDED (not earlier). CM-PD may signal changes only in (the consistent) state ADDED. */

			LSA_UINT16 port_id;

			CM_ASSERT(dev->nr_of_pdev_mappings >= 2); /* interface and at least 1 port */

			for (port_id = 0; port_id < dev->nr_of_pdev_mappings; port_id++) {

				LSA_UINT16 slot_nr;
				LSA_UINT16 subslot_nr;
				LSA_UINT32 mod_ident;
				LSA_UINT32 sub_ident;
				LSA_UINT8  im0_bits;
				LSA_UINT16 pdev_properties;

				LSA_BOOL port_exists = cm_pdsv_port_from_id(channel, port_id, &slot_nr, &subslot_nr, &mod_ident, &sub_ident, &im0_bits, &pdev_properties);

				if( !port_exists || !CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, subslot_nr) ) {

					CM_FATAL(); /* a bug */
				}

				if( CM_PDEV_STATE_IS_PLUGGED(pdev_properties) ) {

					CM_SV_SUBMODULE_ADD_ELEMENT_TYPE elem;

					elem.api        = 0;
					elem.slot_nr    = slot_nr;
					elem.subslot_nr = subslot_nr;
					elem.mod_ident  = mod_ident;
					elem.sub_ident  = sub_ident;
					elem.im0_bits   = im0_bits;

					elem.response = cm_sv_submodule_add_elem(dev, &elem, LSA_FALSE);

					if(elem.response != CM_OK) {

						/* refactor:
						 * - to avoid out-of-mem here, vivification has to be done early (compare to API 0)
						 * - pulled ports form a new special case
						 * - all parameters must pass the checks in cm_sv_submodule_params_ok()
						 */

						CM_FATAL();
					}
				}
			}
		}

		/***/

		while( ! CmListIsEmpty(&dev->res.rpc_callQ) ) {

			CM_RPC_LOWER_RQB_PTR_TYPE rpc_call_ind;

			CmListRemoveHead(&dev->res.rpc_callQ, rpc_call_ind, CM_RPC_LOWER_RQB_PTR_TYPE);

			rpc_call_ind->args.sv.call->server_id = dev->res.rpc_server_id;
			rpc_call_ind->args.sv.call->alloc_len = 0;
			rpc_call_ind->args.sv.call->ndr_data = LSA_NULL;

			cm_rpc_request_lower(channel, rpc_call_ind, CLRPC_OPC_SV_CALL_IND_PROVIDE, dev->device_nr, 0);
		}

		/***/

		cm_callback(channel, CM_OK, &dev->curr);
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
