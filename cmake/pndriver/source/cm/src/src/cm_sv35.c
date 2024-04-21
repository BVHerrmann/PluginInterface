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
/*  F i l e               &F: cm_sv35.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-Server, Alarm things                                                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7350
#define CM_MODULE_ID       7350

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*===========================================================================*/
#if CM_CFG_MAX_SERVERS /* or else exclude from compilation */
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_check_prop(
	LSA_UINT16 prop,
	LSA_UINT16 alarm_type
) {
	/*
	 * the maintenance-bits were re-interpreted starting with AP00267821.
	 * see table "Valid combinations within ChannelProperties" in the PNIO-Spec!
	 */

	switch( (prop & CM_CHPR_SPEC_MASK) ) {

	case CM_CHPR_SPEC_APP:

		if( alarm_type == CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS ) {

			CM_SV_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%x) do not match with alarm_type(%u)"
				, prop, alarm_type
				);
			return LSA_FALSE; /* channel_properties do not match with alarm_type */
		}
		break;

	case CM_CHPR_SPEC_DIS:
	case CM_CHPR_SPEC_DBO:
		break;

	default: /* CM_CHPR_SPEC_RSVD */

		if( (prop & CM_CHPR_MAINT_MASK) == CM_CHPR_MAINT_ERR ) {

			/*
			 * All subsequent a) Diagnosis, MaintenanceRequired, MaintenanceDemanded,
			 * and QualifiedDiagnosis disappear
			 *
			 * a) Subsequent means, all ExtChannelErrorTypes for the delivered ChannelErrorType and
			 * also the ChannelErrorType itself disappears. Shall be used in AlarmNotification only.
			 */
		}
		else {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "channel_properties(0x%x), wrong specifier (reserved)"
				, prop
				);
			return LSA_FALSE; /* wrong specifier (reserved) */
		}
		break;
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_check_USI8000(
	LSA_UINT16 alarm_type,
	CM_COMMON_MEM_U8_PTR_TYPE dat,
	LSA_UINT16 len
) {
	LSA_UINT16 ofs;

	/* ChannelDiagnosis: UserStructureIdentifier(=0x8000), ChannelDiagnosisData* */

	ofs = 0;

	while( ofs < len ) {

		LSA_UINT16 prop;

		/*
		 * ChannelDiagnosisData ::=
		 *   ChannelNumber, ChannelProperties, ChannelErrorType
		 */

		if( ofs + (2+2+2) > len ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "too short for another ChannelDiagnosisData"
				);
			return LSA_FALSE; /* incomplete */
		}

		prop = CM_GET16_NTOH(dat, ofs + 2);

		ofs += (2+2+2);

		if( ! cm_sv_ar_alarm_check_prop(prop, alarm_type) ) {
			return LSA_FALSE; /* check failed */
		}
	}

	if( ofs != len ) {
		CM_FATAL(); /* a bug */
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_check_USI8002(
	LSA_UINT16 alarm_type,
	CM_COMMON_MEM_U8_PTR_TYPE dat,
	LSA_UINT16 len
) {
	LSA_UINT16 ofs;

	/* ExtChannelDiagnosisData: UserStructureIdentifier(=0x8002), ExtChannelDiagnosisData* */

	ofs = 0;

	while( ofs < len ) {

		LSA_UINT16 prop;

		/*
		 * ExtChannelDiagnosisData ::=
		 *   ChannelNumber, ChannelProperties, ChannelErrorType,
		 *   ExtChannelErrorType, ExtChannelAddValue
		 */

		if( ofs + (2+2+2 + 2+4) > len ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "too short for another ExtChannelDiagnosisData"
				);
			return LSA_FALSE; /* incomplete */
		}

		prop = CM_GET16_NTOH(dat, ofs + 2);

		ofs += (2+2+2 + 2+4);

		if( ! cm_sv_ar_alarm_check_prop(prop, alarm_type) ) {
			return LSA_FALSE; /* check failed */
		}
	}

	if( ofs != len ) {
		CM_FATAL(); /* a bug */
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_check_USI8003(
	LSA_UINT16 alarm_type,
	CM_COMMON_MEM_U8_PTR_TYPE dat,
	LSA_UINT16 len
) {
	LSA_UINT16 ofs;

	/* QualifiedChannelDiagnosisData: UserStructureIdentifier(=0x8003), QualifiedChannelDiagnosisData* */

	ofs = 0;

	while( ofs < len ) {

		LSA_UINT16 prop;
		LSA_UINT32 qual;

		/*
		 * QualifiedChannelDiagnosisData ::=
		 *   ChannelNumber, ChannelProperties, ChannelErrorType,
		 *   ExtChannelErrorType, ExtChannelAddValue, QualifiedChannelQualifier
		 */

		if( ofs + (2+2+2 + 2+4+4) > len ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "too short for another QualifiedChannelDiagnosisData"
				);
			return LSA_FALSE; /* incomplete */
		}

		prop = CM_GET16_NTOH(dat, ofs + 2);
		qual = CM_GET32_NTOH(dat, ofs + 2+2+2+2+4);

		ofs += (2+2+2 + 2+4+4);

		if( ! cm_sv_ar_alarm_check_prop(prop, alarm_type) ) {
			return LSA_FALSE; /* check failed */
		}

		{
		LSA_UINT16 bit;
		LSA_UINT16 cnt = 0;

		/* There shall only one bit be set in one QualifiedChannelDiagnosis */

		for( bit = 0; bit < 32; ++bit ) {

			if( (qual & (1UL << bit)) != 0 ) {

				cnt += 1;

				if( bit < 2 ) { /* bits 0 and 1 are reserved (must be zero) */

					CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
						, "qualified_channel_qualifier(0x%x), reserved bits are not zero"
						, qual
						);
					return LSA_FALSE; /* reserved bits are not zero */
				}
			}
		}

		if( cnt != 1 ) {

			CM_SV_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
				, "qualified_channel_qualifier(0x%x), more than one bit set"
				, qual
				);
			return LSA_FALSE; /* more than one bit set */
		}
		}
	}

	if( ofs != len ) {
		CM_FATAL(); /* a bug */
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_check_diagblock(
	CM_UPPER_ALARM_CONST_PTR_TYPE param,
	CM_COMMON_MEM_U8_PTR_TYPE blkdat,
	LSA_UINT16 blklen /* length of whole block */
) {
	LSA_UINT16 blktyp;
	LSA_UINT16 blkver;
	LSA_UINT16 ofs;

	/*
	 * DiagnosisData with BlockVersionLow=0 ::=
	 *   BlockHeader, ChannelDiagnosis ^ ManufacturerSpecificDiagnosis ^ ExtChannelDiagnosis
	 *
	 * DiagnosisData with BlockVersionLow=1 ::=
	 *   BlockHeader, API, ChannelDiagnosis ^ ManufacturerSpecificDiagnosis ^ ExtChannelDiagnosis
	 *
	 */

	if( blklen < (2+2+1+1) ) {
		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "wrong structure"
			);
		return LSA_FALSE; /* wrong structure */
	}

	blktyp = CM_GET16_NTOH(blkdat, 0);

	if( blktyp != CM_BLOCK_TYPE_DIAGNOSIS ) {
		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "wrong block-type"
			);
		return LSA_FALSE; /* wrong block-type */
	}

	blkver = CM_GET16_NTOH(blkdat, 4);

	ofs = (2+2+1+1);

	switch( blkver ) {

	case 0x0100:
		if( param->api != 0 ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "block-version inconsistent with api"
				);
			return LSA_FALSE; /* block-version inconsistent with api */
		}
		break;

	case 0x0101:
		if( ofs + 4 > blklen ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "block-length too short"
				);
			return LSA_FALSE; /* block-length too short */
		}
		if( param->api != CM_GET32_NTOH(blkdat, ofs) ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "block-version inconsistent with api"
				);
			return LSA_FALSE; /* block-version inconsistent with api */
		}
		ofs += 4;
		break;

	default:
		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "wrong block-version"
			);
		return LSA_FALSE; /* wrong block-version */
	}

	{
	LSA_UINT16 numb, prop, usi;

	if( ofs + (2+2 + 2+2 + 2) > blklen ) {
		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "block-length too short"
			);
		return LSA_FALSE; /* block-length too short */
	}

	if( param->slot_nr != CM_GET16_NTOH(blkdat, ofs + 0) ) {
		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "slot-nr inconsistent"
			);
		return LSA_FALSE; /* slot-nr inconsistent */
	}
	if( param->subslot_nr != CM_GET16_NTOH(blkdat, ofs + 2) ) {
		CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
			, "subslot-nr inconsistent"
			);
		return LSA_FALSE; /* subslot-nr inconsistent */
	}

	numb = CM_GET16_NTOH(blkdat, ofs + 4);
	prop = CM_GET16_NTOH(blkdat, ofs + 6);
	usi  = CM_GET16_NTOH(blkdat, ofs + 8);

	ofs += (2+2 + 2+2 + 2);

	blkdat += ofs;
	blklen -= ofs;

	switch( usi ) {

	case 0x8000:

		if( numb != 0x8000 ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "channel-number not 'submodule'"
				);
			return LSA_FALSE; /* channel-number not "submodule" */
		}

		if( ! cm_sv_ar_alarm_check_prop(prop, param->alarm_type) ) {
			return LSA_FALSE; /* check failed */
		}

		if( ! cm_sv_ar_alarm_check_USI8000(param->alarm_type, blkdat, blklen) ) {
			return LSA_FALSE; /* check failed */
		}
		break;

	case 0x8002:

		if( numb != 0x8000 ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "channel-number not 'submodule'"
				);
			return LSA_FALSE; /* channel-number not "submodule" */
		}

		if( ! cm_sv_ar_alarm_check_prop(prop, param->alarm_type) ) {
			return LSA_FALSE; /* check failed */
		}

		if( ! cm_sv_ar_alarm_check_USI8002(param->alarm_type, blkdat, blklen) ) {
			return LSA_FALSE; /* check failed */
		}
		break;

	case 0x8003:

		if( numb != 0x8000 ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "channel-number not 'submodule'"
				);
			return LSA_FALSE; /* channel-number not "submodule" */
		}

		if( ! cm_sv_ar_alarm_check_prop(prop, param->alarm_type) ) {
			return LSA_FALSE; /* check failed */
		}

		if( ! cm_sv_ar_alarm_check_USI8003(param->alarm_type, blkdat, blklen) ) {
			return LSA_FALSE; /* check failed */
		}
		break;

	default:

		if( usi > 0x7FFF ) {
			CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
				, "usi neither 'user' nor 'well-known'"
				);
			return LSA_FALSE; /* usi neither "user" nor "well-known" */
		}
		break;
	}
	}

	return LSA_TRUE;
}

/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_send_format_ok(
	CM_UPPER_ALARM_CONST_PTR_TYPE param
) {
	CM_COMMON_MEM_U8_PTR_TYPE dat;
	LSA_UINT16 len;

	dat = param->alarm_data;
	len = param->alarm_data_length;

	if( len == 0 ) {
		return LSA_TRUE; /* nothing to check, USI is don't care! */
	}

	/*
	 * NOTE: don't forget to consider M-Consumer diagnosis when
	 *       adding checks against param->diag_xxx_avail!
	 */

	switch( param->alarm_tag ) {

	case 0x8000: /* ChannelDiagnosis: UserStructureIdentifier(=0x8000), ChannelDiagnosisData* */

		if( ! cm_sv_ar_alarm_check_USI8000(param->alarm_type, dat, len) ) {
			return LSA_FALSE; /* check failed */
		}
		break;

	case 0x8001: /* Multiple: UserStructureIdentifier(=0x8001), DiagnosisData* */

		{
		LSA_UINT16 ofs = 0;

		while( ofs < len ) {

			CM_COMMON_MEM_U8_PTR_TYPE blkdat;
			LSA_UINT16 blklen;

			/*
			 * DiagnosisData is a Block
			 */

			blkdat = dat + ofs;

			if( ofs + (2+2) > len ) {
				CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
					, "too short for DiagnosisData"
					);
				return LSA_FALSE; /* incomplete */
			}

			blklen = CM_GET16_NTOH(blkdat, 2);

			if( ofs + (2+2+blklen) > len ) {
				CM_SV_TRACE_00(0, LSA_TRACE_LEVEL_ERROR
					, "wrong structure"
					);
				return LSA_FALSE; /* wrong structure */
			}

			blklen += 2+2; /* length of whole block */

			ofs += blklen;

			if( ! cm_sv_ar_alarm_check_diagblock(param, blkdat, blklen) ) {
				return LSA_FALSE; /* check failed */
			}
		}

		if( ofs != len ) {
			CM_FATAL(); /* a bug */
		}
		}
		break;

	case 0x8002: /* ExtChannelDiagnosisData: UserStructureIdentifier(=0x8002), ExtChannelDiagnosisData* */

		if( ! cm_sv_ar_alarm_check_USI8002(param->alarm_type, dat, len) ) {
			return LSA_FALSE; /* check failed */
		}
		break;

	case 0x8003: /* QualifiedChannelDiagnosisData: UserStructureIdentifier(=0x8003), QualifiedChannelDiagnosisData* */

		if( ! cm_sv_ar_alarm_check_USI8003(param->alarm_type, dat, len) ) {
			return LSA_FALSE; /* check failed */
		}
		break;

	default:
		/* format undefined, cannot check */
		break;
	}

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== ALARM SEND PARAMETER CHECK ============================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_send_params_ok(
	CM_UPPER_ALARM_CONST_PTR_TYPE param,
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {

	if( cm_alarm_type_is_internal(param->alarm_type) ) {

		/* this is an error, with the exception of ... */

		if( param->alarm_type == CM_ALARM_TYPE_RETURN_OF_SUBMODULE ) {

			/* return-of-submodule is part of the CM_ALARM_PLUG_GROUP but it is initiated by the user */
		}
		else {

			CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "alarm_type(%u) is not a user-alarm"
				, param->alarm_type
				);

			return LSA_FALSE;
		}
	}

	/*
	 * 0x0000 - 0x7FFF	ManufacturerSpecific
	 * 0x8000			ChannelDiagnosis
	 * 0x8001			Multiple
	 * 0x8002			ExtChannelDiagnosis
	 * 0x8003			QualifiedChannelDiagnosis
	 * 0x8004 - 0x80FF	Reserved
	 * 0x8100			Maintenance
	 * 0x8101 - 0x81FF	Reserved
	 * 0x8200			Upload&Retrieval
	 * 0x8201			iParameter
	 * 0x8202 - 0x82FF	Reserved
	 * 0x8300 - 0x8303	Reporting System ... not checked here - see cm_sv_ar_alarm_send_RS_AlarmItem_ok()
	 * 0x9000 - 0x9FFF	Reserved for profiles
	 * 0xA000 - 0xFFFF	Reserved
	 */

	if( ! (
		param->alarm_tag <= 0x8003
		||
		(param->alarm_tag >= 0x8200 && param->alarm_tag <= 0x8201)
		||
		(param->alarm_tag >= 0x9000 && param->alarm_tag <= 0x9FFF)
	) ) {

		CM_SV_TRACE_04(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "ap/sl/su(%u/%u/%u) alarm tag(%u) out-of-range"
			, param->api, param->slot_nr, param->subslot_nr
			, param->alarm_tag
			);

		return LSA_FALSE;
	}

	/***/

	if( param->alarm_data_length != 0 && cm_is_null(param->alarm_data) ) {

		CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "ap/sl/su(%u/%u/%u) alarm_data_length(%u) but no alarm_data(0x%x)"
			, param->api, param->slot_nr, param->subslot_nr
			, param->alarm_data_length, param->alarm_data
			);

		return LSA_FALSE;
	}

	if( param->alarm_data_length > cm_sv_ar_almi_send_max_length(argr, param->alarm_type) ) {

		CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "ap/sl/su(%u/%u/%u) alarm_data_length(%u) > almi_send_max_length(%u)"
			, param->api, param->slot_nr, param->subslot_nr
			, param->alarm_data_length, cm_sv_ar_almi_send_max_length(argr, param->alarm_type)
			);

		return LSA_FALSE;
	}

	/***/

	return LSA_TRUE;
}

/*===========================================================================*/
/*=== ALARM SEND RS_AlarmItem CHECK =========================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_ar_alarm_send_RS_AlarmItem_ok( /* Reporting System (TIA 1645641) */
	CM_UPPER_ALARM_CONST_PTR_TYPE param,
	CM_AR_GRAPH_CONST_PTR_TYPE argr
) {
	LSA_BOOL ok;

	if( ! (param->api == 0 && param->slot_nr == 0x8000 && param->subslot_nr == 0) ) {

		ok = LSA_FALSE; /* not the "alarm transport container" */
	}
	else if( cm_is_null(argr->rs_info) ) {

		ok = LSA_FALSE; /* no RSInfoBlock */
	}
	else if( param->alarm_type != CM_ALARM_TYPE_STATUS ) {

		ok = LSA_FALSE;
	}
	else if( ! (param->alarm_tag >= 0x8300 && param->alarm_tag <= 0x8302) ) {

		ok = LSA_FALSE;
	}
	else if( param->alarm_data_length > cm_sv_ar_almi_send_max_length(argr, param->alarm_type) ) {

		ok = LSA_FALSE;
	}
	else if(
		param->alarm_data_length != 2
		|| cm_is_null(param->alarm_data)
		|| CM_GET16_NTOH(param->alarm_data, 0) != 0
	) {

		ok = LSA_FALSE; /* RS_AlarmInfo not zero */
	}
	else {

		ok = LSA_TRUE;
	}

	/***/

	if( ! ok ) {

		CM_SV_TRACE_09(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "ap/sl/su(%u/%u/%u) rs_info(0x%08x) alarm_type(%u) alarm_tag(0x%04x) alarm_data_length(%u) alarm_data(0x%02x 0x%02x)"
			, param->api, param->slot_nr, param->subslot_nr
			, argr->rs_info
			, param->alarm_type, param->alarm_tag, param->alarm_data_length
			, ((cm_is_not_null(param->alarm_data) && param->alarm_data_length >= 1) ? param->alarm_data[0] : 0xCC)
			, ((cm_is_not_null(param->alarm_data) && param->alarm_data_length >= 2) ? param->alarm_data[1] : 0xCC)
			);
	}

	return ok;
}

/*===========================================================================*/
/*=== ALARM SEND ============================================================*/
/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_alarm_send(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
) {
	CM_UPPER_ALARM_PTR_TYPE param = rb->args.sv.ar_alarm_send;
	CM_AR_GRAPH_PTR_TYPE argr;
	LSA_UINT16 rsp;

	/***/

	argr = cm_sv_ar_lookup3(channel, param->device_nr, param->session_key, CM_SV_AR_CNFL_EXP_USER_REQ);

	if( cm_is_null(argr) ) {

		CM_SV_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) sk(0x%x) could not cm_sv_ar_lookup3"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			, param->session_key
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else if( cm_is_null(argr->alarm_cr) ) {

		CM_SV_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev/ar(%u/%u) no alarm-cr"
			, param->device_nr, CM_SV_SESSION_KEY_TO_AR_NR(param->session_key)
			);

		rsp = CM_ERR_SEQUENCE;
	}
	else {

		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
		CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

		CM_SV_TRACE_03(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "dev/ar(%u/%u) alarm-type(%u)"
			, argr->device_nr, argr->ar_nr
			, param->alarm_type
			);

		if( param->slot_nr == 0x8000 ) { /* "alarm transport container" for Reporting System (TIA 1645641) */

			ar_ap  = LSA_NULL;
			ar_mod = LSA_NULL;
			ar_sub = LSA_NULL;

			if( ! cm_sv_ar_alarm_send_RS_AlarmItem_ok(param, argr) ) {

				rsp = CM_ERR_PARAM;
			}
			else {

				rsp = CM_OK;
			}
		}
		else {

			cm_ar_graph_find_api_slot_subslot(argr, param->api, param->slot_nr, param->subslot_nr, &ar_ap, &ar_mod, &ar_sub);

			if( cm_is_null(ar_sub) ) {

				CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) not in AR"
					, argr->device_nr, argr->ar_nr
					, param->api, param->slot_nr, param->subslot_nr
					);

				rsp = CM_ERR_PARAM;
			}
			else if( ! cm_sv_ar_alarm_send_params_ok(param, argr) ) {

				rsp = CM_ERR_PARAM;
			}
			else if( ! cm_sv_ar_alarm_send_format_ok(param) ) {

				CM_SV_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "dev/ar(%u/%u) alarm_data not conforming to PNIO-Spec"
					, argr->device_nr, argr->ar_nr
					);

				rsp = CM_ERR_FORMAT;
			}
			else {

				rsp = CM_OK;
			}
		}

		/***/

		if( rsp == CM_OK ) {

			LSA_BOOL do_led_info = LSA_FALSE;

			rsp = cm_sv_ar_almi_alarm_request(argr, rb, ar_ap, ar_mod, ar_sub, &do_led_info); /* checks the submodule state */

			if( rsp == CM_OK ) {

				/* pending, see cm_sv_ar_almi_alarm_confirmation() */
			}

			if( do_led_info ) {

				cm_sv_dev_led_info_eval(argr->cm.sv.dev); /* trigger: reset A.R.P. flag (return-of-submodule alarm) */
			}
		}
	}

	/***/

	if( rsp != CM_OK ) {

		cm_callback(channel, rsp, &rb);
	}
}

/*===========================================================================*/
/*=== PDEV ALARM-INDICATION =================================================*/
/*===========================================================================*/

static LSA_BOOL
cm_sv_alarm_pdev_group(
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE alarm
) {
	CM_SV_DEVICE_PTR_TYPE dev = cm_sv_dev_get_added(channel, channel->usr.sv.pdev_device_nr);

	if( cm_is_null(dev) ) {

		return LSA_FALSE; /* case: AR established, remove device, AR aborts, PDEV alarm before relinquish (TIA 990247) */
	}
	else if( dev->pdev_owner_ar_nr == 0 ) {

		return LSA_FALSE; /* late */
	}
	else {

		CM_AR_GRAPH_PTR_TYPE argr = cm_sv_ar_lookup2(dev, dev->pdev_owner_ar_nr);

		if( cm_is_null(argr) ) {

			CM_FATAL(); /* a bug */
		}
		else {

			CM_UPPER_ALARM_PTR_TYPE param = alarm->args.sv.ar_alarm_send;

			param->device_nr   = argr->device_nr;
			param->session_key = argr->session_key;
			param->ar_nr       = CM_SV_SESSION_KEY_TO_AR_NR(param->session_key); /* as documented */

#if CM_DEBUG
			CM_ASSERT(cm_sv_ar_alarm_send_format_ok(param));
#endif

			if( param->api != 0 || !CM_SV_SUBSLOT_IS_PDEV_LOCAL(dev, param->subslot_nr) ) {
				CM_FATAL(); /* a bug */
			}

			if( cm_ar_com_is_down(argr) ) {

				return LSA_FALSE; /* late */
			}
			else {

				CM_AR_GRAPH_AP_PTR_TYPE ar_ap;
				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				cm_ar_graph_find_api_slot_subslot(argr, param->api, param->slot_nr, param->subslot_nr, &ar_ap, &ar_mod, &ar_sub);

				if( cm_is_not_null(ar_sub) ) { /* always true */

					if( cm_is_not_null(argr->cm.sv.arset) && argr->cm.sv.r1_flags != 0 ) { //R1 marker (pdev alarm)

						CM_SV_ARSET_PTR_TYPE arset = argr->cm.sv.arset;

						cm_sv_arset_check_primary(argr);

						/***/

						if(
							cm_is_not_null(arset->r1b.pd_alarm.orig)
							|| arset->r1b.pd_alarm.orig_is_completed
						) {

							CM_FATAL();
						}
						else if( arset->is_aborting ) {

							return LSA_FALSE;
						}
						else if( cm_is_not_null(arset->first) ) {

							return LSA_FALSE; /* no alarms in "first AR" */
						}
						else if( !CM_SV_SUBSLOT_IS_PLUGGED(ar_sub->sv.real_su) ) {

							return LSA_FALSE; /* late */
						}
						else if( (ar_sub->sv.sm_flags & CM_SV_AR_SMFL_IS_PRMED) == 0 ) {

							return LSA_FALSE; /* not-prmed */
						}
						else {

							arset->r1b.pd_alarm.orig = alarm; /* park */
							arset->r1b.pd_alarm.orig_is_completed = LSA_FALSE;

							/***/

							if( dev->r1b.bridge_state == CM_SV_DEV_R1B_STATE_ESTABLISHED ) {

								cm_sv_r1b_pdev_alarm_mirror_set(dev, alarm);
							}

							/***/

							if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_ORIGINAL) != 0 ) {

								if( argr->cm.sv.arset->primary != argr ) {

									return LSA_TRUE; /* pending (other head may send it) */
								}
							}

							if( (argr->cm.sv.r1_flags & CM_SV_AR_R1FL_REPRESENTATIVE) != 0 ) {

								return LSA_TRUE; /* pending (other head may send it) */
							}
						}
					}

					/***/

					if( cm_sv_ar_almi_alarm_request(argr, alarm, ar_ap, ar_mod, ar_sub, LSA_NULL) != CM_OK ) { /* PDEV group alarm */

						CM_SV_TRACE_05(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "dev/ar(%u/%u) ap/sl/su(%u/%u/%u) could not send pdev-alarm"
							, argr->device_nr, argr->ar_nr
							, param->api, param->slot_nr, param->subslot_nr
							);

						if( argr->cm.sv.r1_flags != 0 ) {

							//R1 VV: split alarm request into two functions (can-send and do-send) and replace R1 checks above with can-send
							CM_FATAL(); /* a bug (not reached, see checks above) */
						}

						return LSA_FALSE; /* AR down (or a bug) */
					}
					else {

						return LSA_TRUE; /* pending */
					}
				}
			}
		}
	}

	/***/

	return LSA_FALSE; /* actually unreachable */
}

/*===========================================================================*/

LSA_VOID
cm_sv_opc_ar_alarm_pdev_group_ind(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE alarm
) {
	if( cm_sv_alarm_pdev_group(channel, alarm) ) {

		/* OK, alarm queued, response: see callers of cm_sv_pdev_callback() */
	}
	else { /* late */

		cm_sv_pdev_callback(channel, alarm, CM_OK_CANCELLED);
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
