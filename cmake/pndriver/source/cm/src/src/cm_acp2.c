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
/*  F i l e               &F: cm_acp2.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  CM-AR-ACP                                                                */
/*  - frame add                                                              */
/*  - frame control                                                          */
/*  - frame remove                                                           */
/*  - frame get apdu status                                                  */
/*  - alarm add                                                              */
/*  - alarm remove                                                           */
/*  - done-callbacks                                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  21
#define CM_MODULE_ID       21

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*===========================================================================*/
/*=== FRAME ADD =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	LSA_BOOL  is_adv, is_sysred;

	CM_ASSERT(! CM_ACPID_ISVALID(io_cr));

	/***/

	is_adv = (io_cr->cm.backlink->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) ? LSA_TRUE : LSA_FALSE;
	is_sysred = (io_cr->cm.backlink->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED) ? LSA_TRUE : LSA_FALSE;

	/***/

	CM_ACP_TRACE_06(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "FrameAdd: frame_id(0x%04x) is_cons(%u) is_multi(%u) is_adv(%u) is_sysred(%u) len(%u)"
		, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast, is_adv, is_sysred
		, io_cr->data_length
		);

	CM_ACP_TRACE_07(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  frame_id(0x%04x) mac(%02x-%02x-%02x-%02x-%02x-%02x)"
		, io_cr->frame_id
		, io_cr->multicast_mac.mac[0], io_cr->multicast_mac.mac[1], io_cr->multicast_mac.mac[2]
		, io_cr->multicast_mac.mac[3], io_cr->multicast_mac.mac[4], io_cr->multicast_mac.mac[5]
		);

	CM_ACP_TRACE_08(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  frame_id(0x%04x) send_clock/rr/phase %u/%u/%u adaption to %u/%u/%u, seq(%u)"
		, io_cr->frame_id
		, io_cr->send_clock, io_cr->reduction_ratio, io_cr->phase
		, io_cr->cm.adpt.send_clock, io_cr->cm.adpt.reduction_ratio, io_cr->cm.adpt.phase
		, io_cr->cm.adpt.sequence
		);

	if (io_cr->cm.is_consumer)
	{
		EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE  cons_add = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE);

		cons_add->FrameID = io_cr->frame_id;

		if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1_UDP )
		{
			cons_add->Properties = EDD_CSRT_CONS_PROP_RTCLASS_UDP;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3 )
		{
			if( is_adv )
			{
				cons_add->Properties = EDD_CSRT_CONS_PROP_RTCLASS_3;
			}
			else
			{
				cons_add->Properties = EDD_CSRT_CONS_PROP_RTCLASS_3 | EDD_CSRT_CONS_PROP_RTC3_MODE_LEGACY;
			}
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_2 )
		{
			cons_add->Properties = EDD_CSRT_CONS_PROP_RTCLASS_2;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1 )
		{
			cons_add->Properties = EDD_CSRT_CONS_PROP_RTCLASS_1;
		}
		else
		{
			CM_FATAL1 (io_cr->iocr_properties);
			return;
		}

		if (is_sysred)
		{
			cons_add->Properties |= EDD_CSRT_CONS_PROP_SYSRED;
		}

		if (io_cr->cm.subframe_data != 0) /* is-dfp */
		{
			cons_add->Properties			|= EDD_CSRT_CONS_PROP_PDU_FORMAT_DFP;
			cons_add->DataLen				= ((io_cr->cm.subframe_data >> 8) & 0xFF);
			cons_add->Partial_DataLen		= 0;
			cons_add->Partial_DataOffset	= 0;
		}
		else if( io_cr->cm.partial_length == io_cr->data_length  &&  io_cr->cm.partial_offset == 0 )
		{
			cons_add->DataLen				= io_cr->data_length;
			cons_add->Partial_DataLen		= 0;
			cons_add->Partial_DataOffset	= 0;
		}
		else
		{
			cons_add->DataLen				= io_cr->data_length;
			cons_add->Partial_DataLen		= io_cr->cm.partial_length;
			cons_add->Partial_DataOffset	= io_cr->cm.partial_offset;
		}

		cons_add->DataHoldFactor			= io_cr->cm.adpt.data_hold_factor;

		cons_add->UserID					= (LSA_UINT32)(io_cr);

		/* fixed cons iodata, same in add and control */
		cons_add->IOParams.AppGroup				= 0;
		cons_add->IOParams.ClearOnMISS			= EDD_CONS_CLEAR_ON_MISS_DISABLE;

		/* set cons iodata with control */
		cons_add->CycleReductionRatio			= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		cons_add->CyclePhase					= EDD_CYCLE_PHASE_UNDEFINED;
		cons_add->IOParams.BufferProperties		= EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED;
		cons_add->IOParams.DataOffset			= EDD_DATAOFFSET_UNDEFINED;
		cons_add->IOParams.CRNumber				= EDD_CRNUMBER_UNDEFINED;
		cons_add->IOParams.ForwardMode			= EDD_FORWARDMODE_UNDEFINED;
		cons_add->IOParams.CutThrough_OutDataStart = 0;
		cons_add->IOParams.SFPosition			= EDD_SFPOSITION_UNDEFINED;
		cons_add->IOParams.SFOffset				= EDD_SFOFFSET_UNDEFINED;

		/* init cnf */
		cons_add->ConsumerID					= EDD_CONS_ID_INVALID; /* out */
		cons_add->IOParams.APDUStatusOffset		= EDD_DATAOFFSET_INVALID; /* out */

		/***/

		cm_edd_request_lower(channel, EDD_SRV_SRT_CONSUMER_ADD, io_cr, edd);
	}
	else
	{
		EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE prov_add = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE);

		prov_add->FrameID = io_cr->frame_id;

		if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1_UDP )
		{
			prov_add->Properties = EDD_CSRT_PROV_PROP_RTCLASS_UDP;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3 )
		{
			if( is_adv )
			{
				prov_add->Properties = EDD_CSRT_PROV_PROP_RTCLASS_3;
			}
			else
			{
				prov_add->Properties = EDD_CSRT_PROV_PROP_RTCLASS_3 | EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY;
			}
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_2 )
		{
			prov_add->Properties = EDD_CSRT_PROV_PROP_RTCLASS_2;
		}
		else if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1 )
		{
			prov_add->Properties = EDD_CSRT_PROV_PROP_RTCLASS_1;
		}
		else
		{
			CM_FATAL1 (io_cr->iocr_properties);
			return;
		}

		if (is_sysred)
		{
			prov_add->Properties |= EDD_CSRT_PROV_PROP_SYSRED;
		}

		if (io_cr->cm.subframe_data != 0) /* is-dfp */
		{
			prov_add->Properties		|= EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP;
			prov_add->DataLen			= ((io_cr->cm.subframe_data >> 8) & 0xFF);
		}
		else
		{
			prov_add->DataLen			= io_cr->cm.partial_length; /* not: data_length */
		}

		if (io_cr->cm.is_multicast)
		{
			if ((prov_add->Properties & EDD_CSRT_PROV_PROP_PDU_FORMAT_DFP) == 0
			&&  (prov_add->Properties & EDD_CSRT_PROV_PROP_RTC3_MODE_LEGACY) == 0)
			{
				prov_add->Properties	|= EDD_CSRT_PROV_PROP_MODE_MC_CR;
			}
		}

		prov_add->UserID				= (LSA_UINT32)(io_cr);

		switch(channel->path_type) {
		case CM_PATH_TYPE_SERVER:
			prov_add->GroupID = CM_PROVIDER_GROUP_IOD;
			break;
		case CM_PATH_TYPE_CLIENT:
			prov_add->GroupID = CM_PROVIDER_GROUP_IOC;
			break;
		case CM_PATH_TYPE_MULTICAST:
			prov_add->GroupID = CM_PROVIDER_GROUP_IOM;
			break;
		default:
			CM_FATAL(); /* a bug */
			break;
		}

		/* fixed prov iodata, same in add and control */
		prov_add->IOParams.AppGroup		= 0;

		/* set prov iodata with control */
		prov_add->CycleReductionRatio	= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		prov_add->CyclePhase			= EDD_CYCLE_PHASE_UNDEFINED;
		prov_add->CyclePhaseSequence	= EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
		prov_add->IOParams.BufferProperties	= EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED | EDD_PROV_BUFFER_PROP_HERA_MEM_TYPE_UNDEFINED;
		prov_add->IOParams.DataOffset	= EDD_DATAOFFSET_UNDEFINED;
		prov_add->IOParams.CRNumber		= EDD_CRNUMBER_UNDEFINED;
		prov_add->IOParams.SFPosition	= EDD_SFPOSITION_UNDEFINED;
		prov_add->IOParams.SFOffset		= EDD_SFOFFSET_UNDEFINED;

		/* init cnf */
		prov_add->ProviderID				= EDD_PROV_ID_INVALID; /* out */
		prov_add->IOParams.CWOffset			= EDD_DATAOFFSET_INVALID; /* out */
		prov_add->IOParams.DataStatusOffset	= EDD_DATAOFFSET_INVALID; /* out */

		/***/

		cm_edd_request_lower(channel, EDD_SRV_SRT_PROVIDER_ADD, io_cr, edd);
	}
}

/*===========================================================================*/
/*=== FRAME ADD DONE ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_add_done (
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr = (CM_AR_GRAPH_IO_CR_PTR_TYPE)user_id;
	CM_AR_GRAPH_PTR_TYPE  argr = io_cr->cm.backlink;

	CM_ASSERT(io_cr->cm.cr_state == CM_AR_CR_PASSIVE);

	/***/

	if (CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK) {

		CM_ACP_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "FrameAddDone: frame_id(0x%04x) add failed, edd-rsp(%u)"
			, io_cr->frame_id, CM_EDD_GET_RESPONSE(edd)
			);

		/*
		 * note: iocr-add can fail because of things CM cannot check:
		 *       wrong edd-data, phase-overload, sequence-not-unique, ...
		 */

		CM_ACPID_SETINVALID(io_cr); /* not added */

		io_cr->cm.cr_state = CM_AR_CR_ERROR; /* sanity */
	}
	else { /* add succeeded */

		if (io_cr->cm.is_consumer) {
			EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE cons_add = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_CONSUMER_ADD_PTR_TYPE);

			io_cr->cm.acp_id        = cons_add->ConsumerID;

			io_cr->cm.edd_as_offset = cons_add->IOParams.APDUStatusOffset;

			io_cr->cm.edd_cw_offset = EDD_DATAOFFSET_INVALID;
			io_cr->cm.edd_ds_offset	= EDD_DATAOFFSET_INVALID;
		}
		else {
			EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE prov_add = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_ADD_PTR_TYPE);

			io_cr->cm.acp_id        = prov_add->ProviderID;

			io_cr->cm.edd_as_offset = EDD_DATAOFFSET_INVALID;

			io_cr->cm.edd_cw_offset = prov_add->IOParams.CWOffset;
			io_cr->cm.edd_ds_offset	= prov_add->IOParams.DataStatusOffset;
		}
	}

	CM_ACP_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "FrameAddDone: frame_id(0x%04x) is_cons(%u) is_multi(%u) acp_id(%u) cr_state(%u)"
		, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast
		, io_cr->cm.acp_id, io_cr->cm.cr_state
		);

	/***/

	cm_arcq_step_next (argr, io_cr, LSA_NULL, edd);

}

/*===========================================================================*/
/*=== FRAME CONTROL =========================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_control(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_UINT16  acp_mode,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	LSA_BOOL  is_adv  = (io_cr->cm.backlink->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) ? LSA_TRUE : LSA_FALSE;
	LSA_BOOL  is_rtc3 = ((io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3) ? LSA_TRUE : LSA_FALSE;

	/***/

	if (io_cr->cm.is_consumer)
	{
		EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE cons_ctrl = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE);

		CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ConsumerControl.req: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) acp_mode(0x%x)"
			, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, acp_mode
			);

		cons_ctrl->ConsumerID	= io_cr->cm.acp_id;
		cons_ctrl->Mode			= acp_mode;

		/* fixed cons iodata, same in add and control */
		cons_ctrl->IOParams.AppGroup				= 0;
		cons_ctrl->IOParams.ClearOnMISS				= EDD_CONS_CLEAR_ON_MISS_DISABLE;

		/* set cons iodata with control */
		cons_ctrl->CycleReductionRatio				= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		cons_ctrl->CyclePhase						= EDD_CYCLE_PHASE_UNDEFINED;
		cons_ctrl->IOParams.BufferProperties		= EDD_CONS_BUFFER_PROP_IRTE_IMG_UNDEFINED;
		cons_ctrl->IOParams.DataOffset				= EDD_DATAOFFSET_UNDEFINED;
		cons_ctrl->IOParams.CRNumber				= EDD_CRNUMBER_UNDEFINED;
		cons_ctrl->IOParams.ForwardMode				= EDD_FORWARDMODE_UNDEFINED;
		cons_ctrl->IOParams.CutThrough_OutDataStart = 0;
		cons_ctrl->IOParams.SFPosition				= EDD_SFPOSITION_UNDEFINED;
		cons_ctrl->IOParams.SFOffset				= EDD_SFOFFSET_UNDEFINED;

		if (acp_mode & EDD_CONS_CONTROL_MODE_ACTIVATE)
		{
			/* set with control */
			cons_ctrl->CycleReductionRatio				= io_cr->cm.adpt.reduction_ratio;
			cons_ctrl->CyclePhase						= io_cr->cm.adpt.phase;
			cons_ctrl->IOParams.BufferProperties		= io_cr->cm.edd_properties;
			cons_ctrl->IOParams.DataOffset				= io_cr->cm.edd_data_offset;
			cons_ctrl->IOParams.CRNumber				= io_cr->cm.edd_cr_number;
			cons_ctrl->IOParams.ForwardMode				= io_cr->cm.edd_forward_mode;
			cons_ctrl->IOParams.CutThrough_OutDataStart	= io_cr->cm.edd_ct_outdatastart;

			if (io_cr->cm.subframe_data != 0) /* is-dfp */
			{
				cons_ctrl->IOParams.SFPosition			= io_cr->cm.subframe_data & 0x7F;
				cons_ctrl->IOParams.SFOffset			= io_cr->cm.subframe_offset;
			}

			/* set src-ip-addr */
			if( (io_cr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1_UDP )
			{
				CM_MEMSET (cons_ctrl->SrcMAC.MacAdr, 0, sizeof(cons_ctrl->SrcMAC.MacAdr));
				CM_MEMSET (cons_ctrl->SrcIP.IPAdr,  0, sizeof(cons_ctrl->SrcIP.IPAdr));

				CM_FATAL(); /* UDP not yet supported (AP01195665) , see cm_ar_graph_insert_io_cr() */
			}
			/* or src-mac-addr */
			else
			{
				CM_MAC_PTR_TYPE  mac;
				if( io_cr->cm.is_multicast )
				{
#if CM_CFG_MAX_MULTICAST_CHANNELS
					mac = &io_cr->cm.backlink->ar.eth_mac; /* see cm_mc_mcons_resolve_done() */
#else
					mac = LSA_NULL; /* warning C4701: potentially uninitialized local variable 'mac' used */
					CM_FATAL();
#endif
				}
				else
				{
					mac = &io_cr->cm.backlink->ar.eth_mac;
				}
				CM_MEMCPY (cons_ctrl->SrcMAC.MacAdr, mac->mac, sizeof(cons_ctrl->SrcMAC.MacAdr));
				CM_MEMSET (cons_ctrl->SrcIP.IPAdr,  0, sizeof(cons_ctrl->SrcIP.IPAdr));
			}
		}

		cm_edd_request_lower(channel, EDD_SRV_SRT_CONSUMER_CONTROL, io_cr, edd);
	}
	else
	{
		EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE prov_ctrl = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE);
		CM_MAC_PTR_TYPE  mac;

		if( io_cr->cm.is_multicast )
		{
			mac = &io_cr->multicast_mac;
		}
		else
		{
			if (is_rtc3)
			{
				if (is_adv)
				{
					mac = &io_cr->multicast_mac;
				}
				else
				{
					mac = &io_cr->cm.backlink->ar.eth_mac;
				}
			}
			else
			{
				mac = &io_cr->cm.backlink->ar.eth_mac;
			}
		}

		CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ProviderControl.req: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) acp_mode(0x%x)"
			, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, acp_mode
			);

		prov_ctrl->ProviderID	= io_cr->cm.acp_id;
		prov_ctrl->Mode			= acp_mode;

		if (acp_mode & EDD_PROV_CONTROL_MODE_AUTOSTOP)
		{
			if (acp_mode & EDD_PROV_CONTROL_MODE_ACTIVATE)
			{
				prov_ctrl->AutoStopConsumerID = io_cr->cm.backlink->ar_com.autostop_cons_id;
			}
			else if (acp_mode & EDD_PROV_CONTROL_MODE_PASSIVATE)
			{
				prov_ctrl->AutoStopConsumerID = EDD_CONSUMER_ID_REMOVE_AS;
			}
			else
			{
				CM_FATAL();
			}
		}
		else
		{
			prov_ctrl->AutoStopConsumerID = EDD_CONS_ID_INVALID;
		}

		prov_ctrl->FrameID		= io_cr->frame_id;
		CM_MEMCPY (prov_ctrl->DstMAC.MacAdr, mac->mac, EDD_MAC_ADDR_SIZE);
		CM_MEMSET (prov_ctrl->DstIP.IPAdr, 0, EDD_IP_ADDR_SIZE);

		/* fixed prov iodata, same in add and control */
		prov_ctrl->IOParams.AppGroup			= 0;

		/* set prov iodata with control */
		prov_ctrl->CycleReductionRatio			= EDD_CYCLE_REDUCTION_RATIO_UNDEFINED;
		prov_ctrl->CyclePhase					= EDD_CYCLE_PHASE_UNDEFINED;
		prov_ctrl->CyclePhaseSequence			= EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
		prov_ctrl->IOParams.BufferProperties	= EDD_PROV_BUFFER_PROP_IRTE_IMG_UNDEFINED | EDD_PROV_BUFFER_PROP_HERA_MEM_TYPE_UNDEFINED;
		prov_ctrl->IOParams.DataOffset			= EDD_DATAOFFSET_UNDEFINED;
		prov_ctrl->IOParams.CRNumber			= EDD_CRNUMBER_UNDEFINED;
		prov_ctrl->IOParams.SFPosition			= EDD_SFPOSITION_UNDEFINED;
		prov_ctrl->IOParams.SFOffset			= EDD_SFOFFSET_UNDEFINED;

		if (acp_mode & (EDD_PROV_CONTROL_MODE_SET_UNDEFINED_PARAMS | EDD_PROV_CONTROL_MODE_ACTIVATE | EDD_PROV_CONTROL_MODE_ACTIVATE_AUX))
		{
			prov_ctrl->IOParams.BufferProperties	= io_cr->cm.edd_properties;
			prov_ctrl->IOParams.DataOffset			= io_cr->cm.edd_data_offset;
			prov_ctrl->IOParams.CRNumber			= io_cr->cm.edd_cr_number;

			if (io_cr->cm.subframe_data != 0) /* is-dfp */
			{
				prov_ctrl->IOParams.SFPosition	= io_cr->cm.subframe_data & 0x7F;
				prov_ctrl->IOParams.SFOffset	= io_cr->cm.subframe_offset;
			}

			prov_ctrl->CycleReductionRatio	= io_cr->cm.adpt.reduction_ratio;
			prov_ctrl->CyclePhase			= io_cr->cm.adpt.phase;
			prov_ctrl->CyclePhaseSequence	= io_cr->cm.adpt.sequence;
		}

		prov_ctrl->Status = 0;
		prov_ctrl->Mask   = 0;

		if (acp_mode & CM_ARCQ_PROVIDER_SET_STATION_MASK)
		{
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("warning: TODO find a better solution for CM_ARCQ_PROVIDER_SET_STATION_MASK")
#endif /* CM_MESSAGE */
			prov_ctrl->Mode &= ~CM_ARCQ_PROVIDER_SET_STATION_MASK;
			prov_ctrl->Mode |= EDD_PROV_CONTROL_MODE_STATUS;

			if (acp_mode & CM_ARCQ_PROVIDER_SET_STATION_FAILURE) {
				prov_ctrl->Mask |= EDD_CSRT_DSTAT_BIT_STATION_FAILURE;
			}
			if (acp_mode & CM_ARCQ_PROVIDER_SET_STATION_OK) {
				prov_ctrl->Mask   |= EDD_CSRT_DSTAT_BIT_STATION_FAILURE;
				prov_ctrl->Status |= EDD_CSRT_DSTAT_BIT_STATION_FAILURE;
			}
		}

		cm_edd_request_lower(channel, EDD_SRV_SRT_PROVIDER_CONTROL, io_cr, edd); /* io_cr? */
	}
}

/*===========================================================================*/
/*=== FRAME CONTROL DONE ====================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_control_done(
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr = (CM_AR_GRAPH_IO_CR_PTR_TYPE)user_id;
	CM_AR_GRAPH_PTR_TYPE  argr = io_cr->cm.backlink;
	EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE cons_ctrl;
	EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE prov_ctrl;
	LSA_UINT16 edd_mode;

	/***/

	if(io_cr->cm.is_consumer){
		cons_ctrl = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_CONSUMER_CONTROL_PTR_TYPE);
		prov_ctrl = LSA_NULL;
		edd_mode = cons_ctrl->Mode;
	}
	else{
		cons_ctrl = LSA_NULL;
		prov_ctrl = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_CONTROL_PTR_TYPE);
		edd_mode = prov_ctrl->Mode;
	}

	/***/

	if( CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK )
	{
		CM_ACP_TRACE_06(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "io_cr(%08x) frame_id(0x%04x) acp_id(%u), is_consumer(%d) edd_mode(0x%x), edd-rsp(%u)"
			, io_cr, io_cr->frame_id, io_cr->cm.acp_id
			, io_cr->cm.is_consumer, edd_mode, CM_EDD_GET_RESPONSE(edd)
			);

		/* since we can set data with control (e.g. edd_data_offset) control can fail. if so, start AR abort */

		io_cr->cm.cr_state = CM_AR_CR_ERROR;

		if (cm_is_not_null(cons_ctrl))
		{
			cm_ar_com_down_debug_data_set (argr, & cons_ctrl->DebugInfo);
		}

		if (cm_is_not_null(prov_ctrl))
		{
			cm_ar_com_down_debug_data_set (argr, & prov_ctrl->DebugInfo);
		}
	}
	else /* control succeeded */
	{
		if (cm_is_not_null(cons_ctrl))
		{
			CM_ACP_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ConsumerControl.cnf: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) edd_mode(0x%x)"
				, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, edd_mode
				);

			if ((edd_mode & EDD_CONS_CONTROL_MODE_ACTIVATE) != 0)
			{
				if (argr->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED)
				{
					io_cr->cm.cons_data_status = CM_EVENT_APDU_STATUS_INITIAL_SYSRED_VALUE;
				}
				else
				{
					io_cr->cm.cons_data_status = CM_EVENT_APDU_STATUS_INITIAL_VALUE;
				}
			}
		}

		if (cm_is_not_null(prov_ctrl))
		{
			CM_ACP_TRACE_05 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "ProviderControl.cnf: io_cr(%08x) frame_id(0x%04x) is_multi(%u) acp_id(%u) edd_mode(0x%x)"
				, io_cr, io_cr->frame_id, io_cr->cm.is_multicast, io_cr->cm.acp_id, edd_mode
				);

			LSA_UNUSED_ARG (edd_mode);
		}
	}

	cm_arcq_step_next (argr, io_cr, LSA_NULL, edd);
}

/*===========================================================================*/
/*=== FRAME REMOVE ==========================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	LSA_UINT32 service;

	CM_ASSERT(io_cr->cm.cr_state == CM_AR_CR_IDLE);

	/***/

	if (! CM_ACPID_ISVALID(io_cr)) {

		CM_FATAL(); /* a bug in caller */
	}

	if (io_cr->cm.is_consumer) {
		EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE cons_remove = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_CONSUMER_REMOVE_PTR_TYPE);

		CM_ACP_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ConsumerRemove: frame_id(0x%04x) is_cons(%u) is_multi(%u) acp_id(%u)"
			, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast, io_cr->cm.acp_id
			);

		service = EDD_SRV_SRT_CONSUMER_REMOVE;

		cons_remove->ConsumerID = io_cr->cm.acp_id;
	}
	else {
		EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE prov_remove = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_PROVIDER_REMOVE_PTR_TYPE);

		CM_ACP_TRACE_04(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ProviderRemove: frame_id(0x%04x) is_cons(%u) is_multi(%u) acp_id(%u)"
			, io_cr->frame_id, io_cr->cm.is_consumer, io_cr->cm.is_multicast, io_cr->cm.acp_id
			);

		service = EDD_SRV_SRT_PROVIDER_REMOVE;

		prov_remove->ProviderID = io_cr->cm.acp_id;
	}

	/***/

	CM_ACPID_SETINVALID(io_cr); /* set to invalid here, not in cm_acp_frame_remove_done() */

	cm_edd_request_lower(channel, service, io_cr, edd);
}

/*===========================================================================*/
/*=== FRAME REMOVE DONE =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_remove_done (
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr = (CM_AR_GRAPH_IO_CR_PTR_TYPE)user_id;
	CM_AR_GRAPH_PTR_TYPE  argr = io_cr->cm.backlink;

	CM_ASSERT(io_cr->cm.cr_state == CM_AR_CR_IDLE);
	CM_ASSERT(! CM_ACPID_ISVALID(io_cr)); /* see cm_acp_frame_remove() */

	/***/

	if( CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK ) {

		CM_ACP_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_FATAL
			, "FrameRemoveDone: frame_id(0x%04x) edd-rsp(%u)"
			, io_cr->frame_id, CM_EDD_GET_RESPONSE(edd)
			);

		CM_FATAL(); /* could add, so why not remove? */
	}
	else { /* remove succeeded */

		CM_ACP_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_NOTE
			, "FrameRemoveDone: frame_id(0x%04x)"
			, io_cr->frame_id
			);

		/***/

		io_cr->cm.cr_state = CM_AR_CR_IDLE;

		cm_arcq_step_next (argr, io_cr, LSA_NULL, edd);
	}
}

/*===========================================================================*/
/*=== ALARM ADD =============================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_alarm_add (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	LSA_OPCODE_TYPE  opcode;

	CM_ACP_TRACE_03(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AlarmAdd: alarm_cr(%08X) local_alarm_ref(%u) remote_ref(%u)"
		, alarm_cr, alarm_cr->local_alarm_ref, alarm_cr->remote_alarm_ref
		);

	acp->args->alarm.open.alarm_ref = alarm_cr->local_alarm_ref;
	acp->args->alarm.open.dst_alarm_ref = alarm_cr->remote_alarm_ref;
	acp->args->alarm.open.properties = ACP_AL_PROP_USE_GATEWAY_NO;

	CM_MEMCPY (acp->args->alarm.open.dst_mac, & alarm_cr->cm.backlink->ar.eth_mac, sizeof(acp->args->alarm.open.dst_mac));

	if( (alarm_cr->alcr_properties & CM_ALCR_PROP_TRANSPORT_MASK) == CM_ALCR_PROP_TRANSPORT_UDP ) {
		opcode = ACP_OPC_UDP_ALARM_OPEN;
		CM_MEMCPY (acp->args->alarm.open.dst_ip, &alarm_cr->cm.backlink->ar.host_ip, sizeof(acp->args->alarm.open.dst_ip));
	}
	else {
		opcode = ACP_OPC_ALARM_OPEN;
		CM_MEMSET (acp->args->alarm.open.dst_ip, 0, sizeof(acp->args->alarm.open.dst_ip));
	}

	acp->args->alarm.open.vlan_id      = 0; /* see cm_ar_graph_insert_alarm_cr(), TIA 208236 */
	acp->args->alarm.open.timeout100ms = alarm_cr->timeout100ms;
	acp->args->alarm.open.retry_count  = alarm_cr->retries;

	acp->args->alarm.open.usr_id = (LSA_UINT32)alarm_cr->cm.backlink;

	cm_acp_request_lower(channel, opcode, alarm_cr, acp);
}

/*===========================================================================*/
/*=== ALARM ADD DONE ========================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_alarm_add_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	LSA_UNUSED_ARG (channel);

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK )
	{
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "AlarmAddDone: alarm_cr(%08X) acp-rsp(%u)"
			, alarm_cr, CM_ACP_GET_RESPONSE(acp)
			);

		/* note: should not happen, because CM selects the alarm-ref */

		alarm_cr->cm.cr_state = CM_AR_CR_ERROR; /* see remove */
	}
	else
	{
		CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "AlarmAddDone: alarm_cr(%08X) acp-rsp(%u)"
			, alarm_cr, CM_ACP_GET_RESPONSE(acp)
			);

		CM_ASSERT (alarm_cr->cm.cr_state == CM_AR_CR_UP);
	}

	/***/

	cm_arcq_step_next (alarm_cr->cm.backlink, alarm_cr, acp, LSA_NULL);
}

/*===========================================================================*/
/*=== ALARM REMOVE ==========================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_alarm_remove (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	LSA_UINT16  vendor_id, device_id;
	LSA_UINT32  pn_version;

	CM_ASSERT(alarm_cr->cm.cr_state == CM_AR_CR_IDLE);

	vendor_id = device_id = 0;

#if CM_CFG_MAX_CLIENTS
	if (alarm_cr->cm.backlink->ar_com.path_type == CM_PATH_TYPE_CLIENT)
	{
		vendor_id = channel->usr.cl.vendor_id;
		device_id = channel->usr.cl.device_id;
	}
#endif
#if CM_CFG_MAX_SERVERS
	if (alarm_cr->cm.backlink->ar_com.path_type == CM_PATH_TYPE_SERVER)
	{
		vendor_id = alarm_cr->cm.backlink->cm.sv.dev->vendor_id;
		device_id = alarm_cr->cm.backlink->cm.sv.dev->device_id;
	}
#endif

	pn_version =
		( ((PNIO_VERSION		& 0xF) << 28)
		| ((PNIO_DISTRIBUTION	& 0xF) << 24)
		| ((PNIO_PROJECT_NUMBER	& 0xF) << 20)
		| ((PNIO_INCREMENT		& 0xF) << 16)
		| (((PNIO_INTEGRATION_COUNTER * 100 + PNIO_GEN_COUNTER) & 0xFFFF) << 0)
		);

	/***/

	CM_ACP_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AlarmRemove: alarm_cr(%08X) local_alarm_ref(%u) reason(%u)  vendor_id(0x%x) device_id(0x%x) pnio_version(0x%x)"
		, alarm_cr, alarm_cr->local_alarm_ref, alarm_cr->cm.backlink->ar_com.down
		, vendor_id, device_id, pn_version
		);

	acp->args->alarm.close.alarm_ref = alarm_cr->local_alarm_ref;
	acp->args->alarm.close.pnio_status_reason = alarm_cr->cm.backlink->ar_com.down;

	{
		acp->args->alarm.close.error_info.vendor_id			= vendor_id;
		acp->args->alarm.close.error_info.device_id			= device_id;
		acp->args->alarm.close.error_info.pnio_version		= pn_version;
		CM_MEMCPY (acp->args->alarm.close.error_info.debug_data
			, alarm_cr->cm.backlink->ar_com.down_debug_data, sizeof(acp->args->alarm.close.error_info.debug_data)
			);
	}

	cm_acp_request_lower(channel, ACP_OPC_ALARM_CLOSE, alarm_cr, acp);
}

/*===========================================================================*/
/*=== ALARM REMOVE DONE =====================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_alarm_remove_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
) {
	LSA_UNUSED_ARG (channel);

	CM_ASSERT(cm_is_not_null (alarm_cr));
	CM_ASSERT(alarm_cr->cm.cr_state == CM_AR_CR_IDLE);

	/***/

	CM_ACP_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AlarmRemoveDone: alarm_cr(%08X) acp-rsp(%u)"
		, alarm_cr, CM_ACP_GET_RESPONSE(acp)
		);

	/***/

	if( CM_ACP_GET_RESPONSE(acp) != ACP_OK )
	{
		CM_FATAL(); /* could add, so why not remove? */
	}
	else /* remove succeeded */
	{
		/*not!!! alarm_cr->alarm_ref = ACP_INVALID_ALARM_REF;*/
		cm_arcq_step_next (alarm_cr->cm.backlink, alarm_cr, acp, LSA_NULL);
	}
}


/*===========================================================================*/
/*=== FRAME GET APDU STATUS =================================================*/
/*===========================================================================*/

LSA_UINT16
cm_acp_frame_get_apdu_status (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_VOID_PTR_TYPE  user_id
) {
	CM_EDD_LOWER_RQB_PTR_TYPE  edd;
	EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE apdu_status;

	if( ! CM_ACPID_ISVALID(iocr)) {
		CM_ACP_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "!acp-id (not yet added or already removed)");
		return CM_ERR_SESSION;
	}

	edd = cm_edd_alloc_rqb (channel, EDD_SRV_SRT_GET_APDU_STATUS);
	if (cm_is_null (edd)) {
		CM_ACP_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!edd-rqb");
		return CM_ERR_ALLOC_LOWER;
	}

	CM_ACP_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr(%08X) frame_id(0x%04x) acp_id(%u)"
		, iocr, iocr->frame_id, iocr->cm.acp_id
		);

	/***/

	apdu_status = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE);

	apdu_status->ID = iocr->cm.acp_id;

	if (iocr->cm.is_consumer) {

		apdu_status->Type = EDD_CSRT_TYPE_CONSUMER;
	}
	else {

		apdu_status->Type = EDD_CSRT_TYPE_PROVIDER;
	}

	cm_edd_request_lower(channel, EDD_SRV_SRT_GET_APDU_STATUS, user_id, edd);

	return CM_OK_PENDING;
}

/*===========================================================================*/
LSA_VOID
cm_acp_frame_get_apdu_status_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  user_id,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
) {
	EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE apdu_status = CM_EDD_GET_PPARAM(edd, EDD_UPPER_CSRT_GET_APDU_STATUS_PTR_TYPE);
	LSA_UINT16	cycle_counter;
	LSA_UINT8	data_status;
	LSA_UINT8	transfer_status;

	if (CM_EDD_GET_RESPONSE(edd) != EDD_STS_OK)
	{
		cycle_counter	= 0;
		data_status		= 0;
		transfer_status	= 0;

		CM_ACP_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "acp_id(%u)  rsp(%u) != EDD_STS_OK"
			, apdu_status->ID
			, CM_EDD_GET_RESPONSE(edd)
			);
	}
	else
	{
		if( apdu_status->Present == EDD_CSRT_APDU_STATUS_PRESENT ) {

			cycle_counter	= apdu_status->APDUStatus.CycleCnt;
			data_status		= apdu_status->APDUStatus.DataStatus;
			transfer_status	= apdu_status->APDUStatus.TransferStatus;
		}
		else { /* returned values are undefined */

			cycle_counter	= 0;
			data_status		= 0;
			transfer_status	= 0;
		}

		CM_ACP_TRACE_05 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "acp_id(%u)  present(%u) cycle_counter(0x%x) data_status(0x%x) transfer_status(0x%x)"
			, apdu_status->ID
			, apdu_status->Present
			, cycle_counter, data_status, transfer_status
			);
	}

	cm_edd_free_rqb (channel, edd);

	/***/

#if CM_CFG_MAX_SERVERS
	cm_sv_frame_get_apdu_status_done (user_id, cycle_counter, data_status, transfer_status);
#else
	LSA_UNUSED_ARG (user_id);
	LSA_UNUSED_ARG (transfer_status);
	LSA_UNUSED_ARG (data_status);
	LSA_UNUSED_ARG (cycle_counter);
	CM_FATAL(); /* a bug */
#endif
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
