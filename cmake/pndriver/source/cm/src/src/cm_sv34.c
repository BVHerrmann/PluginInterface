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
/*  F i l e               &F: cm_sv34.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Diagnosis things                                              */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7340
#define CM_MODULE_ID       7340

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

#if CM_SV_DIAG_TYPE_CHANNEL != 0
#error "check this! used as index."
#elif CM_SV_DIAG_TYPE_EXT_CHANNEL != 1
#error "check this! used as index."
#elif CM_SV_DIAG_TYPE_QUALIFIED != 2
#error "check this! used as index."
#elif CM_SV_DIAG_TYPE_GENERIC != 3
#error "check this! used as index."
#elif CM_SV_DIAG_TYPE_MAX != 4
#error "check this! used as index."
#endif

static LSA_BOOL
cm_sv_diag_key_equal(
	LSA_UINT16 diag_type,
	CM_SV_DIAG_PTR_TYPE diag,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
);

/*===========================================================================*/
/*=== DIAG GET SUBSLOT ======================================================*/
/*===========================================================================*/

/* note that cm_sv_submodule_remove_elem() cannot reuse this function */

static CM_SV_SUBSLOT_PTR_TYPE
cm_sv_diag_get_subslot(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	LSA_UINT32 api,
	LSA_UINT16 slot_nr,
	LSA_UINT16 subslot_nr
) {
	CM_SV_AP_PTR_TYPE ap;
	CM_SV_SLOT_PTR_TYPE sl;
	CM_SV_SUBSLOT_PTR_TYPE su;

	ap = cm_sv_ap_lookup(dev, api);

	sl = cm_is_null(ap) ? LSA_NULL : cm_sv_slot_lookup(ap, slot_nr);

	su = cm_is_null(sl) ? LSA_NULL : cm_sv_subslot_lookup(sl, subslot_nr);

	if( cm_is_null(su) || ! CM_SV_SUBSLOT_IS_PLUGGED(su) ) {

		CM_SV_TRACE_04(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ap/sl/su(%u/%u/%u) is not plugged"
			, dev->device_nr, api, slot_nr, subslot_nr
			);

		return LSA_NULL;
	}

	return su;
}

/*===========================================================================*/
/*=== DIAG PARAMETER CHECK ==================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_diag_params_ok(
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
) {

	if( elem->diag_tag == 0/*all*/ ) {

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "diag_tag(%u) not allowed, is wildcard in diag-remove"
			, elem->diag_tag
			);

		return LSA_FALSE;
	}

	/***/

	if( elem->diag_type >= CM_SV_DIAG_TYPE_MAX ) {

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "diag_type(%u) out of range"
			, elem->diag_type
			);

		return LSA_FALSE;
	}

	/***/

	if( elem->channel_number > 0x8000 ) {

		/* 0-0x7FFF..manufacturer specific, 0x8000..submodule, rest reserved */

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "channel_number(%u) out-of-range", elem->channel_number
			);

		return LSA_FALSE;
	}

	/***/

	if( (elem->channel_properties & CM_CHPR_ACCU_MASK) == CM_CHPR_ACCU_YES ) {

		if( elem->channel_number == 0x8000 ) {

			/* PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01545793 item 8 */
			/* Only ChannelProperties.Accumulative == 0 shall be used in conjunction with ChannelNumber == 0x8000 */

			CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%04x) channel_number(%u), invalid combination"
				, elem->channel_properties, elem->channel_number
				);

			return LSA_FALSE;
		}
	}

	/*
	 * the maintenance-bits were re-interpreted starting with AP00267821.
	 * checks changed with V5.2i4.16, AP01467031
	 * checks changed with PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01531008, AP01529856, and AP01545793 item 10
	 *
	 * see PNIO-Spec
	 * - table "Valid combinations within ChannelProperties" and
	 * - table "Valid combinations for Alarmnotification and RecordDataRead(DiagnosisData)"
	 */

	if( (elem->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_QUA ) { /* severity is in QualifiedChannelQualifier */

		if( elem->diag_type == CM_SV_DIAG_TYPE_QUALIFIED ) {

			LSA_UINT32 qua = elem->u.qua.qualified_channel_qualifier;

			/* note: QualifiedChannelQualifier is related to MaintenanceStatus, see cm_sv_diag_maint_qual() */

			if(
				(qua & ~CM_MAINTENANCE_STATUS_QUA_BITS) != 0 /* non-qualifier bits must be zero */
				|| (qua & CM_MAINTENANCE_STATUS_QUA_BITS) == 0 /* a qualifier bit must be set */
				|| (qua & (qua - 1)) != 0 /* only one bit (2^x check see OpenBSD, param.h) */
			) {

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
					, "qualified_channel_qualifier(0x%08x), invalid value"
					, qua
					);

				return LSA_FALSE;
			}
		}
		else {

			CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%04x) diag_type(%u), invalid combination"
				, elem->channel_properties, elem->diag_type
				);

			return LSA_FALSE;
		}
	}
	else { /* failure, m-req, m-dem */

		if( elem->diag_type == CM_SV_DIAG_TYPE_QUALIFIED ) {

			if( elem->u.qua.qualified_channel_qualifier != 0 ) {

				CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
					, "channel_properties(0x%04x) diag_type(%u), qualified_channel_qualifier not zero"
					, elem->channel_properties, elem->diag_type
					);

				return LSA_FALSE;
			}
		}
	}

	if( elem->diag_type == CM_SV_DIAG_TYPE_QUALIFIED ) { /* TIA 1024519 (V6.0i1.29), TIA 1024518 (V5.4i1.7), TIA 1009638 (V5.3i3.7) */

		/* note: check here to avoid "unreachable code" warnings in code above */

		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "qualified diagnosis is currently not supported (refer to TIA 1009638)"
			);

		return LSA_FALSE;
	}

	/***/

	switch( (elem->channel_properties & CM_CHPR_SPEC_MASK) ) {

	case CM_CHPR_SPEC_APP:
		break; /* appearing is ok for all types */

	case CM_CHPR_SPEC_DIS:
		if( (elem->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) { /* diagnosis */

			/* disappearing ... shall only be used in conjunction with ManufacturerSpecificDiagnosis */

			if( elem->diag_type == CM_SV_DIAG_TYPE_GENERIC ) {

				break; /* ok for generic-diag */
			}
			else {

				CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
					, "channel_properties(0x%04x), disappears but not generic-diag"
					, elem->channel_properties
					);
			}
		}
		else { /* maintenance or qualified */

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%04x), disappears but is maintenance or qualified"
				, elem->channel_properties
				);
		}
		return LSA_FALSE;

	default:
		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "channel_properties(0x%04x), neither appears nor disappears"
			, elem->channel_properties
			);
		return LSA_FALSE;
	}

	/***/

	if( elem->diag_type == CM_SV_DIAG_TYPE_GENERIC ) {

		if( elem->u.gen.info_tag > 0x7FFF ) {

			/* 0-0x7FFF..manufacturer specific, rest other usage */

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "info_tag(%u) out-of-range", elem->u.gen.info_tag
				);

			return LSA_FALSE;
		}

		if( elem->u.gen.info_length > (CM_ALARM_PAYLOAD_MAX_woUSI - CM_ALARM_MAINTENANCEITEM_LENGTH) ) { /* note: must fit into a diagnosis-alarm */

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "info_length(%u) out-of-range", elem->u.gen.info_length
				);

			return LSA_FALSE;
		}

		if( elem->channel_number != 0x8000 ) { /* TIA 1114464 - this implies CM_CHPR_ACCU_NO (channel_properties.accumulative shall be set to zero) */

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "channel_number(%u) invalid: expected 0x8000", elem->channel_number
				);

			return LSA_FALSE;
		}

		if((elem->channel_properties & CM_CHPR_TYPE_MASK) != CM_CHPR_TYPE_OTHER) { /* TIA 1114464: channel_properties.type shall be set to zero */

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%x) invalid: type shall be set to zero", elem->channel_properties
				);

			return LSA_FALSE;
		}

		if((elem->channel_properties & CM_CHPR_DIR_MASK) != CM_CHPR_DIR_OTHER) { /* TIA 1114464: channel_properties.direction shall be set to zero */

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%x) invalid: direction shall be set to zero", elem->channel_properties
				);

			return LSA_FALSE;
		}

		if((elem->channel_properties & CM_CHPR_MAINT_MASK) != CM_CHPR_MAINT_ERR) { /* TIA 1114464: channel_properties.maintainance shall be set to fault */

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%x) invalid: maintenance shall be set to 'fault'", elem->channel_properties
				);

			return LSA_FALSE;
		}

		if( CM_SV_SUBSLOT_IS_PDEV(elem->subslot_nr) ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "subslot(0x%x), manufacturer specific diagnosis not supported for PDEV submodules", elem->subslot_nr
				);

			return LSA_FALSE;
		}
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== DIAG APPEND TAG =======================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_diag_append(
	CM_SV_SUBSLOT_PTR_TYPE su,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
) {
	LSA_UINT32 alloc_length;
	CM_SV_DIAG_PTR_TYPE diag;

	/*
	 * ChannelDiagnosisData ::=
	 *   ChannelNumber, ChannelProperties,
	 *   ChannelErrorType
	 *
	 * ExtChannelDiagnosisData ::=
	 *   ChannelNumber, ChannelProperties,
	 *   ChannelErrorType, ExtChannelErrorType, ExtChannelAddValue
	 *
	 * QualifiedChannelDiagnosisData ::=
	 *   ChannelNumber, ChannelProperties,
	 *   ChannelErrorType, ExtChannelErrorType, ExtChannelAddValue, QualifiedChannelQualifier
	 *
	 * ManufacturerSpecificDiagnosis ::=
	 *   ...,
	 *   ChannelNumber, ChannelProperties,
	 *   UserStructureIdentifier, Data*
	 */

	switch( elem->diag_type ) {

	case CM_SV_DIAG_TYPE_CHANNEL:
		alloc_length = sizeof(*diag);
		break;

	case CM_SV_DIAG_TYPE_EXT_CHANNEL:
		alloc_length = sizeof(*diag) + 2+4;
		break;

	case CM_SV_DIAG_TYPE_QUALIFIED:
		alloc_length = sizeof(*diag) + 2+4+4;
		break;

	case CM_SV_DIAG_TYPE_GENERIC:
		alloc_length = sizeof(*diag) + elem->u.gen.info_length;
		break;

	default:
		CM_FATAL(); /* a bug */
		return LSA_FALSE;
	}

	/***/

	if( alloc_length > 0xFFFF ) {

		diag = LSA_NULL; /* treat as alloc-failure */
	}
	else {

		CM_LOCAL_MEM_PTR_TYPE local_mem;

		CM_ALLOC_LOCAL_MEM(&local_mem, alloc_length);
		diag = (CM_SV_DIAG_PTR_TYPE)local_mem;
	}

	if( cm_is_null(diag) ) {

		CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "cannot alloc-local, size(%u)", alloc_length
			);

		return LSA_FALSE;
	}

	/***/

	diag->r1_mirrored = LSA_FALSE;

	diag->diag_tag = elem->diag_tag;

	diag->channel_number     = elem->channel_number;
	diag->channel_properties = elem->channel_properties;

	{
	LSA_VOID_PTR_TYPE body = (LSA_VOID_PTR_TYPE)(diag + 1);

	switch( elem->diag_type ) {

	case CM_SV_DIAG_TYPE_CHANNEL:
		diag->channel_error_type_usi = elem->u.ch.channel_error_type;
		diag->data_length = 0;
		break;

	case CM_SV_DIAG_TYPE_EXT_CHANNEL:
		diag->channel_error_type_usi = elem->u.ext.channel_error_type;
		diag->data_length = 2+4;
		CM_PUT16_HTON(body, 0, elem->u.ext.ext_channel_error_type);
		CM_PUT32_HTON(body, 2, elem->u.ext.ext_channel_add_value);
		break;

	case CM_SV_DIAG_TYPE_QUALIFIED:
		diag->channel_error_type_usi = elem->u.qua.channel_error_type;
		diag->data_length = 2+4+4;
		CM_PUT16_HTON(body, 0, elem->u.qua.ext_channel_error_type);
		CM_PUT32_HTON(body, 2, elem->u.qua.ext_channel_add_value);
		CM_PUT32_HTON(body, 6, elem->u.qua.qualified_channel_qualifier);
		break;

	case CM_SV_DIAG_TYPE_GENERIC:
		diag->channel_error_type_usi = elem->u.gen.info_tag; /* UserStructureIdentifier */
		diag->data_length = elem->u.gen.info_length;
		CM_MEMCPY(body, elem->u.gen.info_data, elem->u.gen.info_length);
		break;

	default:
		CM_FATAL(); /* a bug */
		break;
	}
	}

	/***/

	CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW
		, "adding diag_tag(%u), diag_type(%u)"
		, diag->diag_tag, elem->diag_type
		);

	CmListInsertTail(&su->diag_list[elem->diag_type], &diag->link);

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== DIAG DELETE TAG =======================================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_diag_delete(
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT32 diag_tag, /* 0 = all */
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
) {
	LSA_BOOL found = LSA_FALSE; /* not found or all deleted */
	LSA_UINT16 diag_type;

	for( diag_type = 0; diag_type < CM_SV_DIAG_TYPE_MAX; ++diag_type ) {

		CM_LIST_ENTRY_PTR_TYPE diag_list = &su->diag_list[diag_type];
		CM_SV_DIAG_PTR_TYPE diag;

		for( CmListEach(diag, diag_list, CM_SV_DIAG_PTR_TYPE) ) {

			if(
				diag_tag == 0/*all*/
				|| ( cm_is_null(elem) ? diag_tag == diag->diag_tag : cm_sv_diag_key_equal(diag_type, diag, elem) )
			) {

				CM_SV_DIAG_PTR_TYPE temp = diag; /* found */

				diag = CmListPrevOrHead(&diag->link, CM_SV_DIAG_PTR_TYPE);

				CmListRemoveEntry(&temp->link);

				/***/

				CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW
					, "deleting diag_tag(%u), diag_type(%u)"
					, temp->diag_tag, diag_type
					);

				{
				LSA_UINT16 ret_val;
				CM_FREE_LOCAL_MEM(&ret_val, temp);
				CM_ASSERT(ret_val == LSA_RET_OK);
				LSA_UNUSED_ARG(ret_val);
				}

				/***/

				if( diag_tag != 0/*all*/ ) {

					found = LSA_TRUE; /* found and deleted (there is only one with a tag != 0) */
					break;
				}
			}
		}

		if( found ) {

			break;
		}
	}

	/***/

	return found;
}

/*===========================================================================*/
/*=== DIAG ADD ONE ELEMENT ==================================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_diag_add_elem(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
) {
	CM_SV_SUBSLOT_PTR_TYPE su;
	LSA_UINT16 rsp;

	CM_SV_TRACE_06(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ap/sl/su(%u/%u/%u) diag_tag(%u) diag_type(%u)"
		, dev->device_nr
		, elem->api, elem->slot_nr, elem->subslot_nr
		, elem->diag_tag, elem->diag_type
		);

	su = cm_sv_diag_get_subslot(dev, elem->api, elem->slot_nr, elem->subslot_nr);

	if( cm_is_null(su) ) {

		rsp = CM_ERR_SEQUENCE;
	}
	else if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) {

		rsp = CM_ERR_PARAM; /* use CM_OPC_PD_DIAG_ADD */
	}
	else if( ! cm_sv_diag_params_ok(elem) ) {

		/* note: a diagnosis with the same diag_tag could exist; we do not remove it because the replacement is invalid */

		rsp = CM_ERR_PARAM;
	}
	else { /* plugged and parameters ok */

		if( cm_sv_diag_delete(su, elem->diag_tag, LSA_NULL) ) {

			rsp = CM_OK_REPLACED;
		}
		else {

			rsp = CM_OK;
		}

		/* note: although mixing ADD and ADD2 is documented as "must not" we have to support the combination of ADD(generic-diag with diag-tag != 1) and ADD2(channel-diag) */

		if( ! cm_sv_diag_append(su, elem) ) {

			rsp = CM_ERR_ALLOC_LOCAL;
		}
	}

	/***/

	return rsp;
}

/*===========================================================================*/
/*=== DIAG REMOVE ONE ELEMENT ===============================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_diag_remove_elem(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_DIAG_REMOVE_ELEMENT_CONST_PTR_TYPE elem
) {
	CM_SV_SUBSLOT_PTR_TYPE su;
	LSA_UINT16 rsp;

	CM_SV_TRACE_05(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ap/sl/su(%u/%u/%u) diag_tag(%u)"
		, dev->device_nr
		, elem->api, elem->slot_nr, elem->subslot_nr
		, elem->diag_tag
		);

	su = cm_sv_diag_get_subslot(dev, elem->api, elem->slot_nr, elem->subslot_nr);

	if( cm_is_null(su) ) {

		rsp = CM_ERR_SEQUENCE;
	}
	else if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) ) {

		rsp = CM_ERR_PARAM; /* use CM_OPC_PD_DIAG_REMOVE */
	}
	else { /* plugged */

		if( ! cm_sv_diag_delete(su, elem->diag_tag, LSA_NULL) ) { /* not found or all deleted */

			if( elem->diag_tag != 0/*all*/ ) {

				CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "diag_tag(%u) not found", elem->diag_tag
					);

				rsp = CM_ERR_SEQUENCE;
			}
			else {

				rsp = CM_OK;
			}
		}
		else {

			rsp = CM_OK;
		}
	}

	/***/

	return rsp;
}

/*===========================================================================*/
/*=== DIAG, MAINTENANCE AND QUALIFIED =======================================*/
/*===========================================================================*/

LSA_BOOL
cm_sv_diag_maint_qual(
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_SV_AP_CONST_PTR_TYPE ap,
	CM_SV_SLOT_CONST_PTR_TYPE sl,
	CM_SV_SUBSLOT_PTR_TYPE su,
	LSA_UINT8 *cha_ptr, /* optional, usecase: AlarmSpecifier.ChannelDiagnosis */
	LSA_UINT8 *gen_ptr, /* optional, usecase: AlarmSpecifier.ManufacturerSpecificDiagnosis */
	LSA_UINT32 *mnt_ptr  /* optional */
) {
	LSA_BOOL sub = LSA_FALSE;
	LSA_BOOL cha = LSA_FALSE;
	LSA_BOOL gen = LSA_FALSE;
	LSA_UINT32 mnt = 0;

	/***/

	if( CM_SV_SUBSLOT_IS_PLUGGED(su) ) { /* note: pulled subslots do not have diagnoses */

		/*
		 * from the PNIO-Spec:
		 *
		 * AlarmSpecifier.SubmoduleDiagnosisState
		 *   At least one DiagnosisData
		 *   with ChannelProperties.Maintenance (=diagnosis) and ChannelProperties.Specifier (=appear)
		 *
		 * AlarmSpecifier.ChannelDiagnosis
		 *   means that the SubslotNumber contains at least one
		 *   ChannelDiagnosis, ExtChannelDiagnosis and QualifiedChannelDiagnosis
		 *   with ChannelProperties.Maintenance (=diagnosis) and ChannelProperties.Specifier (=appear)
		 *
		 * AlarmSpecifier.ManufacturerSpecificDiagnosis
		 *   means that the SubslotNumber contains at least one
		 *   ManufacturerSpecificDiagnosis
		 *   with ChannelProperties.Maintenance (=diagnosis) and ChannelProperties.Specifier (=appear or disappear)
		 */

		LSA_UINT16 diag_index;

		for( diag_index = 0; diag_index < CM_SV_DIAG_TYPE_MAX; ++diag_index ) {

			CM_LIST_ENTRY_PTR_TYPE diag_list = &su->diag_list[diag_index];
			CM_SV_DIAG_PTR_TYPE diag;

			for( CmListEach(diag, diag_list, CM_SV_DIAG_PTR_TYPE) ) {

				if( (diag->channel_properties & CM_CHPR_SPEC_MASK) == CM_CHPR_SPEC_APP ) {

					switch( (diag->channel_properties & CM_CHPR_MAINT_MASK) ) {

					case CM_CHPR_MAINT_ERR:

						sub = LSA_TRUE;

						if( diag_index == CM_SV_DIAG_TYPE_GENERIC ) {

							gen = LSA_TRUE;
						}
						else {

							cha = LSA_TRUE;
						}
						break;

					case CM_CHPR_MAINT_REQ:
						mnt |= CM_MAINTENANCE_STATUS_REQUIRED;
						break;

					case CM_CHPR_MAINT_DEM:
						mnt |= CM_MAINTENANCE_STATUS_DEMANDED;
						break;

					case CM_CHPR_MAINT_QUA:
						mnt |= CM_GET32_NTOH((diag + 1), 6); /* QualifiedChannelQualifier, see cm_sv_diag_append() */
						break;

					default:
						CM_FATAL(); /* a bug */
						break;
					}
				}
				else { /* disappearing */

					if( (diag->channel_properties & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) { /* actually always true */

						if( diag_index == CM_SV_DIAG_TYPE_GENERIC ) { /* actually always true */

							gen = LSA_TRUE;
						}
					}
				}
			}
		}

		/***/

		if( ap->api == 0 && CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, su->subslot_nr) ) {

			LSA_UINT32 temp_mnt;

			if( cm_pdsv_diag_maint_qual(dev->channel, sl->slot_nr, su->subslot_nr, &temp_mnt) ) {

				/* pdev-diagnoses are of type ExtChannelDiagnosis */

				sub = LSA_TRUE;

				cha = LSA_TRUE;
			}

			mnt |= temp_mnt;
		}
	}

	/***/

	CM_SV_TRACE_08(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ap/sl/su(%u/%u/%u) diag(%u) cha(%u) gen(%u) maint(0x%08x)"
		, dev->device_nr, ap->api, sl->slot_nr, su->subslot_nr
		, sub, cha, gen, mnt
		);

	/***/

	if( cm_is_not_null(cha_ptr) ) {

		*cha_ptr = (cha ? CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO);
	}

	if( cm_is_not_null(gen_ptr) ) {

		*gen_ptr = (gen ? CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES : CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO);
	}

	if( cm_is_not_null(mnt_ptr) ) {

		*mnt_ptr = mnt;
	}

	return sub;
}

/*===========================================================================*/
/*=== DIAG KEY EQUAL ========================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_diag_key_equal( /* AP01467031, see document in RQ-attachment */
	LSA_UINT16 diag_type,
	CM_SV_DIAG_PTR_TYPE diag,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem
) {
	/* AP01531008 and AP01529856 update AP01467031 (definition of "key")
	 *
	 * Key = Location + Diagnosis
	 * Location = (API, SlotNumber, SubslotNumber, ChannelNumber, Accumulative, Direction)
	 * Diagnosis = (ChannelErrorType/ExtChannelErrorType rsp. USI)
	 */

	if( diag->channel_number != elem->channel_number ) {
		return LSA_FALSE;
	}

	/* not a key: CM_CHPR_TYPE_MASK */

	if( (diag->channel_properties & CM_CHPR_ACCU_MASK) != (elem->channel_properties & CM_CHPR_ACCU_MASK) ) {
		return LSA_FALSE;
	}

	/* not a key: CM_CHPR_MAINT_MASK */

	/* not a key: CM_CHPR_SPEC_MASK */

	if( (diag->channel_properties & CM_CHPR_DIR_MASK) != (elem->channel_properties & CM_CHPR_DIR_MASK) ) {
		return LSA_FALSE;
	}

	if( diag_type == CM_SV_DIAG_TYPE_GENERIC || elem->diag_type == CM_SV_DIAG_TYPE_GENERIC ) {

		if( diag_type != elem->diag_type ) {
			return LSA_FALSE;
		}

		if( diag->channel_error_type_usi != elem->u.gen.info_tag ) {
			return LSA_FALSE;
		}
	}
	else { /* neither is generic */

		/* AP01531008:
		 * A channel-diag is a ext-channel-diag with ExtChannelErrorType 0 (= "unspecified")
		 * A qualified-diag is a ext-channel-diag with more severities
		 * ExtChannelAddValue is not a key
		 * QualifiedChannelQualifier encodes the severity (thus not a key)
		 */

		LSA_UINT16 diag_ext_channel_error_type;

		if( diag_type == CM_SV_DIAG_TYPE_CHANNEL ) {
			diag_ext_channel_error_type = 0;
		}
		else {
			LSA_VOID_PTR_TYPE body = (LSA_VOID_PTR_TYPE)(diag + 1);
			diag_ext_channel_error_type = CM_GET16_NTOH(body, 0);
		}

		switch( elem->diag_type ) {

		case CM_SV_DIAG_TYPE_CHANNEL:
			if( diag->channel_error_type_usi != elem->u.ch.channel_error_type ) {
				return LSA_FALSE;
			}
			if( diag_ext_channel_error_type != 0 ) {
				return LSA_FALSE;
			}
			break;

		case CM_SV_DIAG_TYPE_EXT_CHANNEL:
			if( diag->channel_error_type_usi != elem->u.ext.channel_error_type ) {
				return LSA_FALSE;
			}
			if( diag_ext_channel_error_type != elem->u.ext.ext_channel_error_type ) {
				return LSA_FALSE;
			}
			break;

		case CM_SV_DIAG_TYPE_QUALIFIED:
			if( diag->channel_error_type_usi != elem->u.qua.channel_error_type ) {
				return LSA_FALSE;
			}
			if( diag_ext_channel_error_type != elem->u.qua.ext_channel_error_type ) {
				return LSA_FALSE;
			}
			break;

		default:
			return LSA_FALSE; /* unreachable */
		}
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== DIAG CHANGE ONE ELEMENT ===============================================*/
/*===========================================================================*/

LSA_UINT16
cm_sv_diag_change_elem( /* AP01467031 */
	CM_SV_DEVICE_CONST_PTR_TYPE dev,
	CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE elem,
	LSA_BOOL is_add,
	LSA_BOOL is_user
) {
	CM_SV_SUBSLOT_PTR_TYPE su;
	LSA_UINT16 rsp;

	CM_SV_TRACE_07(dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ap/sl/su(%u/%u/%u) channel_number(%u) channel_properties(0x%04x) diag_type(%u)"
		, dev->device_nr
		, elem->api, elem->slot_nr, elem->subslot_nr
		, elem->channel_number, elem->channel_properties, elem->diag_type
		);

	su = cm_sv_diag_get_subslot(dev, elem->api, elem->slot_nr, elem->subslot_nr);

	if( cm_is_null(su) ) {

		rsp = CM_ERR_SEQUENCE;
	}
	else if( CM_SV_SUBSLOT_IS_PDEV(su->subslot_nr) && is_user ) {

		CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "subslot(0x%x), diag add is not allowed for a PDev submodule"
			, elem->subslot_nr
			);

		rsp = CM_ERR_PARAM; /* use CM_OPC_PD_DIAG_ADD */
	}
	else if( is_add && (elem->diag_tag != 1 || ! cm_sv_diag_params_ok(elem)) ) {

		/* note: a diagnosis with the same key could exist; we do not remove it because the replacement is invalid */

		rsp = CM_ERR_PARAM;
	}
	else { /* plugged and parameters ok */

		if( is_add ) {

			if( cm_sv_diag_delete(su, elem->diag_tag, elem) ) {

				rsp = CM_OK_REPLACED;
			}
			else {

				rsp = CM_OK;
			}

			/* note: although mixing ADD and ADD2 is documented as "must not" we have to support the combination of ADD(generic-diag with diag-tag != 1) and ADD2(channel-diag) */

			if( ! cm_sv_diag_append(su, elem) ) {

				rsp = CM_ERR_ALLOC_LOCAL;
			}
		}
		else { /* remove */

			if( ! cm_sv_diag_delete(su, elem->diag_tag, elem) ) { /* not found or all deleted */

				if( elem->diag_tag != 0/*all*/ ) {

					CM_SV_TRACE_01(dev->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "diag_tag(%u) not found", elem->diag_tag
						);

					rsp = CM_ERR_SEQUENCE;
				}
				else {

					rsp = CM_OK;
				}
			}
			else {

				rsp = CM_OK;
			}
		}
	}

	/***/

	return rsp;
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
