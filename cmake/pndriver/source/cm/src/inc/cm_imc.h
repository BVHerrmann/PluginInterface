#ifndef CM_IMC_H
#define CM_IMC_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: cm_imc.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - multicast things                                  */
/*                                                                           */
/*****************************************************************************/

/*----------------------------------------------------------------------------*/

/* MDST .. Multicast-Device-STate */
#define CM_MST_ADDING		0
#define CM_MST_PASSIVE		1
#define CM_MST_ACTIVE		2
#define CM_MST_PASSIVATING	3
#define CM_MST_REMOVING		4


/* MWF .. Multicast-Wait-For */
#define CM_MWF__IS_PD_ATTACHED			0x00000001
#define CM_MWF__DO_START				0x00000002
#define CM_MWF__DO_ABORT				0x00000004
#define CM_MWF__DO_RESOLVE				0x00000008
#define CM_MWF__DO_PRE_DATA				0x00000010
#define CM_MWF__DO_IN_DATA				0x00000020
#define CM_MWF__DO_OFFLINE				0x00000040
#define CM_MWF__IS_IN_DATA				0x00000080
#define CM_MWF__FRAMES_ACT_CNF			0x00000100
#define CM_MWF__FRAMES_PAS_CNF			0x00000200
#define CM_MWF__IN_DATA_RSP				0x00000400
#define CM_MWF__OFFLINE_RSP				0x00000800
#define CM_MWF__ADDR_INFO_RSP			0x00001000
#define CM_MWF__APDU_STATUS_RSP			0x00002000
#define CM_MWF__WF_IRT_RUN				0x00004000
#define CM_MWF__NARE_CNF				0x00008000
#define CM_MWF__NARE_PAUSE				0x00010000
#define CM_MWF__NARE_CANCEL				0x00020000

#define CM_MWF__IS_RESET_TO_FACTORY		0x10000000 /* keep until device_remove */
#define CM_MWF__DO_FRAME_RTF_REQ		0x20000000
#define CM_MWF__WF_FRAME_RTF_CNF		0x40000000

/*----------------------------------------------------------------------------*/

typedef struct cm_mc_device_tag {
	LSA_UINT32				trace_idx; /* for "LSA index trace" */

	CM_CHANNEL_PTR_TYPE		channel; /* backlink to channel */

	LSA_UINT16				device_nr; /* key of this entry, given by add */

	CM_AR_GRAPH_IO_CR_PTR_TYPE	mcons; /* if mcons: ptr to its iocr; if mprov: LSA_NULL */

	LSA_UINT8				state; /* state of device, see enum CM_MC_DEV_STATE_TYPE */
	LSA_UINT32				wait_for; /* flag wf cm-user action / abort actions / lower cnf */

	CM_AR_GRAPH_PTR_TYPE	ar_graph;

	CM_LIST_ENTRY_TYPE		event_q; /* pool of notification resources */

	CM_UPPER_RQB_PTR_TYPE	upper_add_rm; /* current upper rqb for: CM_OPC_MC_DEVICE_ADD, CM_OPC_MC_DEVICE_REMOVE */
	CM_UPPER_RQB_PTR_TYPE	upper_ctrl; /* current upper rqb for: CM_CL_DEVICE_CONTROL_ACTIVATE / CM_CL_DEVICE_CONTROL_PASSIVATE */
	CM_UPPER_RQB_PTR_TYPE	upper_apdu; /* current upper rqb for: CM_MC_DEVICE_CONTROL_APDU_STATUS */
	CM_NARE_LOWER_RQB_PTR_TYPE	nare_ident; /* pre-allocated RQB for ident */
	CM_NARE_LOWER_RQB_PTR_TYPE	nare_cancel; /* pre-allocated RQB for ident-cancel */

	CM_ONESHOT_TYPE mcons_timer; /* timer, m-consumer only */

	LSA_UINT8 last_reason_code; /* last reason_code indicated to the user */

} CM_MC_DEVICE_TYPE;

typedef CM_MC_DEVICE_TYPE * CM_MC_DEVICE_PTR_TYPE;

typedef CM_MC_DEVICE_TYPE const * CM_MC_DEVICE_CONST_PTR_TYPE;

/*----------------------------------------------------------------------------*/

/* channel-wait-for */
#define CM_MC_CWF__RESET_TO_FACTORY_CNF		0x0001

typedef struct cm_mc_channel_tag {
	CM_UPPER_RQB_PTR_TYPE	upper;			/* current upper create / delete rqb */
	CM_UPPER_RQB_PTR_TYPE	upper_apdu;		/* current clear / operate rqb */

		/* anchor of devices */
	CM_MC_DEVICE_PTR_TYPE	* dev_arr;		/* pointer to array of dev-pointers, see CM_OPC_MC_CREATE */
	LSA_UINT16				dev_cnt;		/* CM_OPC_MC_CREATE::max_devices */

		/* pd */
	LSA_UINT16				wait_for;		/* channel-wait-for */

	CM_EDD_LOWER_RQB_PTR_TYPE edd_rqb;		/* for EDD_SRV_SRT_PROVIDER_SET_STATE */

} CM_MC_CHANNEL_TYPE;


/*----------------------------------------------------------------------------*/

LSA_VOID
cm_mc_request (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_mc_device_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_mc_device_control (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_mc_device_remove (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

CM_AR_GRAPH_PTR_TYPE
cm_mc_arcq_request_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode,
	LSA_UINT16  arcq_para1
);

LSA_VOID
cm_mc_device_any_port_irt_run(
	CM_CHANNEL_CONST_PTR_TYPE channel
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_mc_opc_device_provide_event (
	CM_CHANNEL_CONST_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID
cm_mc_ar_addr_info_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_mc_ar_in_data_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_mc_ar_offline_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID
cm_mc_apdu_status_rsp (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_mc_acp_global_provider_control_done(
	CM_CHANNEL_PTR_TYPE channel,
	CM_EDD_LOWER_RQB_CONST_PTR_TYPE  edd
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_mc_mcons_pause(
	CM_MC_DEVICE_PTR_TYPE  dev
);

LSA_VOID
cm_mc_mcons_timeout(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE   user_ptr
);

LSA_VOID
cm_mc_mcons_ident(
	CM_MC_DEVICE_PTR_TYPE  dev
);

LSA_VOID
cm_mc_mcons_ident_done(
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
);

LSA_VOID
cm_mc_mcons_ident_cancel(
	CM_MC_DEVICE_PTR_TYPE dev
);

LSA_VOID
cm_mc_mcons_ident_cancel_done(
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr,
	CM_NARE_LOWER_RQB_CONST_PTR_TYPE nare
);

LSA_VOID
cm_mc_ar_mcons_activate_done(
	CM_AR_GRAPH_CONST_PTR_TYPE argr
);

LSA_VOID
cm_mc_ar_mcons_up(
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
);

LSA_VOID
cm_mc_ar_mcons_data_status_check(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_CONST_PTR_TYPE  io_cr
);

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_mc_ar_com_down (
	CM_AR_GRAPH_CONST_PTR_TYPE  argr
);

/*----------------------------------------------------------------------------*/

CM_MC_DEVICE_PTR_TYPE
cm_mc_dev_from_device_nr (
	CM_CHANNEL_CONST_PTR_TYPE  channel,
	LSA_UINT16  device_nr
);

LSA_VOID
cm_mc_device_sm (
	CM_MC_DEVICE_PTR_TYPE  dev
);

CM_UPPER_RQB_PTR_TYPE
cm_mc_dev_event_get(
	CM_MC_DEVICE_CONST_PTR_TYPE dev
);

/*----------------------------------------------------------------------------*/

LSA_BOOL
cm_mc_pdev_set_down_ind (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT8 reason /* CM_AR_REASON_DCP_RESET or CM_AR_REASON_DCP_NAME or CM_AR_REASON_IPSUITE */
);

LSA_VOID
cm_mc_pdev_set_down_check (
	CM_CHANNEL_PTR_TYPE channel
);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_IMC_H */
