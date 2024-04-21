#ifndef CM_ISVAR_SR_H
#define CM_ISVAR_SR_H

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
/*  F i l e               &F: cm_isvar_sr.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal header file - system redundancy server ar-graph things          */
/*                                                                           */
/*****************************************************************************/

struct cm_ar_graph_tag; /* forward (for iterator) */

/*===========================================================================*/
/*=== R1_FLAGS ==============================================================*/
/*===========================================================================*/

/*** defines for argr->cm.sv.r1_flags ***/

#define CM_SV_AR_R1FL_ORIGINAL                   0x00000001 /* AR is the "original AR" (ORIG) */
#define CM_SV_AR_R1FL_REPRESENTATIVE             0x00000002 /* AR is the "representative AR" (REPR) */
#define CM_SV_AR_R1FL_WF_CONN_POSTED             0x00000004 /* ORIG: wait for "connect posted" */
#define CM_SV_AR_R1FL_EX_CONN_DONE               0x00000008 /* ORIG: expect "connect-done" from other side, or "bridge-broken" */
#define CM_SV_AR_R1FL_WF_CONN_DONE_POSTED        0x00000010 /* REPR: wait for "connect-done posted" */
#define CM_SV_AR_R1FL_DO_DISCONN                 0x00000020 /* ORIG: posted "connect" to other side, thus must do disconnect */
#define CM_SV_AR_R1FL_WF_DISCONN_POSTED          0x00000040 /* ORIG: wait for "disconnect posted" */
#define CM_SV_AR_R1FL_EX_DISCONN_DONE            0x00000080 /* ORIG: expect "disconnect-done" from other side, or "bridge-broken" */
#define CM_SV_AR_R1FL_WF_SUBSTATE_POSTED         0x00000100 /* ORIG: wait for "sub-state posted" */
#define CM_SV_AR_R1FL_DO_DISCONN_DONE            0x00000200 /* REPR: got MSG_DISCONNECT, must reply MSG_DISCONNECT_DONE */
#define CM_SV_AR_R1FL_WF_DISCONN_DONE_POSTED     0x00000400 /* REPR: wait for "disconnect-done posted" */
#define CM_SV_AR_R1FL_WF_PRM_END_REMOTE          0x00000800 /* ORIG: wait for "prm-end-remote confirmation", or cancellation via bridge-broken */
#define CM_SV_AR_R1FL_WF_PRM_BEGIN_REMOTE        0x00001000 /* ORIG: wait for "prm-begin-remote confirmation", or cancellation via bridge-broken */
#define CM_SV_AR_R1FL_EX_DISCONN                 0x00002000 /* REPR: AR must not be killed by cm_ar_com_down() except when caller is MSG_DISCONNECT, or bridge "broken" */
#define CM_SV_AR_R1FL_WF_UPDATE_CONN_POSTED      0x00004000 /* ORIG: wait for "update-connect posted" */
#define CM_SV_AR_R1FL_WF_UPDATE_SUBSTATE_POSTED  0x00008000 /* ORIG: wait for "update-sub-state posted" */
#define CM_SV_AR_R1FL_REMOTE_RTF                 0x00010000 /* the other side was RTF-ed */

/*===========================================================================*/
/*=== R1-BRIDGE MESSAGE NUMBERS =============================================*/
/*===========================================================================*/

#define CM_SV_R1BRIDGE_MSG_RESERVED_ZERO             0 /* note: may be used for debugging to trigger fatal */
#define CM_SV_R1BRIDGE_MSG_CONNECT                   1
#define CM_SV_R1BRIDGE_MSG_CONNECT_DONE              2
#define CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT         3
#define CM_SV_R1BRIDGE_MSG_DISCONNECT                4
#define CM_SV_R1BRIDGE_MSG_DISCONNECT_DONE           5
#define CM_SV_R1BRIDGE_MSG_UPDATE_BEGIN              6
#define CM_SV_R1BRIDGE_MSG_UPDATE_DIAG               7
#define CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT            8
#define CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE          9
#define CM_SV_R1BRIDGE_MSG_UPDATE_ALARMS            10
#define CM_SV_R1BRIDGE_MSG_UPDATE_END               11
#define CM_SV_R1BRIDGE_MSG_ALARM_BEGIN              12
#define CM_SV_R1BRIDGE_MSG_ALARM_END                13
#define CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PLUG_GROUP   14
#define CM_SV_R1BRIDGE_MSG_ALARM_END_PLUG_GROUP     15
#define CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PDEV_GROUP   16
#define CM_SV_R1BRIDGE_MSG_ALARM_END_PDEV_GROUP     17
#define CM_SV_R1BRIDGE_MSG_DIAG_CHANGED             18
#define CM_SV_R1BRIDGE_MSG_DIAG_REPLACE_PDEV        19 /* note: because of the semantics named "REPLACE" and not "CHANGED" */
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ            20
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ_DONE       21
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE           22
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE_DONE      23
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN           24
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN_DONE      25 /* note: message is necessary for flow-control */
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_END             26
#define CM_SV_R1BRIDGE_MSG_PDEV_PRM_END_DONE        27
#define CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_SET    28 /* note: used for "update" too */
#define CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CLR    29
#define CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CANCEL 30
#define CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS          31
#define CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS_DONE     32
#define CM_SV_R1BRIDGE_MSG_RESET_TO_FACTORY         33
#define CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END  34

/*===========================================================================*/
/*=== R1-BRIDGE PREFIX ======================================================*/
/*===========================================================================*/

typedef struct cm_sv_r1bmsg_prefix_tag { /* a queueable R1BRIDGE message, see cm_sv_r1b_send() */

	/* see cm_sv_r1b_prefix_alloc() and cm_sv_r1b_prefix_free() */
	/* see cm_sv_r1b_get_message_body_from_prefix() */

	CM_LIST_ENTRY_TYPE link; /* first in struct! */

	CM_COMMON_MEM_U8_PTR_TYPE message_hdr; /* pointer to message-buffer */
	LSA_UINT32 message_max; /* allocated size of "message_ptr", without header offset, see cm_sv_r1b_prefix_alloc() */

	LSA_UINT16 session_key;  /* zero, or the AR (quasi a copy of CM_SV_R1BRIDGE_MSG_HEADER::aruuid) */
	LSA_UINT16 alarm_prio;   /* zero, or the prio of ALMI of the AR */
	LSA_UINT16 message_numb; /* copy of CM_SV_R1BRIDGE_MSG_HEADER::message_numb */
	LSA_UINT32 message_size; /* copy of CM_SV_R1BRIDGE_MSG_HEADER::message_size */

} CM_SV_R1BMSG_PREFIX_TYPE;

typedef CM_SV_R1BMSG_PREFIX_TYPE * CM_SV_R1BMSG_PREFIX_PTR_TYPE;

typedef CM_SV_R1BMSG_PREFIX_TYPE const * CM_SV_R1BMSG_PREFIX_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== R1-BRIDGE MESSAGE =====================================================*/
/*===========================================================================*/

/*
 * a R1-bridge message consists of a message header CM_SV_R1BRIDGE_MSG_HEADER (fixed size CM_SV_R1BMSG_SIZE_HEADER)
 * followed by the message body CM_SV_R1BRIDGE_MSG_XXX (variable size)
 *
 * the maximum size of the message (header + body) is CM_SV_DEVICE_ADD_TYPE::R1_bridge_max
 */

/* CM_SV_R1BRIDGE_MSG_HEADER
 * offset   type   field
 * [0]      U32    message_size // size of body
 * [4]      U16    message_numb // see CM_SV_R1BRIDGE_MSG_XXX...
 * [6]      UUID   ar_uuid
 */
#define CM_SV_R1BMSG_SIZE_HEADER 22U /* not in parenthesis! VxWorks compiler bug (TIA 1511918) */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_CONNECT and CM_SV_R1BRIDGE_MSG_UPDATE_CONNECT
 * offset   type   field
 * [0]      U16    alarm_ref
 * [2]      U16    nr_of_cons (unicast consumers)
 * [4] ARRAY follows
 * [+][0]   U16    iocr_ref
 * [+][2]   U16    frame_id
 * [x]      OCT    connect-PDU (number of octets: message_size - (4 + nr_of_cons * 4))
 */
#define CM_SV_R1BMSG_SIZE_CONNECT(nr_of_cons) (4U + (nr_of_cons) * 4U) /* + connect-PDU */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_CONNECT_DONE
 * offset   type   field
 * [0]      U32    pnio_err
 */
#define CM_SV_R1BMSG_SIZE_CONNECT_DONE (4U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_SUB_STATE_CONNECT, CM_SV_R1BRIDGE_MSG_UPDATE_SUB_STATE and CM_SV_R1BRIDGE_MSG_SUB_STATE_PRM_BEGIN_END
 * offset   type   field
 * [0]      U16    sub_state_count
 * [2] ARRAY follows
 * [+][0]   U32    api
 * [+][4]   U16    slot_nr
 * [+][6]   U16    subslot_nr
 * [+][8]   U16    st_told (all submodule-state bits; as in ModuleDiffBlock)
 * [+][10]  U16    st_curr (only the bits of CM_SUB_STATE_IDENT_MASK and CM_SUB_STATE_AR_INFO_MASK, CM_SUB_STATE_FORMAT_1 = dirty)
 */
#define CM_SV_R1BMSG_SIZE_SUB_STATE (2U)
#define CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM (12U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_DISCONNECT
 * offset   type   field
 * [0]      U8     reason
 */
#define CM_SV_R1BMSG_SIZE_DISCONNECT (1U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_DISCONNECT_DONE
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_DISCONNECT_DONE (0U) /* no parameters */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_UPDATE_BEGIN
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_UPDATE_BEGIN (0U) /* no header, body: pdev-diagnoses */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_UPDATE_DIAG
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_UPDATE_DIAG (0U) /* no header, body: diagnoses (pdev and/or user) */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_UPDATE_ALARMS
 * offset   type   field
 * [0]      U16    alarm_sequence low
 * [2]      U16    alarm_sequence high
 * [4] alarm low (optional!)
 * [+][0]   U16    alarm_priority
 * [+][2]   U16    alarm_sequence
 * [+][4]   U32    api
 * [+][8]   U16    slot_nr
 * [+][10]  U16    subslot_nr
 * [+][12]  U16    is_pull_module
 * [+][14]  U16    is_pdev_alarm
 * [+][16]  U16    alarm_type
 * [+][18]  U16    alarm_tag
 * [+][20]  U16    alarm_data_length
 * [+][22]  OCT    alarm_data[alarm_data_length]
 * [x] alarm high (optional!)
 * [+][0] see "alarm low"
 */
#define CM_SV_R1BMSG_SIZE_UPDATE_ALARMS (4U)
#define CM_SV_R1BMSG_SIZE_UPDATE_ALARMS_ELEM (22U) /* + alarm_data_length */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_UPDATE_END
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_UPDATE_END (0U) /* no parameters */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_ALARM_BEGIN and CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PDEV_GROUP
 * offset   type   field
 * [0]      U16    prio
 * [2]      U32    api
 * [6]      U16    slot_nr
 * [8]      U16    subslot_nr
 * [10]     U16    alarm_type
 * [12]     U16    alarm_sequence
 * [14]     U16    alarm_tag
 * [16]     U16    alarm_data_length
 * [18]     OCT    alarm_data (number of octets: alarm_data_length)
 */
#define CM_SV_R1BMSG_SIZE_ALARM_BEGIN (18U) /* + alarm_data_length */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_ALARM_END and CM_SV_R1BRIDGE_MSG_ALARM_END_PDEV_GROUP
 * offset   type   field
 * [0]      U16    prio
 * [2]      U16    alarm_sequence
 */
#define CM_SV_R1BMSG_SIZE_ALARM_END (4U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_ALARM_BEGIN_PLUG_GROUP
 * offset   type   field
 * [0]      U16    prio
 * [2]      U32    api
 * [6]      U16    slot_nr
 * [8]      U16    subslot_nr
 * [10]     U16    alarm_type
 * [12]     U16    is_pull_module
 * [14]     U16    alarm_sequence
 */
#define CM_SV_R1BMSG_SIZE_ALARM_BEGIN_PLUG_GROUP (16U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_ALARM_END_PLUG_GROUP
 * offset   type   field
 * [0]      U16    prio
 * [2]      U16    alarm_sequence
 * [4]      U16    alarm_type
 * [6]      U16    st_told
 * [8]      U16    st_user
 */
#define CM_SV_R1BMSG_SIZE_ALARM_END_PLUG_GROUP (10U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_DIAG_CHANGED
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_DIAG_CHANGED (0U) /* no header, body: diagnoses */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_DIAG_REPLACE_PDEV
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_DIAG_REPLACE_PDEV (0U) /* no header, body: diagnoses */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ
 * offset   type   field
 * [0]      U16    user_id
 * [2]      U16    slot_nr
 * [4]      U16    subslot_nr
 * [6]      U32    record_index (see PNIO-Spec)
 * [10]     U16    record_data_length
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_READ (12U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_READ_DONE
 * offset   type   field
 * [0]      U16    user_id
 * [2]      U16    record_data_length
 * [4]      U16    r1_response
 * [6]      OCT    record_data (number of octets: record_data_length)
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_READ_DONE (6U) /* body: record_data */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE
 * offset   type   field
 * [0]      U16    user_id
 * [2]      U16    slot_nr
 * [4]      U16    subslot_nr
 * [6]      U32    record_index (see PNIO-Spec)
 * [10]     U16    record_data_length
 * [12]     U16    mk_remanent
 * [14]     OCT    record_data (number of octets: record_data_length)
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE (14U) /* body: record_data */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_WRITE_DONE
 * offset   type   field
 * [0]      U16    user_id
 * [2]      U16    r1_response
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_WRITE_DONE (4U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN
 * offset   type   field
 * [0]      U16    user_id
 * [2]      U16    slot_nr
 * [4]      U16    subslot_nr
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN (6U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_BEGIN_DONE
 * offset   type   field
 * [0]      U16    user_id
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_BEGIN_DONE (2U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_END
 * offset   type   field
 * [0]      U16    user_id
 * [2]      U16    max_alarm_data_length
 * [4]      U16    slot_nr
 * [6]      U16    subslot_nr
 * [8]      U16    nr_of_elems
 * [10] ARRAY follows
 * [+][0]   U16    slot_nr
 * [+][2]   U16    subslot_nr
 * [+][4]   U32    index
 * [+][8]   U32    offset
 * [+][12]  U16    r1_code_flags
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_END (10U)
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_END_ELEM (14U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_PRM_END_DONE
 * offset   type   field
 * [0]      U16    user_id
 * [2]      U16    r1_response
 */
#define CM_SV_R1BMSG_SIZE_PDEV_PRM_END_DONE (4U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_SET
 * offset   type   field
 * [0]      U16    slot_nr
 * [2]      U16    subslot_nr
 * [4]      U16    alarm_type
 * [6]      U16    alarm_tag
 * [8]      U16    alarm_data_length
 * [10]     OCT    alarm_data (number of octets: alarm_data_length)
 */
#define CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET (10U) /* + alarm_data_length */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CLR
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CLR (0U) /* no parameters */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_PDEV_ALARM_MIRROR_CANCEL
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_CANCEL (0U) /* no parameters */

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS
 * offset   type   field
 * [0]      U16    iocr_ref
 * [2]      UUID   ar_uuid
 */
#define CM_SV_R1BMSG_SIZE_GET_APDU_STATUS (18U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_GET_APDU_STATUS_DONE
 * offset   type   field
 * [0]      U16    cycle_counter
 * [2]      U8     data_status
 * [3]      U8     transfer_status
 */
#define CM_SV_R1BMSG_SIZE_GET_APDU_STATUS_DONE (4U)

/*-------------------------------------------------------------------
 * CM_SV_R1BRIDGE_MSG_RESET_TO_FACTORY
 * offset   type   field
 * --no parameters--
 */
#define CM_SV_R1BMSG_SIZE_RESET_TO_FACTORY (0U) /* no parameters */

/*===========================================================================*/
/*=== R1-BRIDGE ARGR R1B TYPE ===============================================*/
/*===========================================================================*/

typedef struct cm_sv_argr_r1b_tag {

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_connect; /* ORIG */

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_connect_done; /* REPR */

	#define CM_SV_R1B_SUB_STATE_MESSAGE_MAX(argr)  (CM_SV_R1BMSG_SIZE_SUB_STATE + ((argr)->cm.sv.nr_of_submodules) * CM_SV_R1BMSG_SIZE_SUB_STATE_ELEM)

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_sub_state; /* ORIG */

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_update_sub_state; /* ORIG */

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_disconnect; /* ORIG */

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_disconnect_done; /* REPR */

	/***/

	CM_UPPER_RQB_PTR_TYPE remote_pdev_begin_rqb; /* ORIG, for remote prm-begin */

	CM_UPPER_RQB_PTR_TYPE remote_pdev_end_rqb; /* ORIG, for remote prm-end */

	CM_PD_AR_PRM_END_63P_PTR_TYPE remote_prm_end_args; /* ORIG, "pre-allocated" parameters for remote_pdev_end_rqb */

} CM_SV_ARGR_R1B_TYPE;

/*===========================================================================*/
/*=== R1 ALARM CURR TYPE ====================================================*/
/*===========================================================================*/

typedef struct cm_sv_r1_alarm_curr {

	LSA_BOOL is_used;
	LSA_BOOL is_pdev_alarm;
	LSA_BOOL is_pull_module; /* pull alarm only */

	LSA_UINT32 api;
	LSA_UINT16 slot_nr;
	LSA_UINT16 subslot_nr;

	LSA_UINT16 alarm_type;
	LSA_UINT16 alarm_sequence;
	LSA_UINT16 alarm_tag;
	LSA_UINT16 alarm_data_length;
	LSA_UINT8  alarm_data[CM_ALARM_PAYLOAD_MAX_woUSI]; /* note: USI is stored in "alarm_tag" */

} CM_SV_R1_ALARM_CURR_TYPE;

typedef CM_SV_R1_ALARM_CURR_TYPE *CM_SV_R1_ALARM_CURR_PTR_TYPE;

/*===========================================================================*/
/*=== R1-BRIDGE ALMI R1B TYPE ===============================================*/
/*===========================================================================*/

typedef struct cm_sv_ar_almi_r1b_tag {

	#define CM_SV_R1B_ALARM_BEGIN_MESSAGE_MAX (CM_SV_R1BMSG_SIZE_ALARM_BEGIN + CM_ALARM_PAYLOAD_MAX_woUSI)
	#define CM_SV_R1B_ALARM_END_MESSAGE_MAX   (CM_SV_R1BMSG_SIZE_ALARM_END)

	#define CM_SV_R1B_ALARM_BEGIN_PLUG_GROUP_MESSAGE_MAX (CM_SV_R1BMSG_SIZE_ALARM_BEGIN_PLUG_GROUP)
	#define CM_SV_R1B_ALARM_END_PLUG_GROUP_MESSAGE_MAX   (CM_SV_R1BMSG_SIZE_ALARM_END_PLUG_GROUP)

	#define CM_SV_R1B_ALARM_BEGIN_END_MESSAGE_MAX ( \
			LSA_MAX( \
				LSA_MAX(CM_SV_R1B_ALARM_BEGIN_MESSAGE_MAX, CM_SV_R1B_ALARM_END_MESSAGE_MAX), \
				LSA_MAX(CM_SV_R1B_ALARM_BEGIN_PLUG_GROUP_MESSAGE_MAX, CM_SV_R1B_ALARM_END_PLUG_GROUP_MESSAGE_MAX) \
			))

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_alarm_begin_end; /* R1B message for alarm-begin and alarm-end */

} CM_SV_AR_ALMI_R1B_TYPE;

/*===========================================================================*/
/*=== R1-BRIDGE PD-ALARM TYPE ===============================================*/
/*===========================================================================*/

typedef struct cm_sv_arset_r1b_pd_alarm_tag {

	#define CM_SV_R1B_PDEV_ALARM_MIRROR_SET_MESSAGE_MAX (CM_SV_R1BMSG_SIZE_PDEV_ALARM_MIRROR_SET + CM_ALARM_PAYLOAD_MAX_woUSI)

	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_mirror_set;    /* pre-allocated R1B message buffer for "pdev alarm mirror set" */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_mirror_cancel; /* pre-allocated R1B message buffer for "pdev alarm mirror cancel" */
	CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_mirror_clr;    /* pre-allocated R1B message buffer for "pdev alarm mirror clear" */

	LSA_BOOL wf_set_posted;    /* true if waiting for "posted" ("buffer" is on-the-fly) */
	LSA_BOOL wf_cancel_posted; /* true if waiting for "posted" ("buffer" is on-the-fly) */
	LSA_BOOL wf_clr_posted;    /* true if waiting for "posted" ("buffer" is on-the-fly) */

	CM_UPPER_RQB_PTR_TYPE orig; /* pdev alarm RQB, originating side (RQB from CM-PD) */
	CM_UPPER_RQB_PTR_TYPE mirr; /* pdev alarm RQB, representative side (pre-allocated), see cm_sv_r1b_handle_pdev_alarm_mirror_set() */

	LSA_BOOL orig_is_completed; /* true: orig is completed, no resubmission, see cm_sv_r1b_pdev_alarm_mirror_join_done() */
	LSA_BOOL mirr_is_completed; /* true: mirr is completed, waiting for mirror-clr which deletes mirr */

} CM_SV_ARSET_R1B_PD_ALARM_TYPE;

typedef CM_SV_ARSET_R1B_PD_ALARM_TYPE *CM_SV_ARSET_R1B_PD_ALARM_PTR_TYPE;

/*===========================================================================*/
/*=== ARSET R1B TYPE ========================================================*/
/*===========================================================================*/

typedef struct cm_sv_arset_r1b_tag {

	LSA_BOOL wait_for_primary_trigger;
		/* the flag solves the race between alarm-resubmission and SetDataStatus(ARID).
		 * user first posts alarm RQBs (0-n), then calls SetDataStatus() sync, then posts ARSet-Trigger(primary).
		 * race: CM detects the new primary-AR before the resubmitted alarm RQB is processed (alarm matching; sequence number).
		 */

	struct {

		CM_SV_R1_ALARM_CURR_TYPE curr[2]; /* index = priority, 0=low 1=high */

	} alarm;

	CM_SV_ARSET_R1B_PD_ALARM_TYPE pd_alarm;

} CM_SV_ARSET_R1B_TYPE;

typedef CM_SV_ARSET_R1B_TYPE *CM_SV_ARSET_R1B_PTR_TYPE;

/*===========================================================================*/
/*=== ARSET TYPE ============================================================*/
/*===========================================================================*/

#define CM_SV_ARSET_MAX_ARS  2 /* ARUUID::Selector[Bits 3-4], "...allocate two ARs for the AR-set" */

typedef struct cm_sv_arset_tag {

	LSA_UINT16 arset_nr;

	LSA_BOOL is_aborting; /* true if the last AR of the AR-set is aborting */

	/*CM_SV_AR_GRAPH_PTR_TYPE*/struct cm_ar_graph_tag * arr[CM_SV_ARSET_MAX_ARS]; /* the ARs of the AR-set */

	CM_UPPER_RQB_PTR_TYPE abort_rqb;

	/***/

	/*CM_AR_GRAPH_PTR_TYPE*/struct cm_ar_graph_tag * first;

	/*CM_AR_GRAPH_PTR_TYPE*/struct cm_ar_graph_tag * primary;

	struct {

		LSA_UINT16 sequence_number;

		CM_LIST_ENTRY_TYPE queue;

	} alarm[2]; /* index = priority, 0=low 1=high */

	/***/

	CM_SV_ARSET_R1B_TYPE r1b; //R1 marker (AR-specific data for R1)

} CM_SV_ARSET_TYPE;

typedef CM_SV_ARSET_TYPE *CM_SV_ARSET_PTR_TYPE;

typedef CM_SV_ARSET_TYPE const *CM_SV_ARSET_CONST_PTR_TYPE;

/*===========================================================================*/
/*=== R1 APDU STAT REQUEST TYPE =============================================*/
/*===========================================================================*/

typedef struct cm_sv_r1_apdu_stat_request_tag {

	CM_LIST_ENTRY_TYPE  link;     /* first in struct! */

	LSA_VOID_PTR_TYPE   user_id;  /* Iterator with CM_PD_ITER_PDEV_ARData_GetAPDUStatus */
	CLRPC_UUID_PTR_TYPE ar_uuid;  /* UUID of the representative AR */
	LSA_UINT16          iocr_ref; /* IOCRReference */

} CM_SV_R1_APDU_STAT_REQUEST_TYPE;

typedef CM_SV_R1_APDU_STAT_REQUEST_TYPE *CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE;

/*===========================================================================*/
/*=== DEVICE R1B TYPE =======================================================*/
/*===========================================================================*/

#define CM_SV_DEV_R1B_STATE_UNKNOWN              0 /* startup, reject any rpc-connect */
#define CM_SV_DEV_R1B_STATE_BROKEN               1
#define CM_SV_DEV_R1B_STATE_ESTABLISHING_ACTIVE  2 /* temporary state for "update" */
#define CM_SV_DEV_R1B_STATE_ESTABLISHING_PASSIVE 3 /* temporary state for "update" */
#define CM_SV_DEV_R1B_STATE_ESTABLISHED          4

#define CM_SV_R1B_PDEV_B_AR_NR (222) /* see usage in "pdev_B" */

#if CM_SV_R1B_PDEV_B_AR_NR <= CM_CFG_MAX_SV_ARS
#error "check this!"
#endif

typedef struct cm_sv_device_r1b_tag {

	LSA_UINT16 bridge_state; /* CM_SV_DEV_R1B_STATE_UNKNOWN, etc. */

	/***/

	struct {

		CM_UPPER_RQB_PTR_TYPE bridge_broken_rqb; /* stores the bridge-control(broken) while cancellation is in progress */
		CM_UPPER_RQB_PTR_TYPE device_remove_rqb; /* stores the device-remove request while bridge-control(broken) is in progress (TIA 1502236) */

	} control;

	/***/

	struct {

		CM_UPPER_RQB_PTR_TYPE resource; /* one indication resource for CM_OPC_SV_R1BRIDGE_SEND_PROVIDE */
		CM_LIST_ENTRY_TYPE    queue;    /* queue of CM_SV_R1BMSG_PREFIX_TYPE */

	} send;

	/***/

	struct {

		#define CM_SV_R1B_UPD_ALARMS_MESSAGE_MAX (CM_SV_R1BMSG_SIZE_UPDATE_ALARMS + 2 * (CM_SV_R1BMSG_SIZE_UPDATE_ALARMS_ELEM + CM_ALARM_PAYLOAD_MAX_woUSI))

		CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_begin_and_diag; /* R1B message for update begin and update diag */
		CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_alarms;         /* R1B message for update alarms */
		CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_end;            /* R1B message for update end */

	} upd;

	/***/

	CM_SV_R1BMSG_PREFIX_PTR_TYPE proxy_connect_done; /* R1B message for "connect_done(-)" if representative-AR could not be created */

	/***/

	struct {

		CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_user_diag; /* pre-allocated R1B message buffer for user diag */
		CM_SV_R1BMSG_PREFIX_PTR_TYPE msg_pdev_diag; /* pre-allocated R1B message buffer for pdev diag */

		CM_LIST_ENTRY_TYPE    queue;
		CM_UPPER_RQB_PTR_TYPE current;

		LSA_UINT16 curr_elem_idx; /* index of next element to marshal in cm_sv_r1b_diag_post_next() if diag elements of "current" are distributed over multiple bridge messages */
		LSA_BOOL update_pending; /* not all submodules diagnosis have yet been sent to other side; see cm_sv_r1b_diag_list_to_message() */

		LSA_BOOL pdev_diag_posted; /* LSA_TRUE: buffer "msg_pdev_diag" is posted, wait for posted-callback */
		LSA_BOOL pdev_diag_again;  /* LSA_TRUE: pdev-diag-changed while posted, transfer diagnoses again */

	} diag;

	/***/

	struct { /* initiator: "A" talks to "B" */

		CM_SV_R1BMSG_PREFIX_PTR_TYPE buffer; /* pre-allocated R1B message buffer (for all messages) */

		CM_LIST_ENTRY_TYPE queue; /* queue of CM_OPC_PD_xxx_SERVER requests */
		CM_UPPER_RQB_PTR_TYPE pd; /* current PDEV request */
		LSA_UINT16 user_id;       /* ID of current request (to match the done-message) */
		LSA_BOOL wf_posted;       /* true if waiting for "posted" ("buffer" is on-the-fly) */

	} pdev_A;

	/***/

	struct { /* responder: "B" listens to "A" and responds */

		CM_SV_R1BMSG_PREFIX_PTR_TYPE buffer; /* pre-allocated R1B message buffer (for all messages) */
		CM_UPPER_RQB_PTR_TYPE rqb;           /* pre-allocated RQB resource */
		CM_PD_AR_PRM_END_TYPE prm_end_args;  /* pre-allocated struct for prm-end */

		CM_UPPER_RQB_PTR_TYPE br_next; /* queue (of size 1) of CM_OPC_SV_R1BRIDGE_RECEIVED requests */
		CM_UPPER_RQB_PTR_TYPE pd;      /* current PDEV request */
		LSA_UINT16 user_id;            /* pdev_A's ID of current request */
		LSA_BOOL wf_posted;            /* true if waiting for "posted" ("buffer" is on-the-fly) */

	} pdev_B;

	/***/

	struct { /* initiator: "A" talks to "B" */

		CM_SV_R1BMSG_PREFIX_PTR_TYPE buffer; /* pre-allocated R1B message buffer for apdu-status */

		CM_LIST_ENTRY_TYPE queue;                 /* queue of apdu-status requests */
		CM_SV_R1_APDU_STAT_REQUEST_PTR_TYPE curr; /* current apdu-status request */
		LSA_BOOL wf_posted;                       /* true if waiting for "posted" ("buffer" is on-the-fly) */

	} stat_A;

	/***/

	struct { /* responder: "B" listens to "A" and responds */

		CM_SV_R1BMSG_PREFIX_PTR_TYPE buffer; /* pre-allocated R1B message buffer for apdu-status-done */
		struct cm_sv_iterator_tag * iter;    /* pre-allocated iterator to find the way back from edd */

		CM_UPPER_RQB_PTR_TYPE br_next; /* queue (of size 1) of CM_OPC_SV_R1BRIDGE_RECEIVED requests */
		LSA_BOOL wf_posted;            /* true if waiting for "posted" ("buffer" is on-the-fly) */

	} stat_B;

} CM_SV_DEVICE_R1B_TYPE;

typedef CM_SV_DEVICE_R1B_TYPE *CM_SV_DEVICE_R1B_PTR_TYPE;

/*===========================================================================*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_ISVAR_SR_H */
