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
/*  F i l e               &F: cm_mc2.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  multicast things                                                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	82
#define CM_MODULE_ID		82

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_MULTICAST_CHANNELS

#define CM_MC_MCONS_NR_OF_EVENTS 2 /* either addr-info or in-data or offline; parallel data-status */
#define CM_MC_MPROV_NR_OF_EVENTS 1 /* either in-data or offline */


/*----------------------------------------------------------------------------*/
static CM_MC_DEVICE_PTR_TYPE
cm_mc_dev_from_device_nr_ (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = LSA_NULL;
	LSA_UINT16  idev;

	for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev)
	{
		CM_MC_DEVICE_PTR_TYPE  loop_dev = channel->usr.mc.dev_arr[idev];

		if (cm_is_not_null(loop_dev) && loop_dev->device_nr == device_nr) {
			dev = loop_dev;
			break;
		}
	}

	return dev;
}


/*----------------------------------------------------------------------------*/
CM_MC_DEVICE_PTR_TYPE
cm_mc_dev_from_device_nr (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr
	)
{
	CM_MC_DEVICE_PTR_TYPE dev;

	dev = cm_mc_dev_from_device_nr_ (channel, device_nr);

	if (cm_is_null (dev)) {
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "channel(%d), dev(%d) not found"
			, channel->my_handle, device_nr
			);
	}

	return dev;
}


/*----------------------------------------------------------------------------*/
static  LSA_UINT32
cm_ar_graph_from_multicast_arcb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_MC_DEVICE_ADD_CONST_PTR_TYPE  add,
	LSA_UINT16  local_clock,
	CM_AR_GRAPH_PTR_TYPE * ar_graph
	)
{
	CM_MAC_TYPE null_mac = { {0, 0, 0, 0, 0, 0} };
	CM_UPPER_ARCB_ADD_PTR_TYPE  cb_ar = add->arcb;
	CM_AR_GRAPH_PTR_TYPE  argr;
	LSA_UINT16  block_field;
	CLRPC_UUID_TYPE  cmi_object_uuid;

	if (cm_is_null (cb_ar)) {
		return CM_PNIO_FAULTY_Connect__CMCTL_StateConflict();
	}

	if (! (1 <= cb_ar->ar_nr && cb_ar->ar_nr <= 255)) {
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u): only ar_nr 1..255 allowed"
			, add->device_nr, cb_ar->ar_nr
			);
		return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
	}

	argr = cm_ar_graph_alloc (channel, add->device_nr, cb_ar->ar_nr);

	if (cm_is_null (argr)) {
		return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
	}

	*ar_graph = argr; /* see cm_ar_graph_free() in caller */

	/***/

	cm_rpc_make_object_uuid (& cmi_object_uuid, 0, 0, 0); /* for check in cm_ar_graph_insert_io_ar() */

	/* note: STEP7 does not generate an AR-Block for MC-endpoints */

	block_field = cm_ar_graph_insert_io_ar (argr,
			0,
			CM_AR_TYPE_SINGLE_RTC3, /* AP00289668, effectively allows all send-clocks */
			& cm_uuid_nil_uuid,
			null_mac.mac,
			& cmi_object_uuid,
			CM_AR_PROP_STATE_ACTIVE | CM_AR_PROP_STARTUP_MODE_ADVANCED, /* advanced(!) see cm_acp_frame_add() */
			100, /* unused (but range-checked) */
			0,
			0, LSA_NULL
		);

	if (block_field != 0) {
		CM_MC_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "io_ar.block_field(%u)", block_field);

		switch (block_field) {
		case 255: /* no mem */
			return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
		default:
			return CM_PNIO_FAULTY_ARBlockReq (block_field);
		}
	}

		/* IO-CR blocks */
	{
	CM_UPPER_ARCB_IOCR_PTR_TYPE  cb_iocr;
	LSA_UINT16  mcons_count = 0;
	LSA_UINT16  mprov_count = 0;
	LSA_UINT16  other_count = 0;

	for (CmListForeach (cb_iocr, &cb_ar->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
	{
		if (cb_iocr->iocr_type == CM_IOCR_TYPE_MULTICAST_CONSUMER) {
			mcons_count += 1;
		}
		else if (cb_iocr->iocr_type == CM_IOCR_TYPE_MULTICAST_PROVIDER) {
			mprov_count += 1;
		}
		else {
			other_count += 1;
		}
	}

	if ((other_count != 0)
	||	(mprov_count != 0 && mcons_count != 0)
	||	(mprov_count == 0 && mcons_count != 1))
	{
		CM_MC_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "wrong iocr-count, mprov_count(%u) mcons_count(%u) other_cnt(%u)"
			, mprov_count, mcons_count, other_count
			);
		return CM_PNIO_FAULTY_IOCRBlockReq (4);
	}

	for (CmListForeach (cb_iocr, &cb_ar->iocr_list, CM_UPPER_ARCB_IOCR_PTR_TYPE))
	{
		CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

		block_field = cm_ar_graph_insert_io_cr (argr
			, cb_iocr->iocr_type, cb_iocr->iocr_ref
			, 0x8892, cb_iocr->iocr_properties
			, cb_iocr->data_length, cb_iocr->frame_id
			, cb_iocr->send_clock, cb_iocr->reduction_ratio, cb_iocr->phase
			, cb_iocr->frame_send_offset
			, cb_iocr->data_hold_factor/*AP01304706*/, cb_iocr->data_hold_factor
			, 0xC000/*prio 6, id 0, see create*/
			, cb_iocr->iocr_multicast_mac.mac
			, & iocr
			);
		if (block_field == 0) {
			block_field = cm_cl_ar_graph_set_iocr_edd_data (iocr
				, cb_iocr->edd_data_offset, cb_iocr->edd_properties
				, cb_iocr->edd_cr_number, cb_iocr->edd_forward_mode, cb_iocr->edd_ct_outdatastart
				, cb_iocr->partial_length, cb_iocr->partial_offset, cb_iocr->apdu_status_length
				, cb_iocr->subframe_data, cb_iocr->subframe_offset
				);
		}
		if (block_field != 0) {
			CM_MC_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "io_cr.block_field(%u) iocr_ref(%u)"
				, block_field, cb_iocr->iocr_ref
				);

			switch (block_field) {
			case 255: /* no mem */
			case 254: /* no provider- or consumer-resource */
			case 253: /* unreachable */
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
			default:
				return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
			}
		}
		CM_ASSERT (cm_is_not_null (iocr));

			/* prepare cnf */
		cb_iocr->frame_id = iocr->frame_id;	/* cnf ! */

		cb_iocr->edd_apdustatus_offset	= EDD_DATAOFFSET_INVALID;
		cb_iocr->edd_cw_offset			= EDD_DATAOFFSET_INVALID;
		cb_iocr->edd_datastatus_offset	= EDD_DATAOFFSET_INVALID;

			/* rest don't care */
	}
	}

		/* IO-CR local-adaption blocks */
	{
		CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE  cb_iocr_local;
		LSA_UINT16  adpt_send_clock = 0;

		for (CmListForeach (cb_iocr_local, &cb_ar->iocr_local_list, CM_UPPER_ARCB_IOCR_LOCAL_PTR_TYPE))
		{
			block_field = cm_cl_ar_graph_set_iocr_adaption (argr, cb_iocr_local->iocr_ref, local_clock
				, cb_iocr_local->send_clock, cb_iocr_local->reduction_ratio, cb_iocr_local->phase
				, cb_iocr_local->frame_send_offset, cb_iocr_local->data_hold_factor
				);
			if (block_field != 0) {
				CM_MC_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iocr_adaption.block_field(%u)", block_field);
				/* no 'no mem' block_field */
				return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
			}
			if (adpt_send_clock == 0)
			{
				adpt_send_clock = cb_iocr_local->send_clock;
			}
		}

			/* AP01009553: if no adaptation is given, take the iocr-data as adaptation */
		if (adpt_send_clock == 0)
		{
			CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
			for (CmListForeach (iocr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
			{
				block_field = cm_cl_ar_graph_set_iocr_adaption (argr, iocr->iocr_ref, local_clock
					, iocr->send_clock, iocr->reduction_ratio, iocr->phase
					, iocr->frame_send_offset, iocr->data_hold_factor
					);
				if (block_field != 0)
				{
					CM_FATAL1(block_field); /* adaptation to self failed */
					return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
				}
				if (adpt_send_clock == 0)
				{
					adpt_send_clock = iocr->send_clock;
				}
			}
		}

		{
			CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

			for (CmListForeach (iocr, &argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
			{
				if (iocr->cm.adpt.send_clock == 0)
				{
					CM_MC_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "adaptation of iocr_ref(%u) missing", iocr->iocr_ref);
					return CM_PNIO_FAULTY_IOCRBlockReq (10);  /* adpt.send_clock not set */
				}
				if (iocr->cm.adpt.send_clock != adpt_send_clock)
				{
					CM_MC_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "adaptation of iocr_ref(%u) differ to other adaption", iocr->iocr_ref);
					return CM_PNIO_FAULTY_IOCRBlockReq (10);  /* adpt.send_clock differ */
				}
			}

			argr->ar_com.ar_send_clock = adpt_send_clock; /* same as local_clock, see cm_cl_ar_graph_set_iocr_adaption() */
		}
	}


	{
	CM_UPPER_ARCB_MCR_PTR_TYPE  cb_mcr;

	for (CmListForeach (cb_mcr, &cb_ar->mcr_list, CM_UPPER_ARCB_MCR_PTR_TYPE))
	{
		CM_AR_GRAPH_MCR_PTR_TYPE  mcr;

		block_field = cm_ar_graph_insert_mcr (argr,
				cb_mcr->iocr_ref, cb_mcr->mcr_properties, cb_mcr->mci_timeout_100ms,
				cb_mcr->provider_name_length, cb_mcr->provider_name,
				& mcr
			);
		if (block_field != 0) {
			CM_MC_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "mcr.block_field(%u)", block_field);

			switch (block_field) {
			case 255: /* no mem */
				return CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
			default:
				return CM_PNIO_FAULTY_MCRBlockReq (block_field);
			}
		}
	}
	}

	/* note: alarm-cr  don't care */

	/* note: ap / module / submodule  don't care */

#if CM_DEBUG
	cm_ar_graph_dump (argr);
#endif

	block_field = cm_ar_graph_check_iocr_against_mcr (argr);
	if (block_field != 0) {
		CM_MC_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "return from cm_ar_graph_check_iocr_against_mcr:block_field(%u)"
			, block_field
			);
		return CM_PNIO_FAULTY_IOCRBlockReq (block_field);
	}

	block_field = cm_ar_graph_check_mcr_against_iocr (argr);
	if (block_field != 0) {
		CM_MC_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "return from cm_ar_graph_check_mcr_against_iocr:block_field(%u)"
			, block_field
			);
		return CM_PNIO_FAULTY_MCRBlockReq (block_field);
	}

	return CM_PNIO_ERR_NONE;
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_DEVICE_ADD
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_device_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_MC_DEVICE_ADD_PTR_TYPE  add = rb->args.mc.dev_add;
	CM_MC_DEVICE_PTR_TYPE  dev;
	CM_AR_GRAPH_PTR_TYPE  argr;
	CM_AR_GRAPH_IO_CR_PTR_TYPE  mcons;
	LSA_UINT16  local_clock;
	LSA_BOOL    is_reduction_ratio_fixed;

	add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_StateConflict();

	if (! cm_pdusr_is_parameterized (channel, &local_clock, &is_reduction_ratio_fixed)) {
		CM_MC_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "pdev not parameterized");
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	if (cm_pdusr_is_reset_to_factory (channel)) {
		CM_MC_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "pdev is reset-to-factory");
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	if (cm_pdusr_is_masterproject (channel)) {
		CM_MC_TRACE_00 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "pdev is configured as MasterProject, MC not allowed");
		/* note: must change cm_mc_pdev_set_down_ind() if MasterProject supported */
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

		/* check params */

	if (! (1 <= add->device_nr  &&  add->device_nr <= CM_MC_MAX_DEVICE_NR)) {
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) device_nr != (1..%d)"
			, add->device_nr, CM_MC_MAX_DEVICE_NR
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	dev = cm_mc_dev_from_device_nr_ (channel, add->device_nr);

	if (cm_is_not_null (dev)) {
		CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) already exists", add->device_nr
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	/***/

	{
	LSA_UINT16  idev;

	for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev)
	{
		if (cm_is_null(channel->usr.mc.dev_arr[idev]))
		{
			break; /* free */
		}
	}

	if (idev >= channel->usr.mc.dev_cnt)
	{
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no more resources for dev(%u), max dev_cnt(%u)"
			, add->device_nr, channel->usr.mc.dev_cnt
			);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	argr = LSA_NULL;
	add->pnio_status = cm_ar_graph_from_multicast_arcb (channel, add, local_clock, & argr);
	if (add->pnio_status != CM_PNIO_ERR_NONE || cm_is_null(argr))
	{
		if (cm_is_not_null(argr)) {
			cm_ar_graph_free (argr);
		}
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%d) pnio_status(0x%x) on build ar-graph", add->device_nr, add->pnio_status
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	mcons = CmListFirst(&argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE);
	if ( cm_is_null(mcons) || ! mcons->cm.is_consumer) {
		mcons = LSA_NULL; /* m-provider */
	}

	/***/

		/* device alloc */
	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& dev), sizeof (*dev));
	if (cm_is_null (dev)) {
		CM_MC_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
				"h(%d): dev(%d) no mem for dev, size(%d)",
				channel->my_handle, add->device_nr, sizeof(*dev)
			);
		cm_ar_graph_free (argr);
		add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
		cm_callback (channel, CM_ERR_ALLOC_LOCAL, & rb);
		return;
	}

	dev->nare_ident   = LSA_NULL;
	dev->nare_cancel  = LSA_NULL;

	if( cm_is_not_null(mcons) ) {
		dev->nare_ident = cm_nare_alloc_rqb (channel);
		dev->nare_cancel = cm_nare_alloc_rqb (channel);

		if (cm_is_null (dev->nare_ident) || cm_is_null (dev->nare_cancel)) {
			LSA_UINT16 rc;

			CM_MC_TRACE_02(channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
				"h(%d): dev(%d) no mem for nare-rqb"
				, channel->my_handle, add->device_nr
				);

			if (cm_is_not_null (dev->nare_ident)) {
				cm_nare_free_rqb(channel, dev->nare_ident);
			}

			if (cm_is_not_null (dev->nare_cancel)) {
				cm_nare_free_rqb(channel, dev->nare_cancel);
			}

			CM_FREE_LOCAL_MEM (& rc, dev);
			CM_ASSERT (rc == LSA_RET_OK);

			cm_ar_graph_free (argr);

			add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
			cm_callback (channel, CM_ERR_ALLOC_LOCAL, & rb);
			return;
		}
	}

	/***/

	argr->cm.mc.dev				= dev; /* backlink from "ar-graph" to "dev" */

	dev->trace_idx				= channel->trace_idx;

	dev->channel				= channel;

	dev->device_nr				= add->device_nr;

	dev->mcons					= mcons;

	dev->state					= CM_MST_ADDING;
	dev->wait_for				= 0;

	dev->ar_graph				= argr;
	dev->ar_graph->session_key	= 0;

	CmListInitialize (& dev->event_q);

	dev->upper_add_rm			= LSA_NULL;
	dev->upper_ctrl				= LSA_NULL;
	dev->upper_apdu				= LSA_NULL;

	cm_oneshot_init(&dev->mcons_timer, cm_mc_mcons_timeout, dev);

	dev->last_reason_code = CM_AR_REASON_NONE;

	/***/

		/* announce needed event resources */
	if ( cm_is_not_null(dev->mcons) ) {
		add->nr_of_event_resources = CM_MC_MCONS_NR_OF_EVENTS;
	}
	else{
		add->nr_of_event_resources = CM_MC_MPROV_NR_OF_EVENTS;
	}

	add->nr_of_alarm_resources = 0; /* no alarms */

	CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%u): needed nr_of_event_resources(%u)"
		, add->device_nr, add->nr_of_event_resources
		);

	/***/

	channel->usr.mc.dev_arr[idev] = dev;
	}

	/***/

	if (cm_pdusr_attach (channel, local_clock) != CM_OK)
	{
		CM_FATAL();
	}

	dev->wait_for |= CM_MWF__IS_PD_ATTACHED;

	/***/

	CM_RQB_SET_RESPONSE (rb, CM_OK);

	dev->upper_add_rm = rb;

	cm_arcq_request (dev->ar_graph, CM_ARCQ_OPC_FRAMES_ADD, 0);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_ar_frames_add_done (
	CM_AR_GRAPH_PTR_TYPE  argr
	)
{
	CM_MC_DEVICE_PTR_TYPE dev = argr->cm.mc.dev;

	CM_MC_TRACE_02 (dev->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%d) wait_for(0x%x)"
		, dev->device_nr
		, dev->wait_for
		);

	if( dev->state != CM_MST_ADDING )
	{
		CM_FATAL();
	}
	else if( cm_ar_com_is_down(argr) )
	{
		cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_REMOVE, 0);
	}
	else
	{
		CM_UPPER_MC_DEVICE_ADD_PTR_TYPE  add = dev->upper_add_rm->args.mc.dev_add;

		/* update the out-parameters */
		if ( cm_is_not_null(add->arcb) ) {
			cm_arcb_device_add_set_iocr_cnf(argr, add->arcb);
		}
		else {
			CM_FATAL();
		}

		add->pnio_status = CM_PNIO_ERR_NONE;

		/***/

		dev->state = CM_MST_PASSIVE;

		cm_callback (dev->channel, CM_OK, & dev->upper_add_rm); /* first call back */

		cm_ar_com_ready (argr); /* then enable down-indication, see cm_mc_ar_com_down() */
	}
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_DEVICE_REMOVE
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_device_remove (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev;

	dev = cm_mc_dev_from_device_nr (channel, rb->args.mc.dev_remove->device_nr);

	if (cm_is_null (dev)) {
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "h(%d): dev(%d) not found",
				channel->my_handle, rb->args.mc.dev_remove->device_nr
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (cm_is_not_null (dev->upper_add_rm)) {
		CM_MC_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "h(%d): upper(0x%x) with opcode (%d) is running",
				channel->my_handle, dev->upper_add_rm, CM_RQB_GET_OPCODE (dev->upper_add_rm)
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	CM_MC_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_WARN,
			"h(%d): remove dev(%d) state(%d) wait_for(0x%x)",
			channel->my_handle, dev->device_nr, dev->state, dev->wait_for
		);

		/* start dev-sm */
	dev->upper_add_rm = rb; /* keep */

	dev->state = CM_MST_REMOVING;

	if( cm_ar_com_is_down(dev->ar_graph) ) {
		cm_mc_device_sm(dev);
	}
	else {
		cm_ar_com_down_debug_data_fmt(dev->ar_graph, CM_MODULE_ID, __LINE__, CM_AR_REASON_RMV, 0);
		cm_ar_com_down (dev->ar_graph, CM_AR_REASON_RMV);
	}
}

/*----------------------------------------------------------------------------*/
static CM_AR_GRAPH_PTR_TYPE
cm_mc_ar_frames_remove_done(
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;
	CM_CHANNEL_PTR_TYPE    channel = dev->channel;
	CM_UPPER_RQB_PTR_TYPE  upper;

	CM_MC_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "h(%d): dev(%d) state(%d) wait_for(0x%x)"
		, channel->my_handle, argr->device_nr, dev->state, dev->wait_for
		);

	if(
		! (dev->state == CM_MST_ADDING || dev->state == CM_MST_REMOVING)
		|| cm_is_null(dev->upper_add_rm)
	) {
		CM_FATAL();
	}

	/***/

	if (dev->wait_for & CM_MWF__IS_PD_ATTACHED) {
		dev->wait_for &= ~CM_MWF__IS_PD_ATTACHED;
		cm_pdusr_detach (channel);
	}

	if (dev->wait_for & CM_MWF__IS_RESET_TO_FACTORY) {
		dev->wait_for &= ~CM_MWF__IS_RESET_TO_FACTORY;
	}

	if (dev->wait_for & CM_MWF__DO_FRAME_RTF_REQ) {
		dev->wait_for &= ~CM_MWF__DO_FRAME_RTF_REQ;
		cm_mc_pdev_set_down_check(dev->channel);
	}

	if (dev->wait_for != 0) {
		CM_FATAL();
	}

	/***/

	for(;;) {
		CM_UPPER_RQB_PTR_TYPE event;
		event = cm_mc_dev_event_get(dev);
		if( cm_is_null(event) ) {
			break;
		}
		cm_callback(channel, CM_OK_CANCELLED, &event);
	}

	if (cm_is_not_null (dev->ar_graph)) {
		cm_arcq_free_ar_graph(dev->ar_graph);
		dev->ar_graph = LSA_NULL;
	}

	if( cm_is_not_null(dev->nare_ident) ) {
		cm_nare_free_rqb(channel, dev->nare_ident);
	}

	if( cm_is_not_null(dev->nare_cancel) ) {
		cm_nare_free_rqb(channel, dev->nare_cancel);
	}

	/***/

	if( cm_is_null(dev->upper_add_rm) ) {
		CM_FATAL();
	}

	upper = dev->upper_add_rm;
	dev->upper_add_rm = LSA_NULL;

	{
	LSA_UINT16  idev;
	LSA_UINT16  rc;

	for (idev = 0; idev < channel->usr.mc.dev_cnt; ++idev)
	{
		if (dev == channel->usr.mc.dev_arr[idev])
		{
			break;
		}
	}

	if (idev >= channel->usr.mc.dev_cnt)
	{
		CM_FATAL();
	}

	channel->usr.mc.dev_arr[idev] = LSA_NULL;

	CM_FREE_LOCAL_MEM (& rc, dev);
	CM_ASSERT (rc == LSA_RET_OK);
	}

	/***/

	switch (CM_RQB_GET_OPCODE (upper))
	{
	case CM_OPC_MC_DEVICE_REMOVE:
		cm_callback (channel, CM_OK, & upper);
		break;

	case CM_OPC_MC_DEVICE_ADD:
		upper->args.mc.dev_add->pnio_status = CM_PNIO_FAULTY_Connect__CMCTL_OutOfResource();
		cm_callback (channel, CM_ERR_LOWER_LAYER, & upper);
		break;

	default:
		CM_FATAL();
		break;
	}

	return LSA_NULL; /* dev (and dev->ar_graph) freed now */
}


/*------------------------------------------------------------------------------
// handle opcode CM_OPC_MC_DEVICE_CONTROL
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_device_control (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
	)
{
	CM_UPPER_MC_DEVICE_CONTROL_PTR_TYPE  ctrl = rb->args.mc.dev_control;
	CM_MC_DEVICE_PTR_TYPE  dev;

	dev = cm_mc_dev_from_device_nr (channel, ctrl->device_nr);

	if (cm_is_null (dev)) {
		CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "h(%d): dev(%d) not found", channel->my_handle, ctrl->device_nr
			);
		cm_callback (channel, CM_ERR_PARAM, & rb);
		return;
	}

	if (cm_is_not_null (dev->upper_add_rm)) {
		CM_MC_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u): upper_add_rm(0x%x) is running, opc(%u)"
			, dev->device_nr, dev->upper_add_rm, CM_RQB_GET_OPCODE (dev->upper_add_rm)
			);
		cm_callback (channel, CM_ERR_SEQUENCE, & rb);
		return;
	}

	if (ctrl->control_tag == CM_CL_DEVICE_CONTROL_APDU_STATUS) {

		LSA_UINT16  acp_state;

		if (cm_is_not_null (dev->upper_apdu)) {
			CM_MC_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
					"dev(%d): another upper_apdu(0x%x) is running",
					dev->device_nr, dev->upper_apdu
				);
			cm_callback (channel, CM_ERR_SEQUENCE, & rb);
			return;
		}

		if (dev->state < CM_MST_PASSIVE || dev->state > CM_MST_PASSIVATING) { /* always false */
			CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "wrong state(%d)", dev->state);
			cm_callback (channel, CM_ERR_SEQUENCE, & rb);
			return;
		}

		if (ctrl->apdu_status == CM_CL_AR_CONTROL_STATION_OK) {
			acp_state = CM_ARCQ_PROVIDER_SET_STATION_OK;
		}
		else if (ctrl->apdu_status == CM_CL_AR_CONTROL_STATION_PROBLEM) {
			acp_state = CM_ARCQ_PROVIDER_SET_STATION_FAILURE;
		}
		else {
			CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR, "!apdu_status(%x)", ctrl->apdu_status);
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}

		if ( cm_is_not_null(dev->mcons) )
		{
			cm_callback (channel, CM_OK, & rb); /* do nothing and tell OK */
		}
		else
		{
			dev->upper_apdu = rb; /* keep */

			cm_arcq_request (dev->ar_graph, CM_ARCQ_OPC_PROV_DATASTATUS_SET, acp_state);
		}
	}
	else { /* activate or passivate */

		if (ctrl->control_tag == CM_CL_DEVICE_CONTROL_ACTIVATE) {
			if (dev->wait_for & CM_MWF__IS_RESET_TO_FACTORY) {
				CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
						"dev(%d): ResetToFactory was running, only device-remove allowed",
						dev->device_nr
					);
				cm_callback (channel, CM_ERR_SEQUENCE, & rb);
				return;
			}
		}

		switch (ctrl->control_tag) {
		case CM_CL_DEVICE_CONTROL_ACTIVATE:
		case CM_CL_DEVICE_CONTROL_PASSIVATE:
			if (cm_is_not_null (dev->upper_ctrl)) {
				CM_MC_TRACE_03 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
						"dev(%d): upper_ctrl(0x%x) is running with control_tag(%d)",
						dev->device_nr, dev->upper_ctrl,
						dev->upper_ctrl->args.mc.dev_control->control_tag
					);
				cm_callback (channel, CM_ERR_SEQUENCE, & rb);
				return;
			}

			if (ctrl->control_tag == CM_CL_DEVICE_CONTROL_ACTIVATE) {
				if (dev->state == CM_MST_PASSIVE) { /* resource check only if passive! */
					LSA_UINT16 nr_of_events;
					CmListLength(&dev->event_q, &nr_of_events);

					if ( cm_is_not_null(dev->mcons) ) {
						if (nr_of_events < CM_MC_MCONS_NR_OF_EVENTS) {
							CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
									"dev(%d): too few resources",
									dev->device_nr
								);
							cm_callback (channel, CM_ERR_SEQUENCE, & rb);
							return;
						}
					}
					else {
						if (nr_of_events < CM_MC_MPROV_NR_OF_EVENTS) {
							CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_ERROR,
									"dev(%d): too few resources",
									dev->device_nr
								);
							cm_callback (channel, CM_ERR_SEQUENCE, & rb);
							return;
						}
					}
				}
			}

			dev->upper_ctrl = rb;
			break;

		default:
				/* or param error */
			CM_MC_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP, "!control_tag(0x%x)", ctrl->control_tag);
			cm_callback (channel, CM_ERR_PARAM, & rb);
			return;
		}

		CM_MC_TRACE_04 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u): mc-device-control: state(%d) wait_for(0x%x)  control_tag(%d)"
			, dev->device_nr, dev->state, dev->wait_for, ctrl->control_tag
			);

			/* kick statemachine */
		switch (ctrl->control_tag) {
		case CM_CL_DEVICE_CONTROL_ACTIVATE:

			switch (dev->state) {
			case CM_MST_PASSIVE:
				dev->state = CM_MST_ACTIVE;
				cm_callback (channel, CM_OK, & dev->upper_ctrl);
				dev->wait_for |= CM_MWF__DO_START;
				cm_mc_device_sm (dev);
				break;

			case CM_MST_ACTIVE:
				/* do nothing */
				cm_callback (channel, CM_OK, & dev->upper_ctrl);
				break;

			default:
				CM_MC_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "dev(%d) state(%d) wait_for(0x%x) upper_add_rm(0x%x) upper_ctrl(0x%x) upper_apdu(0x%x)"
					, dev->device_nr, dev->state, dev->wait_for
					, dev->upper_add_rm, dev->upper_ctrl, dev->upper_apdu
					);
				CM_FATAL1 (dev->state);
				break;
			}
			break;


		case CM_CL_DEVICE_CONTROL_PASSIVATE:

			switch (dev->state) {
			case CM_MST_PASSIVE:
				cm_callback (channel, CM_OK, & dev->upper_ctrl);
				break;

			case CM_MST_ACTIVE:
				dev->state = CM_MST_PASSIVATING;

				if( cm_ar_com_is_down(dev->ar_graph) ) {
					cm_mc_device_sm (dev);
				}
				else {
					cm_ar_com_down_debug_data_fmt(dev->ar_graph, CM_MODULE_ID, __LINE__, CM_AR_REASON_PAS, 0);
					cm_ar_com_down (dev->ar_graph, CM_AR_REASON_PAS);
				}
				break;

			default:
				CM_MC_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_FATAL
					, "dev(%d) state(%d) wait_for(0x%x) upper_add_rm(0x%x) upper_ctrl(0x%x) upper_apdu(0x%x)"
					, dev->device_nr, dev->state, dev->wait_for
					, dev->upper_add_rm, dev->upper_ctrl, dev->upper_apdu
					);
				CM_FATAL1 (dev->state);
				break;
			}
			break;


		default:
			CM_FATAL();
			break;
		}
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_ar_mprov_data_status_control_done (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;

	if (cm_is_null(dev->upper_apdu)) {
		CM_FATAL();
	}

	CM_MC_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%d) state(%d) wait_for(0x%x)"
		, dev->device_nr, dev->state, dev->wait_for
		);

	cm_callback (dev->channel, CM_OK, & dev->upper_apdu);
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_device_event (
	CM_MC_DEVICE_PTR_TYPE  dev,
	LSA_OPCODE_TYPE  event_opcode,
	LSA_UINT8 reason_code
	)
{
	CM_UPPER_RQB_PTR_TYPE  upper;
	CM_UPPER_EVENT_PTR_TYPE  upper_event;

	switch (event_opcode) {
	case CM_OPC_MC_EVENT_AR_IN_DATA_IND:
		CM_MC_TRACE_03 (dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) skey(%d) upper_event(%d/IN_DATA)"
			, dev->device_nr
			, dev->ar_graph->session_key
			, event_opcode
			);
		break;

	case CM_OPC_MC_EVENT_AR_OFFLINE_IND:
		CM_MC_TRACE_04 (dev->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
			, "dev(%u) skey(%d) upper_event(%d/OFFLINE/reason=%u)"
			, dev->device_nr
			, dev->ar_graph->session_key
			, event_opcode
			, reason_code
			);
		break;

	case CM_OPC_MC_EVENT_AR_ADDR_INFO_IND:
		CM_MC_TRACE_04 (dev->trace_idx, LSA_TRACE_LEVEL_NOTE_LOW
			, "dev(%u) skey(%d) upper_event(%d/ADDR_INFO/reason=%u)"
			, dev->device_nr
			, dev->ar_graph->session_key
			, event_opcode
			, reason_code
			);
		break;

	default:
		CM_FATAL();
		break;
	}

	upper = cm_mc_dev_event_get (dev);
	if (cm_is_null (upper)) {
		CM_FATAL(); /* a bug */
		return;
	}

	upper_event = upper->args.mc.event_ind;

		/* common for all events */
	upper_event->device_nr = dev->device_nr;
	upper_event->ar_nr = dev->ar_graph->ar_nr;
	upper_event->session_key = dev->ar_graph->session_key;

	switch (event_opcode) {
	case CM_OPC_MC_EVENT_AR_ADDR_INFO_IND:
		upper_event->u.mc.addr_info.reason_code = reason_code;
		upper_event->u.mc.addr_info.device_mac  = dev->ar_graph->ar.eth_mac;
		upper_event->u.mc.addr_info.device_ip   = dev->ar_graph->ar.host_ip;
		dev->wait_for |= CM_MWF__ADDR_INFO_RSP;
		break;

	case CM_OPC_MC_EVENT_AR_IN_DATA_IND:
		upper_event->u.mc.in_data.nothing = 0;
		dev->wait_for |= CM_MWF__IN_DATA_RSP;
		break;

	case CM_OPC_MC_EVENT_AR_OFFLINE_IND:
		dev->ar_graph->session_key += 1; /* build session_key of next session */
		upper_event->session_key = dev->ar_graph->session_key;

		CM_MC_TRACE_01 (dev->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "new session_key(%d)"
			, dev->ar_graph->session_key
			);

		upper_event->u.mc.ar_offline.reason_code = reason_code;
		dev->wait_for |= CM_MWF__OFFLINE_RSP;
		break;

	default:
		CM_FATAL1 (event_opcode);
		return;
	}

	dev->last_reason_code = reason_code;

	CM_RQB_SET_OPCODE (upper, event_opcode);
	cm_callback (dev->channel, CM_OK, & upper);
}


/*------------------------------------------------------------------------------
// multicast statemachine
//----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_device_sm (
	CM_MC_DEVICE_PTR_TYPE  dev
	)
{
	CM_AR_GRAPH_PTR_TYPE  argr = dev->ar_graph;
	LSA_BOOL is_down = cm_ar_com_is_down(argr);

	CM_MC_TRACE_04 (dev->trace_idx, LSA_TRACE_LEVEL_NOTE,
		"dev(%d) state(%d) wait_for(0x%x) is_down(%u)",
		dev->device_nr, dev->state, dev->wait_for, is_down
		);

	/***/

	if (dev->wait_for & (CM_MWF__FRAMES_ACT_CNF | CM_MWF__FRAMES_PAS_CNF | CM_MWF__WF_FRAME_RTF_CNF)) {
		return;
	}

	if (dev->wait_for & (CM_MWF__IN_DATA_RSP | CM_MWF__OFFLINE_RSP | CM_MWF__ADDR_INFO_RSP)) {
		return;
	}

	/***/

	if (dev->wait_for & CM_MWF__DO_ABORT) { /* see cm_mc_ar_com_down() */

		dev->wait_for &= ~CM_MWF__DO_ABORT;

		dev->wait_for |= CM_MWF__FRAMES_PAS_CNF;
		cm_arcq_request (argr, CM_ARCQ_OPC_FRAMES_PASSIVATE, 0);

		if ( cm_is_not_null(dev->mcons) ) { /* m-consumer */

			if ((dev->wait_for & CM_MWF__NARE_PAUSE) != 0) {
				dev->wait_for &= ~CM_MWF__NARE_PAUSE;
				cm_oneshot_stop(&dev->mcons_timer);
			}

			if ((dev->wait_for & CM_MWF__NARE_CNF) != 0) {
				if ((dev->wait_for & CM_MWF__NARE_CANCEL) == 0) {
					cm_mc_mcons_ident_cancel(dev);
				}
			}
		}

		return;
	}

	/***/

	if (dev->wait_for & CM_MWF__DO_OFFLINE) { /* see cm_mc_ar_frames_passivate_done() */

		dev->wait_for &= ~CM_MWF__DO_OFFLINE;

		if ((dev->wait_for & CM_MWF__IS_IN_DATA) != 0) {
			dev->wait_for &= ~CM_MWF__IS_IN_DATA;
			cm_mc_device_event (dev, CM_OPC_MC_EVENT_AR_OFFLINE_IND, dev->ar_graph->ar_com.down);
			return;
		}
		else if (dev->state != CM_MST_REMOVING) {
			if( dev->ar_graph->ar_com.down != dev->last_reason_code ) {
				cm_mc_device_event (dev, CM_OPC_MC_EVENT_AR_ADDR_INFO_IND, dev->ar_graph->ar_com.down);
				return;
			}
			else {
				/* do nothing (case: nare.cnf(-), nare.cnf(-), ...) */
			}
		}
		else {
			/* do nothing */
		}
	}

	/***/

	if (dev->wait_for & (CM_MWF__NARE_CNF | CM_MWF__NARE_CANCEL)) {
		return;
	}

	if (dev->wait_for & CM_MWF__APDU_STATUS_RSP) {
		return;
	}

	/***/

	if ( cm_is_not_null(dev->mcons) ) { /* m-consumer */

		if (dev->wait_for & CM_MWF__DO_START) {

			dev->wait_for &= ~CM_MWF__DO_START;

			if( cm_pdusr_is_any_port_irt_run(dev->channel) ) {

				dev->wait_for |= CM_MWF__DO_RESOLVE;
			}
			else {

				dev->wait_for |= CM_MWF__WF_IRT_RUN;

				/* note: link-down is only one possibe reason for "not IRT-run" */
				cm_mc_device_event (dev, CM_OPC_MC_EVENT_AR_ADDR_INFO_IND, CM_AR_REASON_LNK_DOWN);
				return;
			}
		}

		if( (dev->wait_for & CM_MWF__DO_RESOLVE) != 0 ) {

			dev->wait_for &= ~CM_MWF__DO_RESOLVE;

			cm_mc_mcons_ident(dev);
			return;
		}

		if( (dev->wait_for & CM_MWF__DO_PRE_DATA) != 0 ) {

			dev->wait_for &= ~CM_MWF__DO_PRE_DATA;

			cm_mc_device_event (dev, CM_OPC_MC_EVENT_AR_ADDR_INFO_IND, CM_AR_REASON_NONE);

			dev->wait_for |= CM_MWF__FRAMES_ACT_CNF;
			cm_arcq_request(dev->ar_graph, CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK, 0);

			return;
		}

		if( (dev->wait_for & CM_MWF__DO_IN_DATA) != 0 ) {

			dev->wait_for &= ~CM_MWF__DO_IN_DATA;

			dev->wait_for |= CM_MWF__IS_IN_DATA;

			cm_mc_device_event (dev, CM_OPC_MC_EVENT_AR_IN_DATA_IND, CM_AR_REASON_NONE);

			/* start data-status indication (runs parallel with in-data/offline) */
			cm_mc_ar_mcons_data_status_check (dev->ar_graph, dev->mcons);

			return;
		}
	}
	else { /* m-provider */

		if (dev->wait_for & CM_MWF__DO_START) {

			dev->wait_for &= ~CM_MWF__DO_START;

			dev->wait_for |= CM_MWF__IS_IN_DATA;

			cm_mc_device_event (dev, CM_OPC_MC_EVENT_AR_IN_DATA_IND, CM_AR_REASON_NONE);
			/* note: frames are sent only if prov-activate.cnf(+) */

			dev->wait_for |= CM_MWF__FRAMES_ACT_CNF;
			cm_arcq_request (argr, CM_ARCQ_OPC_PROV_ACTIVATE, 0);

			return;
		}
	}

	/***/

	if (dev->wait_for & CM_MWF__DO_FRAME_RTF_REQ) {

		dev->wait_for &= ~CM_MWF__DO_FRAME_RTF_REQ;

		/* note: doing "REMOVE_ACW" has no effect for RTC3 (but we keep it this way!) */

		dev->wait_for |= CM_MWF__WF_FRAME_RTF_CNF;
		cm_arcq_request (dev->ar_graph, CM_ARCQ_OPC_FRAMES_RTF, 0);
		return;
	}

	/* idle */

	if( (dev->wait_for & ~(CM_MWF__IS_PD_ATTACHED | CM_MWF__IS_RESET_TO_FACTORY | CM_MWF__WF_IRT_RUN | CM_MWF__IS_IN_DATA)) != 0) {
		CM_FATAL();
	}

	if( is_down ) {

		if( (dev->wait_for & ~(CM_MWF__IS_PD_ATTACHED | CM_MWF__IS_RESET_TO_FACTORY)) != 0) {
			CM_FATAL();
		}

		if( dev->state != CM_MST_REMOVING ) {

			cm_ar_com_idle(dev->ar_graph);
			cm_ar_com_ready(dev->ar_graph);

			if ( cm_is_not_null(dev->mcons) ) { /* m-consumer */
				const CM_MAC_TYPE null_mac = { {0, 0, 0, 0, 0, 0} };
				dev->ar_graph->ar.eth_mac = null_mac;
				dev->ar_graph->ar.host_ip = 0;
			}
		}

		if( dev->state == CM_MST_ACTIVE ) {

			if( (dev->wait_for & CM_MWF__IS_RESET_TO_FACTORY) != 0) {
				dev->state = CM_MST_PASSIVE; /* see CM_CL_DEVICE_CONTROL_PASSIVATE */
			}
			else if ( cm_is_not_null(dev->mcons) ) { /* m-consumer */
				/* note: pause too if was MISS (case: resolve ok -> IRT problem -> MISS within milliseconds) */
				cm_mc_mcons_pause(dev);
			}
			else { /* m-provider */
				/* note: come here if frame-activation failed (e.g., IRData mismatch) */
				/* note: set to passive state so that activate does something (which will fail again...) */
				dev->state = CM_MST_PASSIVE;
			}
		}
		else if( dev->state == CM_MST_PASSIVATING ) {

			if( cm_is_null(dev->upper_ctrl) ) {
				CM_FATAL();
			}
			else {
				dev->state = CM_MST_PASSIVE;
				cm_callback (dev->channel, CM_OK, & dev->upper_ctrl);
			}
		}
		else if( dev->state == CM_MST_REMOVING ) {

			if( cm_is_null(dev->upper_add_rm) ) {
				CM_FATAL();
			}
			else {
				cm_arcq_request(argr, CM_ARCQ_OPC_FRAMES_REMOVE, 0);
			}
		}
		else {
			CM_FATAL();
		}

		return;
	}
}


/*----------------------------------------------------------------------------*/
LSA_VOID
cm_mc_ar_com_down (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;

	CM_MC_TRACE_04 (dev->trace_idx, LSA_TRACE_LEVEL_WARN
		, "dev(%d) state(%d) wait_for(0x%x) reason(%d)"
		, dev->device_nr, dev->state, dev->wait_for, argr->ar_com.down
		);

	if (dev->wait_for & CM_MWF__DO_ABORT) {
		CM_FATAL();
	}
	else {
		dev->wait_for |= CM_MWF__DO_ABORT;

		dev->wait_for &= ~CM_MWF__DO_START;
		dev->wait_for &= ~CM_MWF__DO_RESOLVE;
		dev->wait_for &= ~CM_MWF__DO_PRE_DATA;
		dev->wait_for &= ~CM_MWF__DO_IN_DATA;
		dev->wait_for &= ~CM_MWF__WF_IRT_RUN;

		cm_mc_device_sm (dev);
	}
}


/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_ar_mprov_activate_done (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;

	CM_MC_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE
		, "dev(%d) state(%d) wait_for(0x%x)"
		, dev->device_nr, dev->state, dev->wait_for
		);

	if ((dev->wait_for & CM_MWF__FRAMES_ACT_CNF) == 0) {
		CM_FATAL();
	}
	else {
		dev->wait_for &= ~CM_MWF__FRAMES_ACT_CNF;

		cm_mc_device_sm(argr->cm.mc.dev);
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_ar_frames_passivate_done(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;

	if ((dev->wait_for & CM_MWF__FRAMES_PAS_CNF) == 0) {
		CM_FATAL();
	}
	else {
		dev->wait_for &= ~CM_MWF__FRAMES_PAS_CNF;

		dev->wait_for |= CM_MWF__DO_OFFLINE;

		cm_mc_device_sm(dev);
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID
cm_mc_ar_frames_rtf_done(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
	)
{
	CM_MC_DEVICE_PTR_TYPE  dev = argr->cm.mc.dev;

	if( (dev->wait_for & CM_MWF__WF_FRAME_RTF_CNF) == 0) {
		CM_FATAL();
	}
	else {
		dev->wait_for &= ~CM_MWF__WF_FRAME_RTF_CNF;

		if (dev->wait_for & CM_MWF__IS_PD_ATTACHED) {
			dev->wait_for &= ~CM_MWF__IS_PD_ATTACHED;
			cm_pdusr_detach (dev->channel);
		}

		cm_mc_pdev_set_down_check(dev->channel);

		cm_mc_device_sm(dev);
	}
}

/*----------------------------------------------------------------------------*/
CM_AR_GRAPH_PTR_TYPE
cm_mc_arcq_request_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode,
	LSA_UINT16  arcq_para1
	)
{
	switch (arcq_opcode)
	{
	case CM_ARCQ_OPC_FRAMES_ADD:
		cm_mc_ar_frames_add_done (argr);
		break;

	case CM_ARCQ_OPC_PROV_ACTIVATE:
		cm_mc_ar_mprov_activate_done (argr);
		break;

	case CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK:
		cm_mc_ar_mcons_activate_done (argr);
		break;

	case CM_ARCQ_OPC_FRAMES_PASSIVATE:
		cm_mc_ar_frames_passivate_done (argr);
		break;

	case CM_ARCQ_OPC_FRAMES_RTF:
		cm_mc_ar_frames_rtf_done (argr);
		break;

	case CM_ARCQ_OPC_FRAMES_REMOVE:
		argr = cm_mc_ar_frames_remove_done (argr);
		break;

	case CM_ARCQ_OPC_PROV_DATASTATUS_SET:
		LSA_UNUSED_ARG(arcq_para1); /* currently not used */
		cm_mc_ar_mprov_data_status_control_done(argr);
		break;

	default:
		CM_FATAL1 (arcq_opcode);
		return argr;
	}

	return argr;
}

/*----------------------------------------------------------------------------*/
#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling multicast channel part (CM_CFG_MAX_MULTICAST_CHANNELS=" CM_STRINGIFY(CM_CFG_MAX_MULTICAST_CHANNELS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
