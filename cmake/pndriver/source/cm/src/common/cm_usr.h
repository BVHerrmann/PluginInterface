#ifndef CM_USR_H
#define CM_USR_H

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
/*  F i l e               &F: cm_usr.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef CM_CFG_EVERYTHING_VISIBLE
#define CM_CFG_EVERYTHING_VISIBLE 1 /* 1 = everything visible (AP00754346) */
#else
/* must not be defined outside of cm_usr.h (except for testing) */
#endif

/******************************************************************************
common types
******************************************************************************/
typedef LSA_FATAL_ERROR_TYPE *CM_FATAL_ERROR_PTR_TYPE; /* pointer to LSA_FATAL_ERROR */


typedef LSA_VOID *CM_UPPER_MEM_PTR_TYPE; /* pointer to upper-memory */

typedef LSA_VOID *CM_LOCAL_MEM_PTR_TYPE; /* pointer to local-memory */


typedef LSA_VOID *CM_ACP_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID *CM_EDD_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID *CM_GSY_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID *CM_NARE_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID *CM_OHA_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID *CM_RPC_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID *CM_MRP_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */

typedef LSA_VOID *CM_POF_LOWER_MEM_PTR_TYPE; /* pointer to lower-memory */


typedef LSA_UINT8 * CM_COMMON_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 const * CM_COMMON_MEM_U8_CONST_PTR_TYPE;

typedef LSA_UINT8 * CM_UPPER_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 const * CM_UPPER_MEM_U8_CONST_PTR_TYPE;

typedef LSA_UINT8 * CM_ACP_MEM_U8_PTR_TYPE;

typedef LSA_UINT8 * CM_RPC_MEM_U8_PTR_TYPE;

typedef struct cm_mac_tag {
	LSA_UINT8 mac[6];
} CM_MAC_TYPE;

typedef CM_MAC_TYPE *CM_MAC_PTR_TYPE;

typedef CM_MAC_TYPE const *CM_MAC_CONST_PTR_TYPE;


typedef struct cm_reserved_interval_if_red_tag
{
	LSA_UINT32   RxEndNs;
	LSA_UINT32   RxLocalTransferEndNs;
	LSA_UINT32   TxEndNs;
	LSA_INT32    TxLocalTransferStartNs;
	LSA_UINT32   TxLocalTransferEndNs;
} CM_RESERVED_INTERVAL_IF_RED_TYPE;

typedef CM_RESERVED_INTERVAL_IF_RED_TYPE *CM_RESERVED_INTERVAL_IF_RED_PTR_TYPE;


/******************************************************************************
includes
******************************************************************************/
#include "cm_err.h" /* PNIO error codes and make-fetch-macros */

#include "cm_lib.h" /* conversion functions */

#include "cm_list.h" /* list processing macros */

#include "cm_arcb.h" /* ARCB .. the Application Relation Control Block */


/**************************************************************************//**
@defgroup cm-opcodes Opcodes
@ingroup cm-group
@brief CM Channel-specific opcodes.
@details see @ref CM_RQB_TYPE - opcode (part of CM_RQB_HEADER)
******************************************************************************/

/**************************************************************************//**
@defgroup cm-cl-opcodes 020-059 Client Opcodes
@ingroup cm-opcodes
@brief CM Client specific opcodes
******************************************************************************/

/**************************************************************************//**
@defgroup cm-mc-opcodes 060-099 Multicast Opcodes
@ingroup cm-opcodes
@brief CM Multicast specific opcodes
******************************************************************************/

/**************************************************************************//**
@defgroup cm-sv-opcodes 100-199 Server Opcodes
@ingroup cm-opcodes
@brief CM Server specific opcodes
******************************************************************************/

/**************************************************************************//**
@defgroup cm-sv-opcodes-r1 170-174 Server R1 Opcodes
@ingroup cm-sv-opcodes
@brief CM Server R1 specific opcodes
******************************************************************************/

/**************************************************************************//**
@defgroup cm-pd-opcodes 200-249 Physical Device Opcodes
@ingroup cm-opcodes
@brief CM PhysicalDevice specific opcodes
******************************************************************************/


/*****************************************************************************/
/* OPCODES                                                                   */
/* CM_RQB_TYPE::opcode                                                       */
/**************************************************************************//**
@defgroup cm-opcode-overview Opcode overview
@ingroup cm-opcodes
@{
@brief Overview of all CM opcodes.
******************************************************************************/
/** @cond INTERNAL */
#define CM_OPC_TIMER  0 /**< internally used only */
/** @endcond */

#define CM_OPC_OPEN_CHANNEL           1 /**< see @ref CM_OPC_OPEN_CHANNEL */
#define CM_OPC_CLOSE_CHANNEL          2 /**< see @ref CM_OPC_CLOSE_CHANNEL */
#define CM_OPC_reserved3              3 /**< reserved */
#define CM_OPC_reserved4              4 /**< reserved */
#define CM_OPC_reserved5              5 /**< reserved */
#define CM_OPC_reserved6      6
#define CM_OPC_reserved7      7

#define CM_OPC_reserved8              8


#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE

#define CM_OPC_CL_CREATE                      20 /**< cl-channel specific, see @ref CM_OPC_CL_CREATE */
#define CM_OPC_CL_DELETE                      21 /**< cl-channel specific, see @ref CM_OPC_CL_DELETE */
#define CM_OPC_CL_CONTROL                     22 /**< cl-channel specific, see @ref CM_OPC_CL_CONTROL */

#define CM_OPC_CL_DEVICE_ADD                  23 /**< device/ar specific, see @ref CM_OPC_CL_DEVICE_ADD */
#define CM_OPC_CL_DEVICE_REMOVE               24 /**< device/ar specific, see @ref CM_OPC_CL_DEVICE_REMOVE */
#define CM_OPC_CL_DEVICE_CONTROL              25 /**< device/ar specific, see @ref CM_OPC_CL_DEVICE_CONTROL */

#define CM_OPC_CL_RECORD_READ                 26 /**< see @ref CM_OPC_CL_RECORD_READ */
#define CM_OPC_CL_RECORD_READ_QUERY           27 /**< see @ref CM_OPC_CL_RECORD_READ_QUERY */

/* #define CM_OPC_CL_RECORD_READ_IMPLICIT        28, removed since v70i1.32, use CM_OPC_CL_RECORD_READ */
/* #define CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY  29, removed since v70i1.32, use CM_OPC_CL_RECORD_READ_QUERY */

#define CM_OPC_CL_RECORD_WRITE                30 /**< see @ref CM_OPC_CL_RECORD_WRITE */
#define CM_OPC_CL_RECORD_WRITE_MULTIPLE       31 /**< see @ref CM_OPC_CL_RECORD_WRITE_MULTIPLE */

#define CM_OPC_CL_ALARM_SEND                  32 /**< see @ref CM_OPC_CL_ALARM_SEND */
#define CM_OPC_CL_reserverd33                 33 /**< reserved */

#define CM_OPC_CL_LOGBOOK_READ                34 /**< see @ref CM_OPC_CL_LOGBOOK_READ */

#define CM_OPC_CL_DEVICE_PROVIDE_ALARM        35 /**< see @ref CM_OPC_CL_DEVICE_PROVIDE_ALARM */
#define CM_OPC_CL_ALARM_IND                   36 /**< see [CM_OPC_CL_ALARM_IND](@ref CM_OPC_CL_ALARM_IND_RSP) */
#define CM_OPC_CL_ALARM_RSP                   37 /**< see [CM_OPC_CL_ALARM_RSP](@ref CM_OPC_CL_ALARM_IND_RSP) */

#define CM_OPC_CL_DEVICE_PROVIDE_EVENT        40 /**< see @ref CM_OPC_CL_DEVICE_PROVIDE_EVENT */
#define CM_OPC_CL_AR_ABORT_IND                41 /**< see [CM_OPC_CL_AR_ABORT_IND](@ref CM_OPC_CL_AR_ABORT_IND_RSP) */
#define CM_OPC_CL_AR_ABORT_RSP                42 /**< see [CM_OPC_CL_AR_ABORT_RSP](@ref CM_OPC_CL_AR_ABORT_IND_RSP) */
#define CM_OPC_CL_AR_PRE_DATA_IND             43 /**< see [CM_OPC_CL_AR_PRE_DATA_IND](@ref CM_OPC_CL_AR_PRE_DATA_IND_RSP) */
#define CM_OPC_CL_AR_PRE_DATA_RSP             44 /**< see [CM_OPC_CL_AR_PRE_DATA_RSP](@ref CM_OPC_CL_AR_PRE_DATA_IND_RSP) */
#define CM_OPC_CL_AR_PRE_OFFLINE_IND          45 /**< see [CM_OPC_CL_AR_PRE_OFFLINE_IND](@ref CM_OPC_CL_AR_PRE_OFFLINE_IND_RSP) */
#define CM_OPC_CL_AR_PRE_OFFLINE_RSP          46 /**< see [CM_OPC_CL_AR_PRE_OFFLINE_RSP](@ref CM_OPC_CL_AR_PRE_OFFLINE_IND_RSP) */
#define CM_OPC_CL_AR_IN_DATA_IND              47 /**< see [CM_OPC_CL_AR_IN_DATA_IND](@ref CM_OPC_CL_AR_IN_DATA_IND_RSP) */
#define CM_OPC_CL_AR_IN_DATA_RSP              48 /**< see [CM_OPC_CL_AR_IN_DATA_RSP](@ref CM_OPC_CL_AR_IN_DATA_IND_RSP) */
#define CM_OPC_CL_AR_OFFLINE_IND              49 /**< see [CM_OPC_CL_AR_OFFLINE_IND](@ref CM_OPC_CL_AR_OFFLINE_IND_RSP) */
#define CM_OPC_CL_AR_OFFLINE_RSP              50 /**< see [CM_OPC_CL_AR_OFFLINE_RSP](@ref CM_OPC_CL_AR_OFFLINE_IND_RSP) */

#define CM_OPC_CL_AR_DATA_STATUS_IND          51 /**< see [CM_OPC_CL_AR_DATA_STATUS_IND](@ref CM_OPC_CL_AR_DATA_STATUS_IND_RSP) */
#define CM_OPC_CL_AR_DATA_STATUS_RSP          52 /**< see [CM_OPC_CL_AR_DATA_STATUS_RSP](@ref CM_OPC_CL_AR_DATA_STATUS_IND_RSP) */

#define CM_OPC_CL_PRM_UPDATE                  53 /**< see @ref CM_OPC_CL_PRM_UPDATE */
#define CM_OPC_CL_AR_PRM_BEGIN_IND            54 /**< see [CM_OPC_CL_AR_PRM_BEGIN_IND](@ref CM_OPC_CL_AR_PRM_BEGIN_IND_RSP) */
#define CM_OPC_CL_AR_PRM_BEGIN_RSP            55 /**< see [CM_OPC_CL_AR_PRM_BEGIN_RSP](@ref CM_OPC_CL_AR_PRM_BEGIN_IND_RSP) */
#define CM_OPC_CL_AR_PRM_INDATA_IND           56 /**< see [CM_OPC_CL_AR_PRM_INDATA_IND](@ref CM_OPC_CL_AR_PRM_INDATA_IND_RSP) */
#define CM_OPC_CL_AR_PRM_INDATA_RSP           57 /**< see [CM_OPC_CL_AR_PRM_INDATA_RSP](@ref CM_OPC_CL_AR_PRM_INDATA_IND_RSP) */

#endif /* CM_CFG_MAX_CLIENTS */


#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

#define CM_OPC_MC_CREATE                  60  /**< mc-channel specific, see @ref CM_OPC_MC_CREATE */
#define CM_OPC_MC_DELETE                  61  /**< mc-channel specific, see @ref CM_OPC_MC_DELETE */
#define CM_OPC_MC_CONTROL                 62  /**< mc-channel specific, see @ref CM_OPC_MC_CONTROL */

#define CM_OPC_MC_DEVICE_ADD              63  /**< device/ar commands, see @ref CM_OPC_MC_DEVICE_ADD */
#define CM_OPC_MC_DEVICE_REMOVE           64  /**< device/ar commands, see @ref CM_OPC_MC_DEVICE_REMOVE */
#define CM_OPC_MC_DEVICE_CONTROL          65  /**< device/ar commands, see @ref CM_OPC_MC_DEVICE_CONTROL */

#define CM_OPC_MC_EVENT_AR_ADDR_INFO_IND  66  /**< device/ar indications, see [CM_OPC_MC_EVENT_AR_ADDR_INFO_IND](@ref CM_OPC_MC_EVENT_AR_ADDR_INFO_IND_RSP) */
#define CM_OPC_MC_EVENT_AR_ADDR_INFO_RSP  67  /**< device/ar indications, see [CM_OPC_MC_EVENT_AR_ADDR_INFO_RSP](@ref CM_OPC_MC_EVENT_AR_ADDR_INFO_IND_RSP) */
#define CM_OPC_MC_EVENT_AR_IN_DATA_IND    68  /**< device/ar indications, see [CM_OPC_MC_EVENT_AR_IN_DATA_IND](@ref CM_OPC_MC_EVENT_AR_IN_DATA_IND_RSP) */
#define CM_OPC_MC_EVENT_AR_IN_DATA_RSP    69  /**< device/ar indications, see [CM_OPC_MC_EVENT_AR_IN_DATA_RSP](@ref CM_OPC_MC_EVENT_AR_IN_DATA_IND_RSP) */
#define CM_OPC_MC_EVENT_AR_OFFLINE_IND    70  /**< device/ar indications, see [CM_OPC_MC_EVENT_AR_OFFLINE_IND](@ref CM_OPC_MC_EVENT_AR_OFFLINE_IND_RSP) */
#define CM_OPC_MC_EVENT_AR_OFFLINE_RSP    71  /**< device/ar indications, see [CM_OPC_MC_EVENT_AR_OFFLINE_RSP](@ref CM_OPC_MC_EVENT_AR_OFFLINE_IND_RSP) */
#define CM_OPC_MC_EVENT_APDU_STATUS_IND   72  /**< device/ar indications, see [CM_OPC_MC_EVENT_APDU_STATUS_IND](@ref CM_OPC_MC_EVENT_APDU_STATUS_IND_RSP) */
#define CM_OPC_MC_EVENT_APDU_STATUS_RSP   73  /**< device/ar indications, see [CM_OPC_MC_EVENT_APDU_STATUS_RSP](@ref CM_OPC_MC_EVENT_APDU_STATUS_IND_RSP) */

#define CM_OPC_MC_DEVICE_PROVIDE_EVENT    74 /**< see @ref CM_OPC_MC_DEVICE_PROVIDE_EVENT */

#endif /* CM_CFG_MAX_MULTICAST_CHANNELS */


#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE

#define CM_OPC_SV_CREATE                  100 /**< see @ref CM_OPC_SV_CREATE */
#define CM_OPC_SV_DELETE                  101 /**< see @ref CM_OPC_SV_DELETE */
#define CM_OPC_SV_CONTROL                 102 /**< see @ref CM_OPC_SV_CONTROL */

#define CM_OPC_SV_DEVICE_ADD              103 /**< see @ref CM_OPC_SV_DEVICE_ADD */
#define CM_OPC_SV_DEVICE_REMOVE           104 /**< see @ref CM_OPC_SV_DEVICE_REMOVE */
#define CM_OPC_SV_DEVICE_CONTROL          105 /**< see @ref CM_OPC_SV_DEVICE_CONTROL */
#define CM_OPC_SV_DEVICE_LED_INFO         106 /**< see @ref CM_OPC_SV_DEVICE_LED_INFO */
#define CM_OPC_SV_DEVICE_PROVIDE_EVENT    107 /**< see @ref CM_OPC_SV_DEVICE_PROVIDE_EVENT */
#define CM_OPC_SV_DEVICE_PROVIDE_ALARM    108 /**< see @ref CM_OPC_SV_DEVICE_PROVIDE_ALARM */
#define CM_OPC_SV_DEVICE_READ             109 /**< see @ref CM_OPC_SV_DEVICE_READ */

#define CM_OPC_SV_reserved110             110 /**< reserved */

#define CM_OPC_SV_SUBMODULE_ADD           111 /**< see @ref CM_OPC_SV_SUBMODULE_ADD */
#define CM_OPC_SV_SUBMODULE_REMOVE        112 /**< see @ref CM_OPC_SV_SUBMODULE_REMOVE */

#define CM_OPC_SV_reserved113             113 /**< reserved */

#define CM_OPC_SV_DIAG_ADD                114 /**< see @ref CM_OPC_SV_DIAG_ADD */
#define CM_OPC_SV_DIAG_REMOVE             115 /**< see @ref CM_OPC_SV_DIAG_REMOVE */

#define CM_OPC_SV_DIAG_ADD2               116 /**< V5.2i4.16, AP01467031, see @ref CM_OPC_SV_DIAG_ADD2 */
#define CM_OPC_SV_DIAG_REMOVE2            117 /**< V5.2i4.16, AP01467031, see @ref CM_OPC_SV_DIAG_REMOVE2 */

#define CM_OPC_SV_ARSET_TRIGGER           118 /**< see @ref CM_OPC_SV_ARSET_TRIGGER */
#define CM_OPC_SV_ARSET_ABORT             119 /**< see @ref CM_OPC_SV_ARSET_ABORT */

#define CM_OPC_SV_AR_APPL_READY           120 /**< see @ref CM_OPC_SV_AR_APPL_READY */
#define CM_OPC_SV_AR_CONTROL              121 /**< see @ref CM_OPC_SV_AR_CONTROL */
#define CM_OPC_SV_AR_ABORT                122 /**< see @ref CM_OPC_SV_AR_ABORT */
#define CM_OPC_SV_reserved123             123 /**< reserved */
#define CM_OPC_SV_reserved124             124 /**< reserved */
#define CM_OPC_SV_AR_RIR_ACK              125 /**< see @ref CM_OPC_SV_AR_RIR_ACK */

#define CM_OPC_SV_AR_ALARM_SEND           126 /**< see @ref CM_OPC_SV_AR_ALARM_SEND */

#define CM_OPC_SV_AR_ALARM_IND            127 /**< see [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP) */
#define CM_OPC_SV_AR_ALARM_RSP            128 /**< see [CM_OPC_SV_AR_ALARM_RSP](@ref CM_OPC_SV_AR_ALARM_IND_RSP) */
#define CM_OPC_SV_AR_ALARM_ACK            129 /**< see @ref CM_OPC_SV_AR_ALARM_ACK */

#define CM_OPC_SV_AR_CONNECT_IND          130 /**< see [CM_OPC_SV_AR_CONNECT_IND](@ref CM_OPC_SV_AR_CONNECT_IND_RSP) */
#define CM_OPC_SV_AR_CONNECT_RSP          131 /**< see [CM_OPC_SV_AR_CONNECT_RSP](@ref CM_OPC_SV_AR_CONNECT_IND_RSP) */

#define CM_OPC_SV_AR_DISCONNECT_IND       132 /**< see [CM_OPC_SV_AR_DISCONNECT_IND](@ref CM_OPC_SV_AR_DISCONNECT_IND_RSP) */
#define CM_OPC_SV_AR_DISCONNECT_RSP       133 /**< see [CM_OPC_SV_AR_DISCONNECT_RSP](@ref CM_OPC_SV_AR_DISCONNECT_IND_RSP) */

#define CM_OPC_SV_AR_OWNERSHIP_IND        134 /**< see [CM_OPC_SV_AR_OWNERSHIP_IND](@ref CM_OPC_SV_AR_OWNERSHIP_IND_RSP) */
#define CM_OPC_SV_AR_OWNERSHIP_RSP        135 /**< see [CM_OPC_SV_AR_OWNERSHIP_RSP](@ref CM_OPC_SV_AR_OWNERSHIP_IND_RSP) */

#define CM_OPC_SV_AR_PRM_END_IND          136 /**< see [CM_OPC_SV_AR_PRM_END_IND](@ref CM_OPC_SV_AR_PRM_END_IND_RSP) */
#define CM_OPC_SV_AR_PRM_END_RSP          137 /**< see [CM_OPC_SV_AR_PRM_END_RSP](@ref CM_OPC_SV_AR_PRM_END_IND_RSP) */

#define CM_OPC_SV_AR_IN_DATA_IND          138 /**< see [CM_OPC_SV_AR_IN_DATA_IND](@ref CM_OPC_SV_AR_IN_DATA_IND_RSP) */
#define CM_OPC_SV_AR_IN_DATA_RSP          139 /**< see [CM_OPC_SV_AR_IN_DATA_RSP](@ref CM_OPC_SV_AR_IN_DATA_IND_RSP) */

#define CM_OPC_SV_AR_RIR_IND              140 /**< see [CM_OPC_SV_AR_RIR_IND](@ref CM_OPC_SV_AR_RIR_IND_RSP) */
#define CM_OPC_SV_AR_RIR_RSP              141 /**< see [CM_OPC_SV_AR_RIR_RSP](@ref CM_OPC_SV_AR_RIR_IND_RSP) */

#define CM_OPC_SV_AR_READ_IND             142 /**< see [CM_OPC_SV_AR_READ_IND](@ref CM_OPC_SV_AR_READ_IND_RSP) */
#define CM_OPC_SV_AR_READ_RSP             143 /**< see [CM_OPC_SV_AR_READ_RSP](@ref CM_OPC_SV_AR_READ_IND_RSP) */

#define CM_OPC_SV_AR_WRITE_IND            144 /**< see [CM_OPC_SV_AR_WRITE_IND](@ref CM_OPC_SV_AR_WRITE_IND_RSP) */
#define CM_OPC_SV_AR_WRITE_RSP            145 /**< see [CM_OPC_SV_AR_WRITE_RSP](@ref CM_OPC_SV_AR_WRITE_IND_RSP) */

#define CM_OPC_SV_AR_DATA_STATUS_IND      146 /**< see [CM_OPC_SV_AR_DATA_STATUS_IND](@ref CM_OPC_SV_AR_DATA_STATUS_IND_RSP) */
#define CM_OPC_SV_AR_DATA_STATUS_RSP      147 /**< see [CM_OPC_SV_AR_DATA_STATUS_RSP](@ref CM_OPC_SV_AR_DATA_STATUS_IND_RSP) */

#if CM_CFG_SV_AR_HAS_MCONS || CM_CFG_EVERYTHING_VISIBLE
#define CM_OPC_SV_AR_MCONS_IND            148 /**< see [CM_OPC_SV_AR_MCONS_IND](@ref CM_OPC_SV_AR_MCONS_IND_RSP) */
#define CM_OPC_SV_AR_MCONS_RSP            149 /**< see [CM_OPC_SV_AR_MCONS_RSP](@ref CM_OPC_SV_AR_MCONS_IND_RSP) */
#define CM_OPC_SV_reserved150             150 /**< reserved */
#define CM_OPC_SV_reserved151             151 /**< reserved */
#define CM_OPC_SV_reserved152             152 /**< reserved */
#define CM_OPC_SV_reserved153             153 /**< reserved */
#define CM_OPC_SV_reserved154             154 /**< reserved */
#define CM_OPC_SV_reserved155             155 /**< reserved */
#endif

#define CM_OPC_SV_AR_ALARM_PDEV_GROUP     156 /**< internally used only, see @ref CM_OPC_SV_AR_ALARM_PDEV_GROUP */
#define CM_OPC_SV_AR_ALARM_PLUG_GROUP     157 /**< internally used only, see @ref CM_OPC_SV_AR_ALARM_PLUG_GROUP */
#define CM_OPC_SV_reserved158             158 /**< reserved */
#define CM_OPC_SV_AR_ALARM_ACK_INTERNAL   159 /**< internally used only, see @ref CM_OPC_SV_AR_ALARM_ACK_INTERNAL */
#define CM_OPC_SV_AR_ALARM_RPL_INTERNAL   160 /**< internally used only, see @ref CM_OPC_SV_AR_ALARM_RPL_INTERNAL */
#define CM_OPC_SV_AR_ALARM_R1PD_INTERNAL  161 /**< internally used only, see @ref CM_OPC_SV_AR_ALARM_R1PD_INTERNAL */

#define CM_OPC_SV_reserved162             162 /**< reserved */
#define CM_OPC_SV_reserved163             163 /**< reserved */
#define CM_OPC_SV_reserved164             164 /**< reserved */

#define CM_OPC_SV_AR_PDEVPRM_IND          165 /**< see [CM_OPC_SV_AR_PDEVPRM_IND](@ref CM_OPC_SV_AR_PDEVPRM_IND_RSP) */
#define CM_OPC_SV_AR_PDEVPRM_RSP          166 /**< see [CM_OPC_SV_AR_PDEVPRM_RSP](@ref CM_OPC_SV_AR_PDEVPRM_IND_RSP) */
#define CM_OPC_SV_AR_PDEVPRM_READ         167 /**< see @ref CM_OPC_SV_AR_PDEVPRM_READ */
#define CM_OPC_SV_AR_PDEVPRM_WRITE        168 /**< see @ref CM_OPC_SV_AR_PDEVPRM_WRITE */
#define CM_OPC_SV_reserved169             169 /**< reserved */

#define CM_OPC_SV_R1BRIDGE_SEND_PROVIDE   170 /**< see @ref CM_OPC_SV_R1BRIDGE_SEND_PROVIDE */
#define CM_OPC_SV_R1BRIDGE_SEND_IND       171 /**< see [CM_OPC_SV_R1BRIDGE_SEND_IND](@ref CM_OPC_SV_R1BRIDGE_SEND_IND_RSP) */
#define CM_OPC_SV_R1BRIDGE_SEND_RSP       172 /**< see [CM_OPC_SV_R1BRIDGE_SEND_RSP](@ref CM_OPC_SV_R1BRIDGE_SEND_IND_RSP) */
#define CM_OPC_SV_R1BRIDGE_RECEIVED       173 /**< see @ref CM_OPC_SV_R1BRIDGE_RECEIVED */
#define CM_OPC_SV_R1BRIDGE_CONTROL        174 /**< see @ref CM_OPC_SV_R1BRIDGE_CONTROL */

#endif /* CM_CFG_MAX_SERVERS */


#define CM_OPC_PD_CREATE                       200 /**< see @ref CM_OPC_PD_CREATE */
#define CM_OPC_PD_DELETE                       201 /**< see @ref CM_OPC_PD_DELETE */

#define CM_OPC_PD_REMAP                        202 /**< see @ref CM_OPC_PD_REMAP */

#define CM_OPC_PD_LINK_STATUS_INFO             203 /**< see @ref CM_OPC_PD_LINK_STATUS_INFO */
#define CM_OPC_PD_SYNC_LED_INFO                204 /**< see @ref CM_OPC_PD_SYNC_LED_INFO */

#define CM_OPC_PD_SUBMODULE_CONTROL            205 /**< see @ref CM_OPC_PD_SUBMODULE_CONTROL */
#define CM_OPC_PD_DIAG_ADD                     206 /**< see @ref CM_OPC_PD_DIAG_ADD */
#define CM_OPC_PD_DIAG_REMOVE                  207 /**< see @ref CM_OPC_PD_DIAG_REMOVE */

#define CM_OPC_PD_PRM_READ                     210 /**< see @ref CM_OPC_PD_PRM_READ */

#define CM_OPC_PD_PRM_BEGIN                    211 /**< see @ref CM_OPC_PD_PRM_BEGIN */
#define CM_OPC_PD_PRM_WRITE                    212 /**< see @ref CM_OPC_PD_PRM_WRITE */
#define CM_OPC_PD_PRM_END                      213 /**< see @ref CM_OPC_PD_PRM_END */

#define CM_OPC_PD_PROVIDE_EVENT                214 /**< see @ref CM_OPC_PD_PROVIDE_EVENT */
#define CM_OPC_PD_EVENT_APPL_READY_IND         215 /**< see [CM_OPC_PD_EVENT_APPL_READY_IND](@ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP) */
#define CM_OPC_PD_EVENT_APPL_READY_RSP         216 /**< see [CM_OPC_PD_EVENT_APPL_READY_RSP](@ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP) */

#define CM_OPC_PD_ALARM_IND                    217 /**< see [CM_OPC_PD_ALARM_IND](@ref CM_OPC_PD_ALARM_IND_RSP) */
#define CM_OPC_PD_ALARM_RSP                    218 /**< see [CM_OPC_PD_ALARM_RSP](@ref CM_OPC_PD_ALARM_IND_RSP) */
#define CM_OPC_PD_PROVIDE_ALARM                219 /**< see @ref CM_OPC_PD_PROVIDE_ALARM */

#define CM_OPC_PD_PTCP_MASTER_CONTROL          220 /**< see @ref CM_OPC_PD_PTCP_MASTER_CONTROL */

/** @cond INTERNAL */
#define CM_OPC_PD_PRM_BEGIN_SERVER             230 /**< internal, cm-sv to cm-pd @see @ref CM_OPC_PD_PRM_BEGIN_SERVER */
#define CM_OPC_PD_PRM_READ_SERVER              231 /**< internal, cm-sv to cm-pd @see @ref CM_OPC_PD_PRM_READ_SERVER */
#define CM_OPC_PD_PRM_WRITE_SERVER             232 /**< internal, cm-sv to cm-pd @see @ref CM_OPC_PD_PRM_WRITE_SERVER */
#define CM_OPC_PD_PRM_END_SERVER               233 /**< internal, cm-sv to cm-pd @see @ref CM_OPC_PD_PRM_END_SERVER */
#define CM_OPC_PD_PRM_RELINQUISH_SERVER        234 /**< internal, cm-sv to cm-pd @see @ref CM_OPC_PD_PRM_RELINQUISH_SERVER */
#define CM_OPC_PD_PRM_READ_CLIENT              235 /**< internal, cm-cl to cm-pd @see @ref CM_OPC_PD_PRM_READ_CLIENT */
#define CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER  236 /**< internal, cm-pd to cm-pd @see CM_INTERNAL_OPC_PD_PRM_CONFIG_TRIGGER */
#define CM_INTERNAL_OPC_PD_INTERNAL            237 /**< internal, cm-pd to cm-pd @see CM_INTERNAL_OPC_PD_INTERNAL */
/** @endcond */


#define CM_OPC_MAX  250 /**< Maximum of CM Opcodes */
/** @} ***********************************************************************/

/*****************************************************************************/
/* ERROR_CODES                                                               */
/* CM_RQB_TYPE::response                                                     */
/**************************************************************************//**
@defgroup cm-response-codes Response Codes
@ingroup cm-group
@{
@brief CM_RQB response codes
@details The following codes are used as response in CM_RQB_TYPE::response.
@attention max. 62 error codes
******************************************************************************/
enum cm_response_enum {
	CM_OK                   = LSA_RSP_OK /**< (usually 0x01/1 but configurable) */,
	CM_OK_PENDING           = (LSA_RSP_OK_OFFSET + 0) /**< (0x40/64) internally used only! */,
	CM_OK_CANCELLED         = (LSA_RSP_OK_OFFSET + 1) /**< (0x41/65) */,
	CM_OK_REPLACED          = (LSA_RSP_OK_OFFSET + 2) /**< (0x42/66) */,
	CM_OK_ADAPTION_NEEDED   = (LSA_RSP_OK_OFFSET + 3) /**< (0x43/67) */,
	CM_OK_RESET_TO_FACTORY  = (LSA_RSP_OK_OFFSET + 4) /**< (0x44/68) */,
	CM_OK_R1_NOT_ACKED      = (LSA_RSP_OK_OFFSET + 5) /**< (0x45/69) */,

	CM_ERR_RESOURCE         = LSA_RSP_ERR_RESOURCE /**< (0x84/132) */,
	CM_ERR_PARAM            = LSA_RSP_ERR_PARAM    /**< (0x86/134) */,
	CM_ERR_SEQUENCE         = LSA_RSP_ERR_SEQUENCE /**< (0x87/135) */,
	CM_ERR_SYS_PATH         = LSA_RSP_ERR_SYS_PATH /**< (0x91/145) */,

	CM_ERR_ELEM = LSA_RSP_ERR_OFFSET, /**< (0xc0/192) must check elem[].response */
	CM_ERR_ALLOC_UPPER,       /**< (0xc1/193) could not allocate upper memory */
	CM_ERR_ALLOC_LOCAL,       /**< (0xc2/194) could not allocate local memory */
	CM_ERR_ALLOC_LOWER,       /**< (0xc3/195) could not allocate lower memory */
	CM_ERR_LOWER_LAYER,       /**< (0xc4/196) an error occurred in the lower layer */
	CM_ERR_NOT_ACCESSIBLE,    /**< (0xc5/197) internally used */
	CM_ERR_ABORT,             /**< (0xc6/198) operation was aborted */
	CM_ERR_SESSION,           /**< (0xc7/199) request belongs to an old session */
	CM_ERR_TIMEOUT,           /**< (0xc8/200) RPC failed due to a timeout */
	CM_ERR_COMM,              /**< (0xc9/201) RPC failed due to a comm-error */
	CM_ERR_BUSY,              /**< (0xca/202) RPC peer signalled "busy" (should try again later) */
	CM_ERR_LOWER_NARE,        /**< (0xcb/203) an error occurred in the lower layer NARE */
	CM_ERR_LOWER_RPC,         /**< (0xcc/204) an error occurred in the lower layer CLRPC */
	CM_ERR_RESERVED_CD,       /**< (0xcd/205) */
	CM_ERR_OWNED,             /**< (0xce/206) interface-submodule cannot be removed because it is owned by an AR */
	CM_ERR_FORMAT,            /**< (0xcf/207) wrong format of alarm data */
	CM_ERR_PRM_INDEX,         /**< (0xd0/208) unknown index (PrmWrite and PrmRead) */
	CM_ERR_PRM_PORTID,        /**< (0xd1/209) port-id does not match with index (PrmWrite and PrmRead) */
	CM_ERR_PRM_DATA,          /**< (0xd2/210) data-length too short (PrmRead) or data-length not consistent with block-structure (PrmWrite) */
	CM_ERR_PRM_BLOCK,         /**< (0xd3/211) wrong block-type/version or something wrong with the block-data (PrmWrite) */
	CM_ERR_PRM_CONSISTENCY,   /**< (0xd4/212) the parameterization is not consistent (PrmEnd) */
	CM_ERR_PRM_OTHER,         /**< (0xd5/213) internally used */
	CM_ERR_PRM_VERSION,       /**< (0xd6/214) internally used */
	CM_ERR_ARVENDORBLOCK,     /**< (0xd7/215) could not decode ARVendorBlockReq (V5.2i3, AP01354956) */
	CM_ERR_APPL_RESOURCE,     /**< (0xd8/216) AR parameters violate GSD MaxApplicationXXXLength (V7.0i2.10, TIA 2414553) */
	CM_ERR_PRM_ACCESS_DENIED, /**< (0xd9/217) access denied */

	CM_ERR_NOT_YET = 0xFE   /**< lsa-components may use 0xc0 - 0xfe ... */
};
/** @} ***********************************************************************/

/*****************************************************************************/
/* Diagnosis Channel Properties                                              */
/**************************************************************************//**
@defgroup cm-channel-properties Diagnosis Channel Properties
@ingroup cm-group
@{
@brief CM Diagnosis channel properties
******************************************************************************/
#define CM_DIAG_CHANNEL_PROPERTIES_MAKE(type, acc, m_req, m_dem, spec, dir) ((LSA_UINT16) \
	( (((type)  & 0xFF) <<  0) /* mask 0x00FF */ \
	| (((acc)   & 0x01) <<  8) /* mask 0x0100 */ \
	| (((m_req) & 0x01) <<  9) /* mask 0x0200 */ \
	| (((m_dem) & 0x01) << 10) /* mask 0x0400 */ \
	| (((spec)  & 0x03) << 11) /* mask 0x1800 */ \
	| (((dir)   & 0x07) << 13) /* mask 0xE000 */ \
	))

#define CM_CHPR_TYPE_MASK  0x00FF /**< Bit 0 - 7: ChannelProperties.Type */
#define CM_CHPR_TYPE_OTHER 0x0000 /**<  other */
#define CM_CHPR_TYPE_1     0x0001 /**<  0x01 1 Bit */
#define CM_CHPR_TYPE_2     0x0002 /**<  0x02 2 Bit */
#define CM_CHPR_TYPE_4     0x0003 /**<  0x03 4 Bit */
#define CM_CHPR_TYPE_8     0x0004 /**<  0x04 8 Bit */
#define CM_CHPR_TYPE_16    0x0005 /**<  0x05 16 Bit */
#define CM_CHPR_TYPE_32    0x0006 /**<  0x06 32 Bit */
#define CM_CHPR_TYPE_64    0x0007 /**<  0x07 64 Bit */
#define CM_CHPR_TYPE_RSVD  0x00FF /**<  0x08 - 0xFF reserved */

#define CM_CHPR_ACCU_MASK  0x0100 /**< Bit 8 : ChannelProperties.Accumulative */
#define CM_CHPR_ACCU_NO    0x0000
#define CM_CHPR_ACCU_YES   0x0100

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_MREQ_MASK  0x0200 /**< Bit 9 : ChannelProperties.MaintenanceRequired */
#define CM_CHPR_MREQ_NO    0x0000
#define CM_CHPR_MREQ_YES   0x0200

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_MDEM_MASK  0x0400 /**< Bit 10 : ChannelProperties.MaintenanceDemanded */
#define CM_CHPR_MDEM_NO    0x0000
#define CM_CHPR_MDEM_YES   0x0400

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_MAINT_MASK (CM_CHPR_MREQ_MASK | CM_CHPR_MDEM_MASK)
#define CM_CHPR_MAINT_ERR  (CM_CHPR_MREQ_NO   | CM_CHPR_MDEM_NO)   /**< Fault (was Diagnosis) */
#define CM_CHPR_MAINT_REQ  (CM_CHPR_MREQ_YES  | CM_CHPR_MDEM_NO)   /**< Maintenance required */
#define CM_CHPR_MAINT_DEM  (CM_CHPR_MREQ_NO   | CM_CHPR_MDEM_YES)  /**< Maintenance demanded */
#define CM_CHPR_MAINT_QUA  (CM_CHPR_MREQ_YES  | CM_CHPR_MDEM_YES)  /**< QualifiedChannelQualifier */

/* ==> see table "Valid combinations within ChannelProperties" in the PNIO-Spec! */
#define CM_CHPR_SPEC_MASK  0x1800 /**< Bit 11 - 12: ChannelProperties.Specifier */
#define CM_CHPR_SPEC_RSVD  0x0000 /**<  0x00 reserved */
#define CM_CHPR_SPEC_APP   0x0800 /**<  0x01 error appears */
#define CM_CHPR_SPEC_DIS   0x1000 /**<  0x02 error disappears and error free */
#define CM_CHPR_SPEC_DBO   0x1800 /**<  0x03 error disappears but other errors remain */

#define CM_CHPR_DIR_MASK   0xE000 /**< Bit 13 - 15: ChannelProperties.Direction */
#define CM_CHPR_DIR_OTHER  0x0000 /**<  0x00 manufacturer specific */
#define CM_CHPR_DIR_IN     0x2000 /**<  0x01 Input */
#define CM_CHPR_DIR_OUT    0x4000 /**<  0x02 Output */
#define CM_CHPR_DIR_INOUT  0x6000 /**<  0x03 Input/Output */
#define CM_CHPR_DIR_RSVD   0xE000 /**<  0x04 - 0x07 reserved */
/** @} ***********************************************************************/


/******************************************************************************
forward declaration
******************************************************************************/
typedef struct cm_rqb_tag * CM_UPPER_RQB_PTR_TYPE;

typedef struct cm_rqb_tag const * CM_UPPER_RQB_CONST_PTR_TYPE;

/*****************************************************************************/
/* XXX CM_OPC_ALARM_IND_RES_PROVIDE                                          */
/**************************************************************************//**
@defgroup CM_OPC_ALARM_IND_RES_PROVIDE XXX CM_OPC_ALARM_IND_RES_PROVIDE
@ingroup cm-opcodes
@brief Obsolete!
@details Opcode was renamed to @ref CM_OPC_PD_PROVIDE_ALARM with version V7.0i1.46 (TIA 1957290).

******************************************************************************/

/*****************************************************************************/
/* 217/218 CM_OPC_PD_ALARM_IND/_RSP                                          */
/**************************************************************************//**
@defgroup CM_OPC_PD_ALARM_IND_RSP 217/218 CM_OPC_PD_ALARM_IND/_RSP
@ingroup cm-pd-opcodes
@brief A local alarm is indicated with PDAlarm.ind.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr              **IND** Don't evaluate
@param CM_ALARM_TYPE::ar_nr                  **IND** Don't evaluate
@param CM_ALARM_TYPE::session_key            **IND** Don't evaluate, for compatibility the constant 0 is supplied.
@param CM_ALARM_TYPE::alarm_priority         **IND** CM_ALARM_PRIORITY_LOW
@param CM_ALARM_TYPE::api                    **IND** 0
@param CM_ALARM_TYPE::slot_nr                **IND** Slot Number
@param CM_ALARM_TYPE::subslot_nr             **IND** Subslot Number
@param CM_ALARM_TYPE::alarm_type             **IND**
													 - CM_ALARM_TYPE_REDUNDANCY
													 - CM_ALARM_TYPE_PORT_DATA_CHANGED
													 - CM_ALARM_TYPE_SYNC_DATA_CHANGED
													 - CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM
													 - CM_ALARM_TYPE_TIME_DATA_CHANGED
													 - CM_ALARM_TYPE_MRPD_PROBLEM
													 - CM_ALARM_TYPE_MULTIPLE_INTERFACE
													 - CM_ALARM_TYPE_DFP_PROBLEM

													Status as of May 2015. Refer also to [/11/](@ref cm-ref-no-11). With CM_OPC_PD_SUBMODULE_CONTROL: CM_ALARM_TYPE_PULL or CM_ALARM_TYPE_PLUG
@param CM_ALARM_TYPE::alarm_sequence         **IND** Sequence Number (0, 1, 2, ... 2047, 0, 1, 2, ...)
@param CM_ALARM_TYPE::diag_channel_available **IND** CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES: "appearing" channel diagnosis(es) exists. Otherwise CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::diag_generic_available **IND** Don't evaluate
@param CM_ALARM_TYPE::diag_submod_available  **IND** CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES: "appearing" generic or channel diagnosis available. Otherwise CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::reserved               **IND** Don't evaluate
@param CM_ALARM_TYPE::ar_diagnosis_state     **IND** Don't evaluate
@param CM_ALARM_TYPE::mod_ident              **IND** Value from the field of the same name of @ref CM_OPC_PD_CREATE
@param CM_ALARM_TYPE::sub_ident              **IND** Don't care, filled by CM., **CNF** Current value.
@param CM_ALARM_TYPE::alarm_tag              **IND** The "AlarmItem::UserStructureIdentifier" (in host-byte-order), see @ref cm-rqbs-alarm-data.
@param CM_ALARM_TYPE::alarm_data_length      **IND** see @ref cm-rqbs-alarm-data
@param CM_ALARM_TYPE::alarm_data             **IND** see @ref cm-rqbs-alarm-data
@param CM_ALARM_TYPE::cm_pnio_err            **IND** Don't evaluate
@param CM_ALARM_TYPE::maintenance_status     **IND** MaintenanceStatus

The Indication RQB is taken from the provide pool of low priority alarms. Temporary resource shortage does not lead to information loss. CM-PD handles this case.

CM-PD allocates a buffer of size PDCreate::max_alarm_data_length for "alarm_data".

The opcode requires a response. Refer to chapter @ref cm-product-architecture-programming-model-general-request-block-circulations-requests.

The alarm_sequence of the first PDAlarm.IND is initialized with 0 and is incremented with each indication. A wrap around to 0 occurs after 0x07FF (2047).

Each PDPrmEnd.CNF resets the alarm_sequence to 0.

@see @ref CM_ALARM_TYPE

## AlarmNotification PDU

Alarm_type = Diagnosis alarm

CM-PD delivers the following alarm types:

- Redundancy
- Port data change notification,
- Sync data change notification,
- Network component problem notification
- Time data changed notification
- Dynamic Frame Packing problem notification
- MRPD problem notification
- Multiple interface mismatch notification

The AlarmItem in the AlarmNotification PDU is filled in the ChannelDiagnosisData (UserStructureIdentifier = 0x8000) or ExtChannelDiagnosisData (UserStructureIdentifier = 0x8002) format.

Extract from [/11/](@ref cm-ref-no-11):

Substitution name       | Structure
------------------------|--------------------------------------------------------------------------------------------
ChannelDiagnosisData	| ChannelNumber, ChannelProperties, ChannelErrorType
ExtChannelDiagnosisData	| ChannelNumber, ChannelProperties, ChannelErrorType, ExtChannelErrorType, ExtChannelAddValue

For more details on AlarmNotification PDU, refer to chapter CM_ALARM_TYPE.

## Alarm_type = CM_ALARM_TYPE_PULL

slot_nr / subslot_nr refer the currently pulled submodule.
The diag_xxx fields, maintenance_status and alarm_tag are set to 0.
Alarm_data_length is set to 26, and alarm_data points to the proper "AlarmNotification-PDU" without "AlarmPayload".

The diagnosis of this submodule should set to 'pulled'.
No further diagnosis alarm will be indicated.

## Alarm_type = CM_ALARM_TYPE_PLUG

slot_nr / subslot_nr refer the currently plugged submodule.
The diag_xxx fields, maintenance_status and alarm_tag are set to 0.
Alarm_data_length is set to 26, and alarm_data points to the proper "AlarmNotification-PDU" without "AlarmPayload".

The diagnosis of this submodule should set to 'plugged' and 'no diagnosis'.
With the response of this indication internally known diagnosis will be indicated with the help of diagnosis alarms.
******************************************************************************/


/*****************************************************************************/
/* 219 CM_OPC_PD_PROVIDE_ALARM                                               */
/**************************************************************************//**
@defgroup CM_OPC_PD_PROVIDE_ALARM 219 CM_OPC_PD_PROVIDE_ALARM
@ingroup cm-pd-opcodes
@brief The AlarmProvide function is used to provide one alarm indication resource.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::alarm_priority **REQ** CM_ALARM_PRIORITY_LOW @st{or CM_ALARM_PRIORITY_HIGH}
@note Rest: don't care!

@attention Only low priority is allowed.

Indications change the opcode of the RQB as follows:

Refer to [CM_OPC_PD_ALARM_IND](@ref CM_OPC_PD_ALARM_IND_RSP).

Each indication must be acknowledged with the corresponding CM_OPC_xxx_RSP response.

The alarm indication resources should be provided before a PDEV parametrization.
If the CM-PD-user does not want CM-PD-Alarm.ind, then no resource should be provided.
CM-PD handles a resource shortage transparently. It stores the alarms and reports them as soon as resources are available again.
If the CM-PD-user wants to use the services PDPull or PDPlug, then at least one resource must be provided.
See @ref CM_OPC_PD_SUBMODULE_CONTROL.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK_CANCELLED        | The RQB is to be released. Refer to [PDDelete](@ref CM_OPC_PD_DELETE).
CM_ERR_SEQUENCE        | Request has not been made between CM_OPC_PD_CREATE and CM_OPC_PD_DELETE.
******************************************************************************/


/*****************************************************************************/
/* 035 CM_OPC_CL_DEVICE_PROVIDE_ALARM                                        */
/**************************************************************************//**
@defgroup CM_OPC_CL_DEVICE_PROVIDE_ALARM 035 CM_OPC_CL_DEVICE_PROVIDE_ALARM
@ingroup cm-cl-opcodes
@brief The AlarmProvide function is used to provide one alarm indication resource. With CM-CL this resource is assigned to the proper device.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr **REQ** Assign this event-resource to this device
@note Rest: don't care!

Indications change the opcode of the RQB as follows:

CM_OPC_CL_ALARM_IND refer to chapter @ref CM_OPC_CL_ALARM_IND_RSP.

Each indication must be acknowledged with the corresponding CM_OPC_xxx_RSP response.

There must be as many alarm-resources, as told with DeviceAdd.cnf, see there.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_ERR_PARAM           | Device doesn't exist.
CM_OK_CANCELLED        | The RQB is to be released. Refer to DeleteClient.
******************************************************************************/


/*****************************************************************************/
/* 036/037 CM_OPC_CL_ALARM_IND/_RSP                                          */
/**************************************************************************//**
@defgroup CM_OPC_CL_ALARM_IND_RSP 036/037 CM_OPC_CL_ALARM_IND/_RSP
@ingroup cm-cl-opcodes
@brief The reception of an alarm is signaled with Alarm.IND.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr                  **IND** device number
@param CM_ALARM_TYPE::ar_nr                      **IND** AR number
@param CM_ALARM_TYPE::session_key                **IND** session key, see @ref CM_OPC_CL_AR_OFFLINE_IND
@param CM_ALARM_TYPE::alarm_priority             **IND** CM_ALARM_PRIORITY_LOW or CM_ALARM_PRIORITY_HIGH, see text.
@param CM_ALARM_TYPE::api                        **IND** Application Process Identifier
@param CM_ALARM_TYPE::slot_nr                    **IND** Slot Number
@param CM_ALARM_TYPE::subslot_nr                 **IND** Subslot Number
@param CM_ALARM_TYPE::alarm_type                 **IND** see CM_ALARM_TYPE_PULL, CM_ALARM_TYPE_XXX,..., refer also to [/11/](@ref cm-ref-no-11)
@param CM_ALARM_TYPE::alarm_sequence             **IND** Sequence Number (0, 1, 2, ... 2047, 0, 1, 2, ...) \n
														 Note that the IO device may start with an arbitrary value (checking code changed with V5.2i4.5, AP01334245). \n
														 For system redundancy ARs (CM_AR_TYPE_SINGLE_SYSRED) the sequence number must be checked by the user (CM cannot do it). \n
														 See "AlarmSpecifier" bits 0-10. \n
														 Refer also to [/11/](@ref cm-ref-no-11).
@param CM_ALARM_TYPE::diag_channel_available     **IND** CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES: "appearing" channel diagnosis(es) exists. Otherwise CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::diag_generic_available     **IND** CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES: "appearing" or "disappearing" generic diagnosis exists (generic always contains channel diagnosis too). Otherwise CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::diag_submod_available      **IND** CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES: "appearing" generic or channel diagnosis available. Otherwise CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::reserved                   **IND** Don't evaluate
@param CM_ALARM_TYPE::ar_diagnosis_state         **IND** CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES: One or more diagnoses exist in the AR. Otherwise CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::mod_ident                  **IND** Module Ident Number
@param CM_ALARM_TYPE::sub_ident                  **IND** Submodule Ident Number
@param CM_ALARM_TYPE::alarm_tag                  **IND** The "AlarmItem::UserStructureIdentifier" (in host-byte-order), see @ref cm-rqbs-alarm-data.
@param CM_ALARM_TYPE::alarm_data_length          **IND** see @ref cm-rqbs-alarm-data, for maximum refer to @ref CM_OPC_CL_CREATE.
@param CM_ALARM_TYPE::alarm_data                 **IND** see @ref cm-rqbs-alarm-data
@param CM_ALARM_TYPE::cm_pnio_err                **RSP** Refer to the file cm_err.h
@param CM_ALARM_TYPE::maintenance_status         **IND** MaintenanceStatus
@param CM_ALARM_TYPE::u.cl.diff_list             **IND** In alarm type CM_ALARM_TYPE_PLUG or CM_ALARM_TYPE_RELEASED: \n
														 List with 0 or 1 submodule that does not correspond to the expected configuration. Refer to chapter @ref CM_OPC_CL_AR_IN_DATA_IND_RSP \n
														 In alarm type CM_ALARM_TYPE_MCR_MISMATCH: \n
														 List of the concerned submodule.
@param CM_ALARM_TYPE::u.cl.mod_diff_block_length **IND** In alarm type CM_ALARM_TYPE_PLUG or CM_ALARM_TYPE_RELEASED: \n
														 Length of the block mod_diff_block \n
														 AP00679788
@param CM_ALARM_TYPE::u.cl.mod_diff_block        **IND** In alarm type CM_ALARM_TYPE_PLUG or CM_ALARM_TYPE_RELEASED: \n
														 Block with all submodules that do not correspond to the reference configuration. \n
														 The structure of the bock is defined in [/11/](@ref cm-ref-no-11) "ModuleDiffBlock"

CM allocates a buffer of the maximum size "max_alarm_data_length" (refer to Channel Details) for "alarm_data". The AR is cancelled if the received data do not fit in the buffer (protocol error).

The parameter cm_pnio_err has to be set in the response

@note Prior to version v6.0i1.20 the parameter cm_pnio_err has been a parameter of CM_OPC_CL_ALARM_ACK_SEND.

## RTClass1/2

Alarms, which are received before INDATA, are queued internally. After InData.IND these alarms will be passed to the user.

## RTClass3

Alarms, which are received before INDATA, are queued internally. After PreData.IND these alarms will be passed to the user.
If no PreData.IND occurs, queued alarms will be passed to the user after InData.IND.


The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

## Alarm priority

On the device side, an alarm's priority is selected by CM-SV. See @ref CM_OPC_SV_AR_ALARM_SEND.

A controller can force a device to only send low priority alarms by setting CM_ALCR_PROP_PRIO_ONLY_LOW (Use cases: High priority not implemented on the controller side, the application program cannot process two priorities in parallel).

## Comment

The following behaviour is integrated according to AP00174491:
- An alarm to a non-projected submodule leads to an AR cancellation with reason code CM_AR_REASON_PROT.
- An alarm with a not supported alarm type is rejected CM-Internally, refer also to chapter 10.4.
- An alarm to a submodule that is not the state CM_SUB_STATE_IDENT_(OK|SUBSTITUTE), is rejected with the PNIO status CM_PNIO_ERR_AlarmAndWrongSubmoduleState (refer to cm_err.h).
- As of TIA1101169, if cm_pnio_err is CM_PNIO_ERR_AlarmAck_IOCARSR_backup and the CM_CL_DEVICE_PROPERTIES_CAN_SRD_BACKUP_ALARM bit is not set, the AR is aborted with CM_ERROR_CAUSE_SRD_ABORTED.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/


/*****************************************************************************/
/* 032 CM_OPC_CL_ALARM_SEND                                                  */
/**************************************************************************//**
@defgroup CM_OPC_CL_ALARM_SEND 032 CM_OPC_CL_ALARM_SEND
@ingroup cm-cl-opcodes
@brief The AlarmSend function is used to send an alarm. There are high and low priority alarms that are independent of each other.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr              **REQ** device number
@param CM_ALARM_TYPE::ar_nr                  **REQ** AR number
@param CM_ALARM_TYPE::session_key            **REQ** session key, see @ref CM_OPC_CL_AR_OFFLINE_IND
@param CM_ALARM_TYPE::alarm_priority         **REQ** Don't care, filled by CM \n **CNF** Current value.
@param CM_ALARM_TYPE::api                    **REQ** Application Process Identifier
@param CM_ALARM_TYPE::slot_nr                **REQ** Slot Number
@param CM_ALARM_TYPE::subslot_nr             **REQ** Subslot Number
@param CM_ALARM_TYPE::alarm_type             **REQ** Don't care, filled by CM.
@param CM_ALARM_TYPE::alarm_sequence         **REQ** Don't care, filled by CM. \n **CNF** Current value.
@param CM_ALARM_TYPE::diag_channel_available **REQ** Don't care, filled by CM. see "AlarmSpecifier" bit 11 \n **CNF** Current value.
@param CM_ALARM_TYPE::diag_generic_available **REQ** Don't care, filled by CM. see "AlarmSpecifier" bit 12 \n **CNF** Current value.
@param CM_ALARM_TYPE::diag_submod_available  **REQ** Don't care, filled by CM. see "AlarmSpecifier" bit 13 \n **CNF** Current value.
@param CM_ALARM_TYPE::reserved               **REQ** Don't care, filled by CM. see "AlarmSpecifier" bit 14 \n **CNF** 0
@param CM_ALARM_TYPE::ar_diagnosis_state     **REQ** Don't care, filled by CM. see "AlarmSpecifier" bit 15 \n **CNF** Current value.
@param CM_ALARM_TYPE::mod_ident              **REQ** Don't care, filled by CM. \n **CNF** Current value.
@param CM_ALARM_TYPE::sub_ident              **REQ** Don't care, filled by CM. \n **CNF** Current value.
@param CM_ALARM_TYPE::alarm_tag              **REQ** The "AlarmItem::UserStructureIdentifier" (in host-byte-order), see @ref cm-rqbs-alarm-data.
@param CM_ALARM_TYPE::alarm_data_length      **REQ** see @ref cm-rqbs-alarm-data, for maximum refer to @ref CM_OPC_CL_CREATE
@param CM_ALARM_TYPE::alarm_data             **REQ** see @ref cm-rqbs-alarm-data above
@param CM_ALARM_TYPE::cm_pnio_err            **CNF** Value from alarm ack, see CM_PNIO_ERR_MAKE()
@param CM_ALARM_TYPE::maintenance_status     **REQ** Don't care.

CM sets the priority as specified in [/11/](@ref cm-ref-no-11).

The meaning of the fields is described in chapter @ref cm-rqbs-alarm-data.

An alarm goes to the submodule of the AR.

If no session (i.e., AR) is available (e.g., because it has failed meanwhile), then the order is confirmed with CM_ERR_SESSION.

Network interactions:

SendAlarm is only confirmed if the corresponding alarm ack was received.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Alarm was sent and the corresponding alarm ack was received.
CM_ERR_ABORT           | Operation was cancelled (e.g., AR monitoring)
CM_ERR_SESSION         | This AR has been dropped or removed.
CM_ERR_SEQUENCE        | Call sequence not as documented.
CM_ERR_PARAM           | Invalid parameter (e.g., out-of-range)
CM_ERR_LOWER_LAYER     | The lower layer has detected an error
******************************************************************************/


/*****************************************************************************/
/* 108 CM_OPC_SV_DEVICE_PROVIDE_ALARM                                        */
/**************************************************************************//**
@defgroup CM_OPC_SV_DEVICE_PROVIDE_ALARM 108 CM_OPC_SV_DEVICE_PROVIDE_ALARM
@ingroup cm-sv-opcodes
@brief The request CM_OPC_SV_DEVICE_PROVIDE_ALARM provides an alarm indication-resource to the device.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr **REQ** Device number
@note Rest: don't care!

The user has to provide "nr_of_alarm_resources" as returned from @ref CM_OPC_SV_DEVICE_ADD.

The request is cancelled when the device is removed. The request is returned to the user with CM_OK_CANCELLED before the device-remove request is confirmed.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK_CANCELLED        | Out-parameters are invalid.
CM_ERR_SEQUENCE        | CM-SV is not created. Device is not added.
******************************************************************************/


/*****************************************************************************/
/* 126 CM_OPC_SV_AR_ALARM_SEND                                               */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_ALARM_SEND 126 CM_OPC_SV_AR_ALARM_SEND
@ingroup cm-sv-opcodes
@brief The request CM_OPC_SV_AR_ALARM_SEND sends an alarm. An alarm is either sent with high or low priority. The two priorities form two queues that are independent of each other.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr              **REQ** device number
@param CM_ALARM_TYPE::ar_nr                  **REQ** set to CM_SV_SESSION_KEY_TO_AR_NR(session_key)
@param CM_ALARM_TYPE::session_key            **REQ** session key, see @ref CM_OPC_SV_AR_CONNECT_IND
@param CM_ALARM_TYPE::alarm_priority         **CNF** Current value, see text.
@param CM_ALARM_TYPE::api                    **REQ** Application Process Identifier
@param CM_ALARM_TYPE::slot_nr                **REQ** Slot Number
@param CM_ALARM_TYPE::subslot_nr             **REQ** Subslot Number
@param CM_ALARM_TYPE::alarm_type             **REQ** see CM_ALARM_TYPE_PULL, CM_ALARM_TYPE_XXX,..., refer also to [/11/](@ref cm-ref-no-11)
@param CM_ALARM_TYPE::alarm_sequence         **CNF** Current value.
@param CM_ALARM_TYPE::diag_channel_available **CNF** Current value.
@param CM_ALARM_TYPE::diag_generic_available **CNF** Current value.
@param CM_ALARM_TYPE::diag_submod_available  **CNF** Current value.
@param CM_ALARM_TYPE::reserved               **CNF** 0
@param CM_ALARM_TYPE::ar_diagnosis_state     **CNF** Current value.
@param CM_ALARM_TYPE::mod_ident              **CNF** Current value.
@param CM_ALARM_TYPE::sub_ident              **CNF** Current value.
@param CM_ALARM_TYPE::alarm_tag              **REQ** The "AlarmItem::UserStructureIdentifier" (in host-byte-order), see @ref cm-rqbs-alarm-send.
@param CM_ALARM_TYPE::alarm_data_length      **REQ** Length of "AlarmItem::Data*". Refer to @ref cm-rqbs-alarm-send. \n
													 See @ref CM_OPC_SV_AR_CONNECT_IND, Maximum is CM_SV_AR_CONNECT_TYPE::alarm_send_max_length. \n
													 For the alarm types of the CM_ALARM_DIAG_GROUP, the maximum length is 14 bytes shorter (MaintenanceItem + USI, see [/11/](@ref cm-ref-no-11)). \n
													 See too @ref CM_OPC_SV_DEVICE_ADD, parameter "alarm_send_max_length".
@param CM_ALARM_TYPE::alarm_data             **REQ** see @ref cm-rqbs-alarm-data above \n
													 Pointer to "AlarmItem::Data*". \n
													 Can be LSA_NULL if alarm_data_length == 0. \n
													 For "AlarmNotification-PDU" see chapter @ref cm-rqbs-alarm-notification-pdu or refer to [/11/](@ref cm-ref-no-11).
@param CM_ALARM_TYPE::cm_pnio_err            **CNF** Value from the alarm-acknowledge.
@param CM_ALARM_TYPE::maintenance_status     **CNF** Current value.

The priority is selected internally by CM-SV according to [/11/](@ref cm-ref-no-11): A process alarm is sent with high priority, all other alarms with low priority. However, the IO controller can override this per AR by setting the AlarmCRProperties to "use only low priority" in which case all alarms are sent with low priority. History: The logic was changed with V6.0i2.10 (TIA 1166054).

An alarm is accepted (i.e., queued for sending) if:

 - the submodule is plugged
 - the submodule is "owned"
 - the submodule is "ok" or "substitute"
 - the submodule is "application-ready" or "application-ready-pending"
 - the AR is not disconnecting.

Otherwise, the alarm is rejected.

A queued alarm is cancelled if:

 - the submodule is pulled (see chapter @ref CM_OPC_SV_SUBMODULE_REMOVE)
 - the submodule is "taken over" by an IO supervisor AR.
 - the AR is disconnecting.

@todo CM documentation Prinzp "Stauen an der Quelle" kurz beschreiben. Nicht die Queue anfüllen.

@see @ref CM_ALARM_TYPE

## Return-of-submodule alarm

A return-of-submodule alarm signals a change of IOPS/IOCS from BAD to GOOD. A return-of-submodule alarm must be sent only after the buffer-update took effect.

If for example the EDDI is configured with EDDI_CFG_USE_SW_IIIB_IF_PROV, the return-of-submodule alarms must be sent only after a successful eddi_ProviderBufferRequest_Xchange().

If the submodule was "application-ready-pending", the return-of-submodule alarm implicitly changes the state to "application-ready" (AP01054520).

Note that a return-of-submodule alarm does not trigger an extra in-data indication, because a return-of-submodule alarm is completed only after a new-data event (see chapter @ref cm-product-architecture-behavior-sequence-diagrams-return-sub-alarm and [/10/](@ref cm-ref-no-10)).

RTC3-AR before RIR-Acknowledge: Although it is principally ok to send a return-of-submodule (if the buffer-update works in this phase) it should not be done.

## Case: no sync master (RTC3 frames are not forwarded) and case: advanced startup mode (one provider mode)

IO Controller          |                     | IO Device
-----------------------|---------------------|--------------------------------------
Connect, Write, PrmEnd |                     | Connect, Write, PrmEnd
|                      | ...                 |                                     |
|                      | no frames           |                                     |
|                      | ...                 |                                     |
Not "received in red" thus not waiting for new data (would block) -> Inputs are not new! | <- Application Ready RPC     | Update Inputs (if possible)
Outputs = BAD          | cnf ->              | Not "received in red" thus not waiting for new data (would block) -> Outputs are not new!
|                      | ...                 |                                     |
|                      | no frames           |                                     |
|                      | ...                 |                                     |
Not "received in red" thus not waiting for new data (would block) -> Inputs are not new! | <- Return-of-Submodule alarm | IOPS changed to GOOD
Outputs = BAD          |Alarm acknowledge -> | Not "received in red" thus not waiting for new data (would block) -> Outputs are not new! Note: The problem is that the Outputs do not correspond to the Inputs, because the new-data handshake is skipped.

## Case: sync master comes and RTC3 frames are forwarded

IO Controller  |                        | IO Device
---------------|------------------------|--------------------------------------
|              | frames ->              |                                     |
|              | frames ->              | ReceivedInRed.IND
|              | frames ->              | ReceivedInRed.ACK
|              | frames -> \n <- frames | -> wait for new data \n-> InData.IND
received in red (internal) \n -> wait for new data \n ->InData.IND ("return of station") \n Note: The inputs with IOPS == GOOD will be seen here. frames | frames -> \n <- frames | |
|              | frames -> \n <- frames | -> wait for new data \n-> InData.IND

## Diagnosis alarm

For the alarm types of the CM_ALARM_DIAG_GROUP, the maximum length is 14 bytes shorter than CM_SV_AR_CONNECT_TYPE::alarm_send_max_length (MaintenanceItem + USI, see [/11/](@ref cm-ref-no-11)).

Usage according to [/10/](@ref cm-ref-no-10):

CM_ALARM_TYPE_DIAGNOSIS shall be used to
 - indicate appearing or disappearing items for one or more channels.

CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS shall only be used to
 - indicate all disappearing without an Alarm Item (optimization)
 - indicate detailed disappearing items

Several individual diagnoses can be transported in a single alarm with the help of the well-defined alarms "ChannelDiagnosis" (alarm_tag = 0x8000), "Multiple" (alarm_tag = 0x8001), "ExtChannelDiagnosis" (alarm_tag = 0x8002) and "QualifiedChannelDiagnosis" (alarm_tag = 0x8003).

Refer to [/11/](@ref cm-ref-no-11) for the details. Especially the meaning of ChannelProperties.Specifier == "Disappears but other remain" (see @ref cm-channel-properties).

## Reporting system alarm

An RS alarm must address api 0, slot_nr 0x8000 and subslot_nr 0x0000. RS alarms are supported only on ARs with "has_RSInfoBlock == CM_SV_HAS_RSINFOBLOCK_YES" (see chapter @ref CM_OPC_SV_AR_CONNECT_IND ).

## Network interactions

The request is confirmed with the receiving of the corresponding alarm-acknowledge.

@todo CM documentation BILD machen! Es gibt so was für return-of-submodule @ref cm-product-architecture-behavior-sequence-diagrams-return-sub-alarm.

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-alarm-send. \n
NAP-Type R1: See additional information in chapter @ref cm-server-channels-r-device-alarm-send.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success. Alarm-Acknowledge received.
CM_OK_CANCELLED        | Not an error. The alarm-processing was started, but it was interrupted before the alarm-acknowledge was received: the AR was aborted, or the AR is already disconnecting, or the submodule is not yet parameterized (i.e., before application-ready (-pending) from user), or the submodule was pulled, or an IO-supervisor AR took over control, or SR-AR is "first" (TIA 535510), etc.
CM_OK_R1_NOT_ACKED     | Not an error. Only for R1 device. Please refer to chapter @ref cm-server-channels-r-device-alarm-send
CM_ERR_SEQUENCE        | CM-SV is not created. Device is not added. AR is not connected (before connect.RSP or after disconnect.RSP).
CM_ERR_PARAM           | Parameter(s) out of range. Submodule is not in AR.
CM_ERR_FORMAT          | Alarm_type, alarm_tag (UserStructureIdentifier) and format of alarm_data are not allowed in this combination. Refer to [/11/](@ref cm-ref-no-11).
******************************************************************************/


/*****************************************************************************/
/* 127/128 CM_OPC_SV_AR_ALARM_IND/_RSP                                       */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_ALARM_IND_RSP 127/128 CM_OPC_SV_AR_ALARM_IND/_RSP
@ingroup cm-sv-opcodes
@brief The indication CM_OPC_SV_AR_ALARM_IND signals the reception of an alarm. The indication has to be responded.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr                  **IND** device number
@param CM_ALARM_TYPE::ar_nr                      **IND** = CM_SV_SESSION_KEY_TO_AR_NR(session_key)
@param CM_ALARM_TYPE::session_key                **IND** Session key, refer to [CM_OPC_SV_AR_CONNECT_IND](@ref CM_OPC_SV_AR_CONNECT_IND_RSP)
@param CM_ALARM_TYPE::alarm_priority             **IND** CM_ALARM_PRIORITY_LOW or CM_ALARM_PRIORITY_HIGH
@param CM_ALARM_TYPE::api                        **IND** Application Process Identifier
@param CM_ALARM_TYPE::slot_nr                    **IND** Slot Number
@param CM_ALARM_TYPE::subslot_nr                 **IND** Subslot Number
@param CM_ALARM_TYPE::alarm_type                 **IND** see CM_ALARM_TYPE_PULL, CM_ALARM_TYPE_XXX,..., refer also to [/11/](@ref cm-ref-no-11)
@param CM_ALARM_TYPE::alarm_sequence             **IND** Sequence Number (0, 1, 2, ... 2047, 0, 1, 2, ...) \n
														 Note that the IO device may start with an arbitrary value (checking code changed with V5.2i4.5, AP01334245). \n
														 For system redundancy ARs (CM_AR_TYPE_SINGLE_SYSRED) the sequence number must be checked by the user (CM cannot do it). \n
														 See "AlarmSpecifier" bits 0-10. \n
														 Refer also to [/11/](@ref cm-ref-no-11).
@param CM_ALARM_TYPE::diag_channel_available     **IND** CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::diag_generic_available     **IND** CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::diag_submod_available      **IND** CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::reserved                   **IND** Don't evaluate
@param CM_ALARM_TYPE::ar_diagnosis_state         **IND** CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES..One or more diagnoses exist in the AR. Otherwise CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO
@param CM_ALARM_TYPE::mod_ident                  **IND** Module Ident Number
@param CM_ALARM_TYPE::sub_ident                  **IND** Submodule Ident Number
@param CM_ALARM_TYPE::alarm_tag                  **IND** The "AlarmItem::UserStructureIdentifier" (in host-byte-order), see @ref cm-rqbs-alarm-data.
@param CM_ALARM_TYPE::alarm_data_length          **IND** see @ref cm-rqbs-alarm-data
@param CM_ALARM_TYPE::alarm_data                 **IND** see @ref cm-rqbs-alarm-data
@param CM_ALARM_TYPE::cm_pnio_err                        Don't care.
@param CM_ALARM_TYPE::maintenance_status                 Don't care.

An alarm has to be acknowledged with CM_OPC_SV_AR_ALARM_ACK.

The functionality "alarm responder" must be enabled via CM_OPC_SV_CREATE flag "enable_AlarmResponder". See chapter @ref CM_OPC_SV_CREATE.

CM allocates buffers of size "CM_DETAIL_TYPE::u.sv.acp.max_alarm_data_length" for "alarm_data". The AR is aborted if the received data does not fit into the buffer (protocol error).

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. an AR-abort.
The user has to handle the "late" indication gracefully.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/


/*****************************************************************************/
/* 129 CM_OPC_SV_AR_ALARM_ACK                                                */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_ALARM_ACK 129 CM_OPC_SV_AR_ALARM_ACK (alarm responder)
@ingroup cm-sv-opcodes
@brief The request CM_OPC_SV_AR_ALARM_ACK sends an alarm-acknowledge.
@details
@see @ref CM_ALARM_TYPE
@param CM_ALARM_TYPE::device_nr              **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::ar_nr                  **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::session_key            **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::alarm_priority         **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::api                    **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::slot_nr                **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::subslot_nr             **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::alarm_type             **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::alarm_sequence         **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::diag_channel_available **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::diag_generic_available **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::diag_submod_available  **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::ar_diagnosis_state     **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::reserved               **REQ** Same as in [CM_OPC_SV_AR_ALARM_IND](@ref CM_OPC_SV_AR_ALARM_IND_RSP).
@param CM_ALARM_TYPE::mod_ident                      Don't care
@param CM_ALARM_TYPE::sub_ident                      Don't care
@param CM_ALARM_TYPE::alarm_tag                      Don't care
@param CM_ALARM_TYPE::alarm_data_length              Don't care
@param CM_ALARM_TYPE::alarm_data                     Don't care
@param CM_ALARM_TYPE::cm_pnio_err            **REQ** Refer to file cm_err.h.
@param CM_ALARM_TYPE::maintenance_status             Don't care

## Sequence

An alarm-acknowledge must be sent only after the [CM_OPC_SV_AR_ALARM_RSP](@ref CM_OPC_SV_AR_ALARM_IND_RSP). This sequence ensures that exactly 2 alarm indication-resources per AR are sufficient.

## Network interactions

ArAlarmAck is completed when the transport-ack is received.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/


/*****************************************************************************/
/* CM_ALARM_TYPE                                                             */
/**************************************************************************//**
@defgroup CM_ALARM_TYPE CM_ALARM_TYPE
@ingroup cm-rqbs-group
@ingroup CM_OPC_PD_PROVIDE_ALARM
@ingroup CM_OPC_PD_ALARM_IND_RSP
@ingroup CM_OPC_CL_DEVICE_PROVIDE_ALARM
@ingroup CM_OPC_CL_ALARM_IND_RSP
@ingroup CM_OPC_CL_ALARM_SEND
@ingroup CM_OPC_SV_DEVICE_PROVIDE_ALARM
@ingroup CM_OPC_SV_AR_ALARM_SEND
@ingroup CM_OPC_SV_AR_ALARM_IND_RSP
@ingroup CM_OPC_SV_AR_ALARM_ACK
@{
@brief The CM_ALARM_TYPE forms the arguments ('args') of an alarm indication or an alarm request.
@details
The individual fields are declared for each alarm Opcode.

# Parallel operation of alarms and alarm acknowledgments

n SendAlarm requests can be set corresponding to one date/time; CM serialized.

Exactly one high and one low alarm indication can run corresponding to each date/time. The current alarm must be acknowledged before another alarm can be sent.

As a consequence, exactly the same number (i.e., one high and 1 low) of SendAlarmAck requests can be set.

# alarm_data and alarm_data_length # {#cm-rqbs-alarm-data}

Sending and receiving with reference to the alarm_data and alarm_data_length fields is not symmetrical on account of the requirement that the raw AlarmNotification PDU (Refer to [/11/](@ref cm-ref-no-11)) must be visible.

## AlarmNotification PDU ## {#cm-rqbs-alarm-notification-pdu}

The byte order in the AlarmNotification PDU is Big Endian.

Extract from [/11/](@ref cm-ref-no-11):

Substitution name     | Structure
----------------------|------------------------------------------------------------------------------------------------------------------------------
AlarmNotification PDU | BlockHeader, AlarmType, API, SlotNumber, SubslotNumber, ModuleIdentNumber, SubmoduleIdentNumber, AlarmSpecifier, AlarmPayload
AlarmPayload          | [MaintenanceItem], [DiagnosisItem] ^ [AlarmItem] ^ [Upload&RetrievalItem] ^ [iParameterItem] ^ [PE_AlarmItem]
AlarmItem             | UserStructureIdentifier, Data*

![AlarmNotification PDU](cm-alarm-notification-PDU.png)

## Sending alarm ## {#cm-rqbs-alarm-send}

The AlarmNotification PDU built by CM (and ACP) from the alarm RQB (marshalling). The byte order of all the fields of RQB - up to the alarm_data - is the host byte order.

The data of the "UserStructureIdentifier" comes from the "alarm_tag" field.

The data of "Data" comes from the "alarm_data" and it is transferred as it is.

The length of "Data" comes from the "alarm_data_length" field.

The block type is derived from the alarm priority. BlockVersion is a constant.

The remainder comes from the corresponding fields of the Alarm RQB.

@attention ACP does not transfer any "AlarmItem" at present if the length of the data is 0.

## Receiving alarm

The AlarmNotification PDU is unpacked by the CM (and ACP) in the alarm RQB (unmarshalling). The byte order of all the fields of the RQB - up to alarm_data - is the host byte order.

"alarm_data" shows the start of the AlarmNotification PDU.

"alarm_data_length" contains the length of the total AlarmNotification PDU.

"Data" is transferred as it is.

@attention If the length of data is 0, it is interpreted currently by ACP as no "AlarmItem" available.

The following constants are defined to access the raw PDU:

Constant                  | Description
--------------------------|------------------------------------
CM_ALARM_OFFSET_USI       | Offset of "UserStructureIdentifier"
CM_ALARM_OFFSET_DATA      | Offset of "Data"
CM_ALARM_OFFSET_SPECIFIER | Offset of "AlarmSpecifier"

Pseudo code:

@code
if( alarm_data_length == CM_ALARM_HEADER_LENGTH )
{
	// AlarmItem (USI + DATA) not available
	// The field CM_ALARM_TYPE::alarm_tag is not valid.
	AlarmItem::USI  := non existent
	AlarmItem::DATA := non existent
}
else
{
	// AlarmItem (USI + DATA) available
	// The field CM_ALARM_TYPE::alarm_tag is valid.
	LengthOfAlarmItem := alarm_data_length - CM_ALARM_HEADER_LENGTH
	PointerToRawAlarmItem::USI := alarm_data + CM_ALARM_OFFSET_USI
	if( AlarmItem::USI == MULTIPLE )
	{
		PointerToBlock := alarm_data + CM_ALARM_OFFSET_DATA
		for each block
		{
			decode block
			PointerToBlock += length-of-block
		}
		assert(PointerToBlock == alarm_data + alarm_data_length)
	}
	else
	{
		LengthOfRawData  := alarm_data_length - CM_ALARM_OFFSET_DATA
		PointerToRawData := alarm_data        + CM_ALARM_OFFSET_DATA
	}
}
@endcode
******************************************************************************/
#define CM_ALARM_PRIORITY_LOW   ACP_ALARM_PRIORITY_LOW
#define CM_ALARM_PRIORITY_HIGH  ACP_ALARM_PRIORITY_HIGH

/*
0x0000 reserved
*/
#define CM_ALARM_TYPE_DIAGNOSIS                  0x0001
#define CM_ALARM_TYPE_PROCESS                    0x0002
#define CM_ALARM_TYPE_PULL                       0x0003
#define CM_ALARM_TYPE_PLUG                       0x0004
#define CM_ALARM_TYPE_STATUS                     0x0005
#define CM_ALARM_TYPE_UPDATE                     0x0006
#define CM_ALARM_TYPE_MEDIA_REDUNDANCY           0x0007
#define CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR   0x0008
#define CM_ALARM_TYPE_RELEASED                   0x0009 /**< old: RELEASED_BY_SUPERVISOR */
#define CM_ALARM_TYPE_PLUG_WRONG                 0x000A
#define CM_ALARM_TYPE_RETURN_OF_SUBMODULE        0x000B
#define CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS       0x000C
#define CM_ALARM_TYPE_MCR_MISMATCH               0x000D /**< Multicast Communication Mismatch */
#define CM_ALARM_TYPE_PORT_DATA_CHANGED          0x000E
#define CM_ALARM_TYPE_SYNC_DATA_CHANGED          0x000F
#define CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM     0x0010
#define CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM  0x0011
#define CM_ALARM_TYPE_TIME_DATA_CHANGED          0x0012
#define CM_ALARM_TYPE_DFP_PROBLEM                0x0013
#define CM_ALARM_TYPE_MRPD_PROBLEM               0x0014
#define CM_ALARM_TYPE_reserved_0x0015            0x0015
#define CM_ALARM_TYPE_MULTIPLE_INTERFACE         0x0016 /**< Multiple interface mismatch notification */
#define CM_ALARM_TYPE_reserved_0x0017            0x0017
#define CM_ALARM_TYPE_reserved_0x0018            0x0018
#define CM_ALARM_TYPE_reserved_0x0019            0x0019
#define CM_ALARM_TYPE_reserved_0x001A            0x001A
#define CM_ALARM_TYPE_reserved_0x001B            0x001B
#define CM_ALARM_TYPE_reserved_0x001C            0x001C
#define CM_ALARM_TYPE_reserved_0x001D            0x001D
#define CM_ALARM_TYPE_UPLOAD_AND_STORAGE         0x001E
#define CM_ALARM_TYPE_PULL_MODULE                0x001F /**< see AP00517543 */
/*
0x0020 - 0x007F manufacturer specific
0x0080 - 0x00FF reserved for profiles
0x0100 - 0xFFFF reserved
*/

#define CM_ALARM_MANUFACTURER_SPECIFIC_GROUP \
			( (1UL << 0) /* using bit 0 for the whole range 0x0020 - 0x007F */ )

#define CM_ALARM_RESERVED_GROUP \
			( (1UL << CM_ALARM_TYPE_reserved_0x0015) \
			| (1UL << CM_ALARM_TYPE_reserved_0x0017) \
			| (1UL << CM_ALARM_TYPE_reserved_0x0018) \
			| (1UL << CM_ALARM_TYPE_reserved_0x0019) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001A) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001B) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001C) \
			| (1UL << CM_ALARM_TYPE_reserved_0x001D) \
			)

#define CM_ALARM_PLUG_GROUP \
			( (1UL << CM_ALARM_TYPE_PULL) \
			| (1UL << CM_ALARM_TYPE_PLUG) \
			| (1UL << CM_ALARM_TYPE_CONTROLLED_BY_SUPERVISOR) \
			| (1UL << CM_ALARM_TYPE_RELEASED) \
			| (1UL << CM_ALARM_TYPE_PLUG_WRONG) \
			| (1UL << CM_ALARM_TYPE_RETURN_OF_SUBMODULE) \
			| (1UL << CM_ALARM_TYPE_PULL_MODULE) \
			)

#define CM_ALARM_DIAG_GROUP \
			( (1UL << CM_ALARM_TYPE_DIAGNOSIS) \
			| (1UL << CM_ALARM_TYPE_DIAGNOSIS_DISAPPEARS) \
			| (1UL << CM_ALARM_TYPE_MEDIA_REDUNDANCY) \
			| (1UL << CM_ALARM_TYPE_MCR_MISMATCH) \
			| (1UL << CM_ALARM_TYPE_PORT_DATA_CHANGED) \
			| (1UL << CM_ALARM_TYPE_SYNC_DATA_CHANGED) \
			| (1UL << CM_ALARM_TYPE_ISOCHRONE_MODE_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_NETWORK_COMPONENT_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_TIME_DATA_CHANGED) \
			| (1UL << CM_ALARM_TYPE_DFP_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_MRPD_PROBLEM) \
			| (1UL << CM_ALARM_TYPE_MULTIPLE_INTERFACE) \
			)

#define CM_ALARM_IS_PLUG_GROUP(alarm_type) ((alarm_type) <= 31/*!*/ && ((1UL << (alarm_type)) & CM_ALARM_PLUG_GROUP) != 0)
#define CM_ALARM_IS_DIAG_GROUP(alarm_type) ((alarm_type) <= 31/*!*/ && ((1UL << (alarm_type)) & CM_ALARM_DIAG_GROUP) != 0)

#define CM_ALARM_DATA_LENGTH_MIN   200  /**< min. size of the AlarmNotification-PDU, see part 6 */
#define CM_ALARM_DATA_LENGTH_MAX   1432 /**< max. size of the AlarmNotification-PDU, see part 6 */

#define CM_ALARM_HEADER_LENGTH     (ACP_ALARM_USER_DATA_OFFSET - 2) /**< = 2+2+1+1 + 2 + 4+2+2 + 4+4 + 2 = 26 */

#define CM_ALARM_PAYLOAD_MIN       (CM_ALARM_DATA_LENGTH_MIN - CM_ALARM_HEADER_LENGTH) /**< 200 - 26 = 174 = USI + 172 bytes */
#define CM_ALARM_PAYLOAD_MAX       (CM_ALARM_DATA_LENGTH_MAX - CM_ALARM_HEADER_LENGTH) /**< 1432 - 26 = 1406 = USI + 1404 bytes */
#define CM_ALARM_PAYLOAD_MAX_woUSI (CM_ALARM_PAYLOAD_MAX - 2) /**< max. size of "AlarmNotification-PDU::AlarmPayload" without USI (1404 bytes) */

#define CM_ALARM_MAINTENANCEITEM_LENGTH (2 + 2+2+1+1 + 1+1 + 4)

#define CM_ALARM_OFFSET_USI        (CM_ALARM_HEADER_LENGTH)
#define CM_ALARM_OFFSET_DATA       (CM_ALARM_HEADER_LENGTH + 2)
#define CM_ALARM_OFFSET_SPECIFIER  (CM_ALARM_HEADER_LENGTH - 2)
/**< ## see PNIO-Spec for the definition of AlarmNotification-PDU and AlarmItem
 * the min. size of the AlarmNotification-PDU is CM_ALARM_DATA_LENGTH_MIN \n
 * the max. size of the AlarmNotification-PDU is CM_ALARM_DATA_LENGTH_MAX
 *
 * ## CM_OPC_CL_ALARM_SEND and CM_OPC_SV_AR_ALARM_SEND
 * - alarm_data ......... points to AlarmItem::DATA
 * - alarm_data_length .. is the length of AlarmItem::DATA
 * - alarm_tag .......... is the AlarmItem::UserStructureIdentifier (in host-byte-order), it is used only if alarm_data_length != 0, else "don't care".
 *
 * ## CM_OPC_CL_ALARM_IND and CM_OPC_SV_AR_ALARM_IND
 * - alarm_data ......... points to AlarmNotification-PDU to allow accessing the "raw" data
 * - alarm_data_length .. is the length of the AlarmNotification-PDU
 * - alarm_tag .......... is the AlarmItem::UserStructureIdentifier (in host-byte-order)
 *
 * alarm_data + CM_ALARM_OFFSET_USI ... points to the "raw" AlarmItem::UserStructureIdentifier \n
 * alarm_data + CM_ALARM_OFFSET_DATA .. points to the "raw" AlarmItem::DATA \n
 * alarm_tag, AlarmItem::UserStructureIdentifier and AlarmItem::DATA are valid only if alarm_data_length >= CM_ALARM_OFFSET_DATA
 */

#define CM_ALARM_USI_MAINTENANCE  0x8100

#define CM_MAINTENANCE_STATUS_REQUIRED  0x00000001
#define CM_MAINTENANCE_STATUS_DEMANDED  0x00000002
#define CM_MAINTENANCE_STATUS_RESERVED  0x00000004 /**< reserved since PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01545793 item 11 */
#define CM_MAINTENANCE_STATUS_QUA_BITS  0xFFFFFFF8 /**< see QualifiedChannelQualifier */

#define CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_NO  0
#define CM_ALARM_SPECIFIER_CHANNEL_DIAGNOSIS_AVAILABLE_YES 1

#define CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_NO  0
#define CM_ALARM_SPECIFIER_GENERIC_DIAGNOSIS_AVAILABLE_YES 1

#define CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_NO  0
#define CM_ALARM_SPECIFIER_SUBMODULE_DIAGNOSIS_AVAILABLE_YES 1

#define CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_NO  0
#define CM_ALARM_SPECIFIER_AR_DIAGNOSIS_AVAILABLE_YES 1

/** The type used for CM_OPC_PD_PROVIDE_ALARM, CM_OPC_PD_ALARM_*, CM_OPC_CL_DEVICE_PROVIDE_ALARM, CM_OPC_CL_ALARM_*, CM_OPC_SV_DEVICE_PROVIDE_ALARM, CM_OPC_SV_AR_ALARM_*. */
typedef struct cm_alarm_tag {

	LSA_UINT16 device_nr;
	/**< device number */

	LSA_UINT16 ar_nr;
	/**< ar number */

	LSA_UINT16 session_key;
	/**< session key, see CL_EVENT_AR_OFFLINE_IND or CM_OPC_SV_AR_CONNECT_IND */

	LSA_UINT16 alarm_priority;
	/**< CM_ALARM_PRIORITY_LOW or CM_ALARM_PRIORITY_HIGH */

	LSA_UINT32 api;
	/**< application process identifier */

	LSA_UINT16 slot_nr;
	/**< slot number */

	LSA_UINT16 subslot_nr;
	/**< subslot number */

	LSA_UINT16 alarm_type;
	/**< see CM_ALARM_TYPE_... */

	LSA_UINT16 alarm_sequence;
	/**< see "AlarmSpecifier" bits 0-10 */

	LSA_UINT8 diag_channel_available;
	/**< see "AlarmSpecifier" bit 11 */

	LSA_UINT8 diag_generic_available;
	/**< see "AlarmSpecifier" bit 12 */

	LSA_UINT8 diag_submod_available;
	/**< see "AlarmSpecifier" bit 13 */

	LSA_UINT8 reserved;
	/**< see "AlarmSpecifier" bit 14 */

	LSA_UINT8 ar_diagnosis_state;
	/**< see "AlarmSpecifier" bit 15 */


	LSA_UINT32 mod_ident;
	LSA_UINT32 sub_ident;

	LSA_UINT16 alarm_tag;
	/**< see comment on CM_ALARM_OFFSET_DATA above */

	LSA_UINT16 alarm_data_length;
	/**< see comment on CM_ALARM_OFFSET_DATA above */

	CM_COMMON_MEM_U8_PTR_TYPE alarm_data;
	/**< see comment on CM_ALARM_OFFSET_DATA above */

	LSA_UINT32 cm_pnio_err;
	/**< see the macro CM_PNIO_ERR_MAKE() in file cm_err.h \n
	 * alarm.req: don't care \n
	 * alarm.cnf: see CM_PNIO_ERR_MAKE() \n
	 * alarm.IND see CM_PNIO_ERR_MAKE() \n
	 * alarm.rsp: don't care
	 */


	LSA_UINT32 maintenance_status;
	/**< see CM_MAINTENANCE_STATUS_xxx \n
	 * alarm.req: don't care \n
	 * alarm.cnf: current status \n
	 * alarm.IND from the maintenance extension (USI 0x8100) \n
	 *            BlockType := 0x0F00 \n
	 *            BlockVersion := 1.0 \n
	 *            BlockLength := 4 \n
	 *            Value := U32 (Bitfield) \n
	 * alarm.rsp: don't change (same as in alarm.ind)
	 */

	union {

		struct {
			LSA_UINT32 nothing_so_far; /**< do not evaluate */
		} pd;

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
		struct {

			CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag)  diff_list; /**< diff_list */
			LSA_UINT16 mod_diff_block_length; /**< AP00679788 */
			CM_UPPER_MEM_PTR_TYPE  mod_diff_block; /**< mod_diff_block */

		} cl; /**< only valid with (alarm_type == CM_ALARM_TYPE_PLUG or CM_ALARM_TYPE_RELEASED) */
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
		struct {
			LSA_VOID_PTR_TYPE void_ptr; /**< do not evaluate (used internally) */
		} sv;
#endif
	} u;

} CM_ALARM_TYPE;

typedef CM_ALARM_TYPE *CM_UPPER_ALARM_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_PROVIDE_ALARM, CM_OPC_PD_ALARM_*, CM_OPC_CL_DEVICE_PROVIDE_ALARM, CM_OPC_CL_ALARM_*, CM_OPC_SV_DEVICE_PROVIDE_ALARM, CM_OPC_SV_AR_ALARM_*. */

typedef CM_ALARM_TYPE const *CM_UPPER_ALARM_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_PD_PROVIDE_ALARM, CM_OPC_PD_ALARM_*, CM_OPC_CL_DEVICE_PROVIDE_ALARM, CM_OPC_CL_ALARM_*, CM_OPC_SV_DEVICE_PROVIDE_ALARM, CM_OPC_SV_AR_ALARM_*. */

typedef CM_UPPER_ALARM_PTR_TYPE CM_UPPER_CL_ALARM_SEND_PTR_TYPE; /**< just an alias @deprecated Use CM_UPPER_ALARM_PTR_TYPE! */
/** @} ***********************************************************************/


/******************************************************************************
module- and submodule-states
******************************************************************************/

enum cm_mod_state_enum {
	CM_MOD_STATE_NO_MODULE           = 0,
	CM_MOD_STATE_WRONG_MODULE        = 1,
	CM_MOD_STATE_PROPER_MODULE       = 2,
	CM_MOD_STATE_SUBSTITUTED_MODULE  = 3,
	/***/
	CM_MOD_STATE_RESERVED
};

enum cm_sub_state_enum {

	/* PNIO V2.0 defines a new format for the submodule-state */

	CM_SUB_STATE_FORMAT_MASK                    = 0x8000,
	CM_SUB_STATE_FORMAT_0                       = 0x0000, /* note: only for backward-compatibility */
	CM_SUB_STATE_FORMAT_1                       = 0x8000,

	/* the format 0 definitions - DO NOT USE ANYMORE! */

	CM_SUB_STATE_DETAIL_MASK                    = 0x0007, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_NO_SUBMODULE            = 0x0000, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_WRONG_SUBMODULE         = 0x0001, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_LOCKED_IOCONTROLLER     = 0x0002, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_RESERVED_3              = 0x0003, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_APPL_READY_PENDING      = 0x0004, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_RESERVED_5              = 0x0005, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_RESERVED_6              = 0x0006, /* DO NOT USE ANYMORE! */
	CM_SUB_STATE_DETAIL_SUBSTITUTED_SUBMODULE   = 0x0007, /* DO NOT USE ANYMORE! */

	/* the format 1 definitions */

	CM_SUB_STATE_ADD_INFO_MASK                  = 0x0007, /* Bit 0 - 2: SubmoduleState.AddInfo */
	CM_SUB_STATE_ADD_INFO_NONE                  = 0x0000,
	CM_SUB_STATE_ADD_INFO_TAKEOVER_NOT_ALLOWED  = 0x0001,

	CM_SUB_STATE_ADVICE_MASK                    = 0x0008, /* Bit 3: SubmoduleState.Advice */
	CM_SUB_STATE_ADVICE_NOT_AVAILABLE           = 0x0000,
#if 0 /* future requirement, see TIA 1376282 */
	CM_SUB_STATE_ADVICE_AVAILABLE               = 0x0008,
#endif

	CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK      = 0x0010, /* Bit 4: SubmoduleState.MaintenanceRequired */
	CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED       = 0x0000,
	CM_SUB_STATE_MAINTENANCE_REQUIRED           = 0x0010,

	CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK      = 0x0020, /* Bit 5: SubmoduleState.MaintenanceDemanded */
	CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED       = 0x0000,
	CM_SUB_STATE_MAINTENANCE_DEMANDED           = 0x0020,

	CM_SUB_STATE_DIAG_INFO_MASK                 = 0x0040, /* Bit 6: SubmoduleState.DiagInfo */
	CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE        = 0x0000,
	CM_SUB_STATE_DIAG_INFO_AVAILABLE            = 0x0040,

	CM_SUB_STATE_AR_INFO_MASK                   = 0x0780, /* Bit 7 - 10: SubmoduleState.ARInfo */
	CM_SUB_STATE_AR_INFO_OWNED                  = 0x0000,
	CM_SUB_STATE_AR_INFO_APPL_READY_PENDING     = 0x0080,
	CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED  = 0x0100,
	CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER    = 0x0180,
	CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR    = 0x0200,

	CM_SUB_STATE_IDENT_MASK                     = 0x7800, /* Bit 11 - 14: SubmoduleState.IdentInfo */
	CM_SUB_STATE_IDENT_OK                       = 0x0000,
	CM_SUB_STATE_IDENT_SUBSTITUTE               = 0x0800,
	CM_SUB_STATE_IDENT_WRONG                    = 0x1000,
	CM_SUB_STATE_IDENT_NO_SUBMODULE             = 0x1800
};

#define CM_SUB_STATE_PULLED /* for initialization */ \
	( CM_SUB_STATE_FORMAT_1 \
	| CM_SUB_STATE_ADD_INFO_NONE \
	| CM_SUB_STATE_ADVICE_NOT_AVAILABLE \
	| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED \
	| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED \
	| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_AR_INFO_OWNED \
	| CM_SUB_STATE_IDENT_NO_SUBMODULE \
	)

#define CM_SUB_STATE_GOOD /* what the client assumes if not told otherwise */ \
	( CM_SUB_STATE_FORMAT_1 \
	| CM_SUB_STATE_ADD_INFO_NONE \
	| CM_SUB_STATE_ADVICE_NOT_AVAILABLE \
	| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED \
	| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED \
	| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_AR_INFO_OWNED \
	| CM_SUB_STATE_IDENT_OK \
	)

#define CM_SUB_STATE_WRONG /* what the client assumes on receiving a plug-wrong-alarm */ \
	( CM_SUB_STATE_FORMAT_1 \
	| CM_SUB_STATE_ADD_INFO_NONE \
	| CM_SUB_STATE_ADVICE_NOT_AVAILABLE \
	| CM_SUB_STATE_MAINTENANCE_NOT_REQUIRED \
	| CM_SUB_STATE_MAINTENANCE_NOT_DEMANDED \
	| CM_SUB_STATE_DIAG_INFO_NOT_AVAILABLE \
	| CM_SUB_STATE_AR_INFO_OWNED \
	| CM_SUB_STATE_IDENT_WRONG \
	)

#define CM_SUB_STATE_IS_OK(ss)            (((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_OK)
#define CM_SUB_STATE_IS_SUBSTITUTE(ss)    (((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_SUBSTITUTE)
#define CM_SUB_STATE_IS_WRONG(ss)         (((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_WRONG)
#define CM_SUB_STATE_IS_NO_SUBMODULE(ss)  (((ss) & CM_SUB_STATE_IDENT_MASK) == CM_SUB_STATE_IDENT_NO_SUBMODULE)

#define CM_SUB_STATE_IS_OWNED(ss)         (((ss) & CM_SUB_STATE_AR_INFO_MASK) <= CM_SUB_STATE_AR_INFO_APPL_READY_PENDING)

#define CM_SUB_STATE_GET_AR_INFO(ss)            ((ss) & CM_SUB_STATE_AR_INFO_MASK)
#define CM_SUB_STATE_SET_AR_INFO(ss, ar_info)   { ss = (((ss) & ~CM_SUB_STATE_AR_INFO_MASK) | ((ar_info) & CM_SUB_STATE_AR_INFO_MASK)); }

#define CM_SUB_STATE_SET_IDENT(ss, ident)       { ss = (((ss) & ~CM_SUB_STATE_IDENT_MASK) | ((ident) & CM_SUB_STATE_IDENT_MASK)); }

#define CM_SUB_STATE_SET_DIAG_INFO(ss, diag_info) {\
	ss = (((ss) & ~CM_SUB_STATE_DIAG_INFO_MASK) | ((diag_info) & CM_SUB_STATE_DIAG_INFO_MASK));\
	}


/**************************************************************************//**
@addtogroup CM_EVENT_TYPE
@{
# Reason codes for AR Addr Info, AR Abort and AR Offline # {#cm-ar-reasons}

These codes are used by the CM client and the CM server. See PNIO-Spec, chapter "Values of ErrorCode1 and ErrorCode2 for ErrorDecode with the value PNIO"

ErrorCode | ErrorDecode | ErrorCode1 | ErrorCode2
----------|-------------|------------|---------------
0xCF      | 0x81        | 0xFD       | CM_AR_REASON_*

@see @ref cm_ar_reason_enum

## Used for the following opcodes

 - INDICATION/RESPONSE: @ref CM_OPC_CL_AR_OFFLINE_IND_RSP
 - INDICATION/RESPONSE: @ref CM_OPC_SV_AR_DISCONNECT_IND_RSP
******************************************************************************/
enum cm_ar_reason_enum {

	CM_AR_REASON_NONE       =   0, /**< 0x00: no error */
	/***/
	CM_AR_REASON_1          =   1, /**< 0x01: sequence numbers do not match (no longer used in versions >= V3.9)*/
	CM_AR_REASON_2          =   2, /**< 0x02: alarm instance closed (no longer used in versions >= V3.6) */
	CM_AR_REASON_MEM        =   3, /**< 0x03: out of mem */
	CM_AR_REASON_FRAME      =   4, /**< 0x04: add provider or consumer failed */
	CM_AR_REASON_MISS       =   5, /**< 0x05: miss (consumer) */
	CM_AR_REASON_TIMER      =   6, /**< 0x06: cmi timeout */
	CM_AR_REASON_ALARM      =   7, /**< 0x07: alarm-open failed */
	CM_AR_REASON_ALSND      =   8, /**< 0x08: alarm-send.cnf(-) */
	CM_AR_REASON_ALACK      =   9, /**< 0x09: alarm-ack-send.cnf(-) */
	CM_AR_REASON_ALLEN      =  10, /**< 0x0A: alarm-data too long */
	CM_AR_REASON_ASRT       =  11, /**< 0x0B: alarm.ind(err) */
	CM_AR_REASON_RPC        =  12, /**< 0x0C: rpc-client call.cnf(-) */
	CM_AR_REASON_ABORT      =  13, /**< 0x0D: ar-abort.req */
	CM_AR_REASON_RERUN      =  14, /**< 0x0E: rerun aborts existing */
	CM_AR_REASON_REL        =  15, /**< 0x0F: got release.ind */
	CM_AR_REASON_PAS        =  16, /**< 0x10: device passivated */
	CM_AR_REASON_RMV        =  17, /**< 0x11: device / AR removed */
	CM_AR_REASON_PROT       =  18, /**< 0x12: protocol violation */
	CM_AR_REASON_NARE       =  19, /**< 0x13: NARE error */
	CM_AR_REASON_BIND       =  20, /**< 0x14: RPC-Bind error */
	CM_AR_REASON_CONNECT    =  21, /**< 0x15: RPC-Connect error */
	CM_AR_REASON_READ       =  22, /**< 0x16: RPC-Read error */
	CM_AR_REASON_WRITE      =  23, /**< 0x17: RPC-Write error */
	CM_AR_REASON_CONTROL    =  24, /**< 0x18: RPC-Control error */
	CM_AR_REASON_25         =  25, /**< 0x19: reserved (formerly: pull or plug in forbidden window) */
	CM_AR_REASON_26         =  26, /**< 0x1A: reserved (formerly: AP removed) */
	CM_AR_REASON_LNK_DOWN   =  27, /**< 0x1B: link "down", for local purpose only */
	CM_AR_REASON_28         =  28, /**< 0x1C: reserved (formerly: could not register multicast-mac) */
	CM_AR_REASON_SYNC       =  29, /**< 0x1D: not synchronized (cannot start companion-ar) */
	CM_AR_REASON_30         =  30, /**< 0x1E: reserved (formerly: wrong topology (cannot start companion-ar)) */
	CM_AR_REASON_DCP_NAME   =  31, /**< 0x1F: dcp, station-name changed */
	CM_AR_REASON_DCP_RESET  =  32, /**< 0x20: dcp, reset to factory or factory reset */
	CM_AR_REASON_33         =  33, /**< 0x21: reserved (formerly: cannot start companion-ar) */
	CM_AR_REASON_IRDATA     =  34, /**< 0x22: no irdata record yet */
	CM_AR_REASON_PDEV       =  35, /**< 0x23: ownership of physical device */
	CM_AR_REASON_LNK_MODE   =  36, /**< 0x24: link mode not full duplex */
	CM_AR_REASON_IPSUITE    =  37, /**< 0x25: IP-Suite [of the IOC] changed by means of DCP_set(IPParameter) or local engineering */
	CM_AR_REASON_RDHT       =  38, /**< 0x26: IOCARSR RDHT expired */
	CM_AR_REASON_PDEV_PRM   =  39, /**< 0x27: IOCARSR PDev, parametrization impossible (AP01238541) */
	CM_AR_REASON_ARDY       =  40, /**< 0x28: Remote application timeout expired */
	CM_AR_REASON_41_unused  =  41, /**< 0x29: IOCARSR Redundant interface lost or access to the peripherals impossible */
	CM_AR_REASON_MTOT       =  42, /**< 0x2A: IOCARSR MTOT expired */
	CM_AR_REASON_43_unused  =  43, /**< 0x2B: IOCARSR AR protocol violation */
	CM_AR_REASON_COC        =  44, /**< 0x2C: Pdev, plug port without CombinedObjectContainer */

	/** @cond INTERNAL */

	CM_AR_REASON_R1_BRIDGE  = 252, /**< internally used only, not visible in LogBookData */
	CM_AR_REASON_R1_CONN    = 253, /**< internally used only, not visible in LogBookData */
	CM_AR_REASON_R1_DISC    = 254, /**< internally used only, not visible in LogBookData */

	/** @endcond */

	CM_AR_REASON_MAX        = 255 /**< !*/
};
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM Records                                                                */
/**************************************************************************//**
@defgroup cm-records CM Records
@ingroup cm-rqbs-group
@{
@brief Common defines for @ref CM_CL_RECORD_TYPE and @ref CM_SV_RECORD_TYPE.
CM_SCL_RECORD_TYPE and CM_SV_RECORD_TYPE are part of @ref CM_EVENT_TYPE and are used for the record services (requests and indications).
******************************************************************************/
#define CM_RECORD_HEADER_SIZE  (64U)
#define CM_RECORD_OFFSET       (CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE)

#define CM_RECORD_READ_NOT_HANDLED(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_READ, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xA0, /* application, read error */ \
		(code2) \
		))

#define CM_RECORD_READ_NO_IODATA(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_READ, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xB0, /* invalid index, see AP00383155 */ \
		(code2) \
		))

#define CM_RECORD_WRITE_NOT_HANDLED(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_WRITE, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xA1, /* application, write error */ \
		(code2) \
		))

#define CM_RECORD_WRITE_BACKUP(code2) (CM_PNIO_ERR_MAKE( \
		CM_PNIO_ERR_CODE_WRITE, \
		CM_PNIO_ERR_DECODE_PNIORW, \
		0xBA, /* access, backup (usecase: system redundancy) */ \
		(code2) \
		))
/** @} ***********************************************************************/


/*****************************************************************************/
/* 214 CM_OPC_PD_PROVIDE_EVENT                                               */
/**************************************************************************//**
@defgroup CM_OPC_PD_PROVIDE_EVENT 214 CM_OPC_PD_PROVIDE_EVENT
@ingroup cm-pd-opcodes
@brief The EventProvide function is used to provide one event indication resource.
@details
Indications change the opcode of the RQB as follows:

Refer to @ref CM_OPC_PD_EVENT_APPL_READY_IND.

Each indication must be acknowledged with the corresponding response CM_OPC_xxx_RSP.

@see @ref CM_EVENT_TYPE

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK_CANCELLED        | The RQB is to be released. Refer to @ref CM_OPC_PD_DELETE.
CM_ERR_SEQUENCE        | Request has not been made between CM_OPC_PD_CREATE and CM_OPC_PD_DELETE.
******************************************************************************/


/*****************************************************************************/
/* 215/216 CM_OPC_PD_EVENT_APPL_READY_IND/_RSP                               */
/**************************************************************************//**
@defgroup CM_OPC_PD_EVENT_APPL_READY_IND_RSP 215/216 CM_OPC_PD_EVENT_APPL_READY_IND/_RSP
@ingroup cm-pd-opcodes
@{
@brief The end of the parameterization is signaled with the PrmApplReady.IND indication.
@details
The opcode requires a response (memory release, refer to chapter @ref cm-product-architecture-programming-model-general-request-block-circulations-indications).

If there is currently not resource for PrmApplReady.ind, (assuming CMPD is waiting for PrmApplReady.rsp), the indication is delayed, until the response is returned to CMPD.

@deprecated Each PD_PrmEnd.CNF increases the value of "session_key" by 1. \n
Each PD_PrmEnd.CNF sets the value of "alarm_sequence" to 0.

The "diff_list" list is allocated by CM (UpperMem).

This service also releases the ownership of the PDEV.

On PrmApplReady.IND

Set the diagnosis state of all PDEV submodules to "no diagnosis".

@note PrmApplReady.IND is signaled too if the PDEV is parameterized remotely via an AR. See @ref CM_OPC_SV_DEVICE_CONTROL.

If rqb.response == CM_OK:

This means that the **"given parameterization"** was executed. Starting with V6.0i1.50, if the parameterization was faulty, an **"empty parameterization"** is enabled within the PDEV and a diagnose is recorded for every fault in the parameterization reported by a subcomponent, which also obsoletes the parameter .faulty_record_index.


The "diff_list" contains a hierarchical list of PDEV submodules with the current diagnosis state. The field "sub_state" has to be evaluated:

CM_SUB_STATE_AR_INFO_MASK
== CM_SUB_STATE_AR_INFO_OWNED

CM_SUB_STATE_ADVICE_MASK
CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK
CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK
CM_SUB_STATE_DIAG_INFO_MASK
Update the diagnosis state appropriately

CM_SUB_STATE_IDENT_MASK
== CM_SUB_STATE_IDENT_OK
== CM_SUB_STATE_IDENT_NO_SUBMODULE


CM_SUB_STATE_xxx_MASK
Don't evaluate (bits are all 0).

@deprecated ## If rqb.response == CM_ERR_PRM_CONSISTENCY
This means that the parameterization had a fault and was not accepted. The **"empty parameterization"** was written to the subcomponents. \n
The "diff_list" contains a hierarchical list of PDEV submodules with the current (i.e., prior) diagnosis state. The field "sub_state" has to be evaluated: \n
CM_SUB_STATE_AR_INFO_MASK \n
== CM_SUB_STATE_AR_INFO_APPL_READY_PENDING \n
CM_SUB_STATE_ADVICE_MASK \n
CM_SUB_STATE_MAINTENANCE_REQUIRED_MASK \n
CM_SUB_STATE_MAINTENANCE_DEMANDED_MASK \n
CM_SUB_STATE_DIAG_INFO_MASK \n
Update the diagnosis state appropriately \n
CM_SUB_STATE_xxx_MASK \n
Don't evaluate (bits are all 0). \n
The PDev has the internal state "NotReady", therefore the start-up of IO devices is delayed. \n

## If rqb.response == CM_OK_ADAPTION_NEEDED:

This means some missing tailoring information. The **"empty parameterization"** was written to the subcomponents.

After the user has tailored the parameterization, this tailoring triggers a re-parameterization of the PDev. The end of this parameterization is indicated again with the PrmApplReady.ind.

Evaluate the "diff_list" as like response CM_OK.
The PD-Alarm.ind circle is enabled.
The PDev has the internal state "NotReady", therefore the start-up of IO devices is delayed.

## If rqb.response == CM_OK_RESET_TO_FACTORY:

This means that the trigger ResetToFactory has written the **"default parameterization"** to the subcomponents.

Evaluate the "diff_list" as like response CM_OK.
The PD-Alarm.ind cycle is disabled.
The PDev has the internal state "NotReady", therefore the start-up of IO devices is delayed.

The only possibility to leave this state is a new PrmSequence of the PDev.

@note rqb.args.channel.event.device_nr, rqb.args.channel.event.ar_nr and rqb.args.channel.event.session_key (-> For compatibility the constant 0 is supplied.) are not evaluated!

@see CM_EVENT_TYPE::u.pd

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | The parameterization was executed. If the parameterization has been faulty, an empty parameterization is enabled within the PDEV and an extended diagnosis is recorded for every fault in the parameterization.
CM_OK_ADAPTION_NEEDED  | The given parameterization is part of a MasterProject and further adaption is needed. The empty parameterization is enabled within PDev. The startup of the IO devices is prevented, until the needed adaption is done.
CM_OK_RESET_TO_FACTORY | The default parameterization is enabled within PDev because of an extern ResetToFactory request. Only DeviceRemove is allowed as further action. Then a new given parameterization must be written to the PDev.

@deprecated CM_ERR_PRM_CONSISTENCY: The given parameterization was faulty and not executed.
The empty parameterization is enabled within PDev.
The startup of the IO devices is not allowed, until a new parameterization was written to the PDev.
******************************************************************************/

/** Type used for @ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP, @ref CM_OPC_CL_AR_PRE_DATA_IND_RSP and @ref CM_OPC_CL_AR_PRM_INDATA_IND_RSP */
typedef struct cm_pd_diff_submodule_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag) link;
	/**< link to next entry */

	LSA_UINT16 subslot_nr;
	/**< ## IND
	 * subslot_nr
	 */

	LSA_UINT32 sub_ident;
	/**< ## IND
	 * real submodule-ident number
	 */

	LSA_UINT16 sub_state;
	/**< ## IND
	 * see cm_sub_state_enum
	 */

} CM_PD_DIFF_SUBMODULE_TYPE;

typedef CM_PD_DIFF_SUBMODULE_TYPE * CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE;

/** Type used for @ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP, @ref CM_OPC_CL_AR_PRE_DATA_IND_RSP and @ref CM_OPC_CL_AR_PRM_INDATA_IND_RSP */
typedef struct cm_pd_diff_module_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag) link;
	/**< link to next entry */

	LSA_UINT16 slot_nr;
	/**< ## IND
	 * slot_nr
	 */

	LSA_UINT32 mod_ident;
	/**< ## IND
	 * real module-ident number
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_submodule_tag) submodule_list;
	/**< list of type: CM_PD_DIFF_SUBMODULE_TYPE */

} CM_PD_DIFF_MODULE_TYPE;

typedef CM_PD_DIFF_MODULE_TYPE * CM_UPPER_PD_DIFF_MODULE_PTR_TYPE;

/** Type used for @ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP, @ref CM_OPC_CL_AR_PRE_DATA_IND_RSP and @ref CM_OPC_CL_AR_PRM_INDATA_IND_RSP */
typedef struct cm_pd_diff_ap_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) link;
	/**< link to next entry */

	LSA_UINT32 api;
	/**< ## IND
	 * application process identifier
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_module_tag) module_list;
	/**< list of type: CM_PD_DIFF_MODULE_TYPE */

} CM_PD_DIFF_AP_TYPE;

typedef CM_PD_DIFF_AP_TYPE * CM_UPPER_PD_DIFF_AP_PTR_TYPE;

typedef struct cm_pd_appl_ready_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) diff_list;
	/**< ## IND
	 * Hierarchical list with submodules that do not correspond to the expected configuration.
	 * list of type: CM_PD_DIFF_AP_TYPE
	 */

	LSA_UINT16 mod_diff_block_length;
	/**< ## IND
	 * Length of "mod_diff_block"
	 * AP00679788
	 */

	CM_UPPER_MEM_PTR_TYPE mod_diff_block;
	/**< ## IND
	 * Same information as "diff_list" but coded as normative "ModuleDiffBlock".
	 */


	LSA_UINT16 diag_block_length;
	/**< ## IND
	 * Length of "diag_block"
	 */

	CM_UPPER_MEM_PTR_TYPE diag_block;
	/**< ## IND
	* The initial diagnosis information (V5.2i4.11, AP01427283).
	* Coded as normative "RecordDataRead, Index 0xF80C" (i.e. "DiagnosisData with BlockVersionLow=1" for each port that has an initial diagnosis, see [/11/](@ref cm-ref-no-11)).
	*/

	CM_RESERVED_INTERVAL_IF_RED_TYPE rsv_interval_red;
	/**< ## IND
	 * Timing parameters for the IO update (V5.2i4.29, AP00964333 and AP01317720).
	 * For the meaning of the values, please refer to the EDD documentation / EDD_RESERVED_INTERVAL_IF_RED_TYPE.
	 * @note The timing can be optimized by inserting the record PDIRApplicationData (index 0x00010002, see EDD documentation) into the PDEV parameterization of the interface submodule.
	 * see EDD documentation, EDD_RESERVED_INTERVAL_IF_RED_TYPE
	*/

} CM_PD_APPL_READY_TYPE;

typedef CM_PD_APPL_READY_TYPE * CM_UPPER_PD_APPL_READY_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_APPL_READY_IND/CM_OPC_PD_APPL_READY_RSP */
/** @} ***********************************************************************/


/******************************************************************************
begin of CM-CL events
******************************************************************************/
#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*****************************************************************************/
/* 041/042 CM_OPC_CL_AR_ABORT_IND/_RSP                                       */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_ABORT_IND_RSP 041/042 CM_OPC_CL_AR_ABORT_IND/_RSP
@ingroup cm-cl-opcodes
@{
@brief The Abort.IND indication signals the failure of an AR. The session ends with this indication.
@details
@see @ref CM_EVENT_TYPE

A new AR creation trial is started only after the arrival of the Abort.RSP.

## IP-Address
The given IP-Address depends on:
- CM_NARE_MODE_xxx given by DeviceAdd()
- PDMasterTailorData is given to device and AddressTailoring is set

CM_NARE_MODE_xxx          | Normal usecase             | With AddressTailoring
--------------------------|----------------------------|-------------------------------
_CONFIG_IP_FIXED          | DevAdd.device_ip           | Tailored IP-Address. 0 if the IP-Address of the IO Controller is not set
_RESOLVE_DCP              | 0                          | 0

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/
#define CM_ERROR_CAUSE_NONE					0	/**< No problem, everything OK */
#define CM_ERROR_CAUSE_IP_MULTIPLE			1	/**< IP address exists multiple times */
#define CM_ERROR_CAUSE_NOS_MULTIPLE			2	/**< NameOfStation exists multiple times */
#define CM_ERROR_CAUSE_LINK_DOWN			3	/**< Link down (no MAC link, no IP suite) */
#define CM_ERROR_CAUSE_MAU_MISMATCH			4	/**< MAUType mismatch */
#define CM_ERROR_CAUSE_RPC_ERROR			5	/**< RPC protocol error */
#define CM_ERROR_CAUSE_PNIO_CONFIGURATION	6	/**< PNIO configuration protocol error */
#define CM_ERROR_CAUSE_RPC_TIMEOUT			7	/**< RPC application timeout */
#define CM_ERROR_CAUSE_IP_SET_ABORTED		8	/**< IP address assignment to IO device aborted */
#define CM_ERROR_CAUSE_IOD_NOT_FOUND		9	/**< IO device not found */
#define CM_ERROR_CAUSE_OUT_OF_AR_RESOURCES	10	/**< Out of AR resources */
#define CM_ERROR_CAUSE_OUT_OF_RESOURCES			/* OUTDATED: do not use define */ CM_ERROR_CAUSE_OUT_OF_AR_RESOURCES
#define CM_ERROR_CAUSE_MISS					11	/**< WDT / DHT expired */
#define CM_ERROR_CAUSE_NOS_SET_ABORTED		12	/**< Device with wrong NameOfStation found on topological position */
#define CM_ERROR_CAUSE_IOC_ABORTED			13	/**< Controller local abort reason */
#define CM_ERROR_CAUSE_IOD_ABORTED			14	/**< Device abort reason */
#define CM_ERROR_CAUSE_SRD_ABORTED			15	/**< SystemRedundancy abort reason */
#define CM_ERROR_CAUSE_VENDORID_ABORTED		16	/**< Device with wrong VendorId/DeviceId/Instance found on topological position */
#define CM_ERROR_CAUSE_MP_IN_PROGRESS		17	/**< MasterProject: Machine- or AddressTailoring in progress */
#define CM_ERROR_CAUSE_MP_NOS_MISSING		18	/**< MasterProject: NameOfStation adaption missing (PDev with invalid NoS) */
#define CM_ERROR_CAUSE_MP_IPv4_MISSING		19	/**< MasterProject: IPv4Suite adaption missing (PDev with invalid IPv4Suite) */
#define CM_ERROR_CAUSE_MP_NOS_INVALID		20	/**< MasterProject: tailored NameOfStation|PTCPSubdomainName|MRPDomainName is invalid */
#define CM_ERROR_CAUSE_MP_IPv4_INVALID		21	/**< MasterProject: tailored IPv4Suite of IOD is invalid */
#define CM_ERROR_CAUSE_MP_TAILORING_MISSING	22	/**< MasterProject: MachineTailoring missing */
#define CM_ERROR_CAUSE_MP_PDEV_ERROR		23	/**< MasterProject: PDev configuration error */
#define CM_ERROR_CAUSE_MP_DEVICE_DISABLED	24	/**< MasterProject: disabled Device is activated */
#define CM_ERROR_CAUSE_MP_CONFIG_INCOMPLETE	25	/**< MasterProject: configuration incomplete (e.g. neighbour does not exist because of device-remove) */
#define CM_ERROR_CAUSE_OUT_OF_CR_RESOURCES	26	/**< Connect: Out of CR resources */
#define CM_ERROR_CAUSE_PDEV_OWNERSHIP		27	/**< Connect: Conflicting parameter assignment to the PN interface of the device */
#define CM_ERROR_CAUSE_SYSTEM_REDUNDANCY	28	/**< Connect: Conflicting states or parameters in a system redundancy configuration */

typedef struct cm_cl_error_cause_tag {

	LSA_UINT8 reason_code;
	/**< ## IND
	 * Reason code (equivalent to ErrorCode2 from /IEC/) \n
	 * see @ref cm_ar_reason_enum, see ERR-RTA-PDU, see ErrorCode2 for ErrorCode1 == 253
	 */

	LSA_UINT8 padding;
	/**< ## IND
	 * do not use: only for alignment reasons
	 */

	LSA_UINT16 error_cause;
	/**< ## IND
	 * Big endian, contains information from AR abort reason, see CM_ERROR_CAUSE_xxx \n
	 * The concerned values are described in [/18/](@ref cm-ref-no-18).
	 */

	LSA_UINT8 additional_info[10];
	/**< ## IND
	 * Big endian, contains additional information, see [/18/](@ref cm-ref-no-18)
	 */

} CM_CL_ERROR_CAUSE_TYPE;

typedef CM_CL_ERROR_CAUSE_TYPE * CM_UPPER_CL_ERROR_CAUSE_PTR_TYPE; /**< The pointer type used for CM_CL_ERROR_CAUSE_TYPE */

typedef struct cm_cl_ar_abort_tag {

	CM_CL_ERROR_CAUSE_TYPE error_cause; /**< refer @ref CM_CL_ERROR_CAUSE_TYPE */
	LSA_UINT32 device_ip;
	/**< ## IND
	 * IP address of the device, may 0 if address must resolved.
	 */

} CM_CL_AR_ABORT_TYPE;

typedef CM_CL_AR_ABORT_TYPE * CM_UPPER_CL_AR_ABORT_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* 043/044 CM_OPC_CL_AR_PRE_DATA_IND/_RSP                                    */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_PRE_DATA_IND_RSP 043/044 CM_OPC_CL_AR_PRE_DATA_IND/_RSP
@ingroup cm-cl-opcodes
@{
@brief The intended purpose of this indication is to signal the end-of-parameterization to the user.
@details
@see @ref CM_EVENT_TYPE

The structure of an AR (ModuleDiffBlock) is signaled with the help of the diff_list. The same information is contained in normative format in the field mod_diff_block.

With the response of this indication, the alarm delivery from CM will be enabled.
Furthermore the user is able to read and write records.

"diff_list" is a hierarchical list containing submodules that do not correspond to the reference configuration.

"mod_diff_block" contains the same submodules as the diff_list, but formatted as ModuleDiffBlock.

With "ip_suite" the ip-suite and the mac-address of the related device are signaled to the user.

@note the buffer of the consumer is not valid, when this indication appears.

Usage of "diff_list" or "mod_diff_block":

The field 'mod_state' was removed with CM V4.2i2.1 (AP00841926). The field 'sub_state' contains all information about a submodule.

The field 'mod_diff_block' is not a copy of the bytes on the wire. It is regenerated by CM, because of some legacy issues and AP00841926.
The ModuleState CM_MOD_STATE_NO_MODULE is a shortcut for "all submodules of this module are pulled" (i.e., SubmoduleState == CM_SUB_STATE_PULLED).
The ModuleState CM_MOD_STATE_WRONG_MODULE will never be delivered due to AP00841926.

This indication uses one of the event resources provided by AddDevice.
The appended list and block is allocated by CM (UpperMem).

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations



## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/
typedef CM_UPPER_PD_DIFF_SUBMODULE_PTR_TYPE  CM_UPPER_CL_DIFF_SUBMODULE_PTR_TYPE;

typedef CM_UPPER_PD_DIFF_MODULE_PTR_TYPE  CM_UPPER_CL_DIFF_MODULE_PTR_TYPE;

typedef CM_UPPER_PD_DIFF_AP_PTR_TYPE  CM_UPPER_CL_DIFF_AP_PTR_TYPE;

typedef struct cm_cl_ip_suite_tag {

	CM_MAC_TYPE device_mac;
	/**< ## CM_OPC_CL_AR_PRE_DATA_IND / CM_OPC_CL_AR_IN_DATA_IND
	 * Ethernet mac-addr of device or error-mac
	 */

	LSA_UINT32 device_ip;
	/**< ## CM_OPC_CL_AR_PRE_DATA_IND / CM_OPC_CL_AR_IN_DATA_IND
	 * ip-addr of device or error-ip
	 */

	LSA_UINT32 device_subnet_mask;
	/**< ## CM_OPC_CL_AR_PRE_DATA_IND / CM_OPC_CL_AR_IN_DATA_IND
	 * Subnet-mask of device
	 */

	LSA_UINT32 device_gateway_ip;
	/**< ## CM_OPC_CL_AR_PRE_DATA_IND / CM_OPC_CL_AR_IN_DATA_IND
	 * The IP address of the router that is responsible for forwarding the IP telegrams if the controller cannot be reached in the local subnetwork of the device.
	 */

} CM_CL_IP_SUITE_TYPE;

typedef CM_CL_IP_SUITE_TYPE * CM_UPPER_CL_IP_SUITE_PTR_TYPE; /**< The pointer type used for CM_CL_IP_SUITE_TYPE */


typedef struct cm_cl_ar_in_data_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) diff_list;
	/**< ## CM_OPC_CL_AR_PRE_DATA_IND / CM_OPC_CL_AR_IN_DATA_IND
	 * List of type: CM_PD_DIFF_AP_TYPE \n
	 * Hierarchical list of all submodules that do not correspond to the reference configuration.
	 */

	LSA_UINT16 mod_diff_block_length;
	/**< AP00679788
	 * ## CM_OPC_CL_AR_PRE_DATA_IND / CM_OPC_CL_AR_IN_DATA_IND
	 * Length of the block mod_diff_block
	 */

	CM_UPPER_MEM_PTR_TYPE mod_diff_block;
	/**< ## CM_OPC_CL_AR_PRE_DATA_IND / CM_OPC_CL_AR_IN_DATA_IND
	 * Block with all submodules that do not correspond to the reference configuration. \n
	 * The structure of the bock is defined in [/11/](@ref cm-ref-no-11) "ModuleDiffBlock"
	 */

	CM_CL_IP_SUITE_TYPE ip_suite;
	/**< values retrieved from name resolution, refer @ref CM_CL_IP_SUITE_TYPE */

} CM_CL_AR_IN_DATA_TYPE;

typedef CM_CL_AR_IN_DATA_TYPE * CM_UPPER_CL_AR_IN_DATA_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_AR_PRE_DATA_IND/_RSP */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 045/046 CM_OPC_CL_AR_PRE_OFFLINE_IND/_RSP                                 */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_PRE_OFFLINE_IND_RSP 045/046 CM_OPC_CL_AR_PRE_OFFLINE_IND/_RSP
@ingroup cm-cl-opcodes
@brief The PreOffline.IND indication signals the failure of an AR.
@details
@see @ref CM_EVENT_TYPE

The session ends with this indication.\n
A new AR creation trial is started only after the arrival of the PreOffline.RSP.

## Diagnosis
The field ar_abort.error_cause contains information about the abort reason of the AR. \n
The concerned values are described in [/18/](@ref cm-ref-no-18).

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

For the parameters of this opcode, please refer to @ref CM_OPC_CL_AR_ABORT_IND_RSP.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/


/*****************************************************************************/
/* 047/048 CM_OPC_CL_AR_IN_DATA_IND/_RSP                                     */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_IN_DATA_IND_RSP 047/048 CM_OPC_CL_AR_IN_DATA_IND/_RSP
@ingroup cm-cl-opcodes
@brief This indication signals that this AR has successful achieved the state 'IN_DATA'. This means, the buffers of the device are valid.
@details
@see @ref CM_EVENT_TYPE

The different fields have the same meaning as in chapter @ref CM_OPC_CL_AR_PRE_DATA_IND.

This indication uses one of the event resources provided by AddDevice. \n
The appended lists are allocated by CM (UpperMem).

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

For the parameters of this opcode, please refer to @ref CM_CL_AR_IN_DATA_TYPE;

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/


/*****************************************************************************/
/* 049/050 CM_OPC_CL_AR_OFFLINE_IND/_RSP                                     */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_OFFLINE_IND_RSP 049/050 CM_OPC_CL_AR_OFFLINE_IND/_RSP
@ingroup cm-cl-opcodes
@brief The AR_Offline.IND indication signals the failure of an AR.
@details
@see @ref CM_EVENT_TYPE
@param For the parameters of this opcode, please refer to @ref CM_OPC_CL_AR_ABORT_IND_RSP.

The session ends with this indication. \n
A new AR establishment is started only with the arrival of the AR_Offline.RSP.

## Session key
AddDevice is used to initialize the session key in CM with 0 (the user must start with this value too).
This is also the start of the first session. The current session ends with AR_Offline.IND.
The session key of the next session is contained in the "session_key" field.

## Diagnosis
The field ar_abort.error_cause contains information about the abort reason of the AR. \n
The concerned values are described in [/18/](@ref cm-ref-no-18).

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | Success.
******************************************************************************/


/*****************************************************************************/
/* 054/055 CM_OPC_CL_AR_PRM_BEGIN_IND/_RSP                                   */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_PRM_BEGIN_IND_RSP 054/055 CM_OPC_CL_AR_PRM_BEGIN_IND/_RSP
@ingroup cm-cl-opcodes
@{
@brief The PrmBegin.IND signals the beginning of the prm-sequence to the user.
@details
@see @ref CM_EVENT_TYPE

With prm_mode = AR the prm-sequence of an AR is indicated.

With prm_mode = SUBMODULE the prm-sequence of a submodule is indicated.
The fields api / slot_nr / subslot_nr address the concerned submodule.

With prm_mode = COC the prm-sequence of a COC group is indicated.
The fields api / slot_nr / subslot_nr addresses the access point of the COC group.

The opcode requires a response.
With the response the associated RPC-PrmBegin will be started.

For the dynamical behavior see figures in @ref CM_OPC_CL_PRM_UPDATE.

## Result of the indication

Return Code        | Meaning
-------------------|---------
CM_OK              | Success
******************************************************************************/
typedef struct cm_cl_prm_begin_tag
{
	LSA_UINT16 prm_mode;
	/**< ## IND
	 * Mirror the prm_mode from the corresponding PrmUpdate, see @ref CM_OPC_CL_PRM_UPDATE.
	 */

	/* api/slot_nr/subslot_nr valid if prm_mode is CM_CL_PRMMODE_SUBMODULE or CM_CL_PRMMODE_COC */
	LSA_UINT32 api;
	/**< ## IND
	 * Valid with prm_mode = CM_CL_PRMMODE_SUBMODULE or prm_mode = CM_CL_PRMMODE_COC. \n
	 * Indicate the API of the  target submodule or COC access point.
	 */

	LSA_UINT16 slot_nr;
	/**< ## IND
	 * Valid with prm_mode = CM_CL_PRMMODE_SUBMODULE or prm_mode = CM_CL_PRMMODE_COC. \n
	 * Indicate the slot number of the concerned submodule or COC access point.
	 */

	LSA_UINT16 subslot_nr;
	/**< ## IND
	 * Valid with prm_mode = CM_CL_PRMMODE_SUBMODULE or prm_mode = CM_CL_PRMMODE_COC. \n
	 * Indicate the subslot number of the concerned submodule or COC access point.
	 */
} CM_CL_PRM_BEGIN_TYPE;

typedef CM_CL_PRM_BEGIN_TYPE * CM_UPPER_CL_PRM_BEGIN_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_AR_PRM_BEGIN_IND/_RSP */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 056/057 CM_OPC_CL_AR_PRM_INDATA_IND/_RSP                                  */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_PRM_INDATA_IND_RSP 056/057 CM_OPC_CL_AR_PRM_INDATA_IND/_RSP
@ingroup cm-cl-opcodes
@{
@brief The PrmInData.IND signals the end of the prm-sequence to the user.
@details
@see @ref CM_EVENT_TYPE

With prm_mode = AR the prm-sequence of an AR is indicated. The "diff_list" targets all submodule of the AR.

With prm_mode = SUBMODULE the prm-sequence of a submodule is indicated.
The fields api / slot_nr / subslot_nr address the concerned submodule.
The "diff_list" targets only the given submodule.

With prm_mode = COC the prm-sequence of of a COC group is indicated.
The fields api / slot_nr / subslot_nr address the access point of the COC group.
The "diff_list" targets only the access point of the COC group.

"diff_list" is a hierarchical list containing submodules that are not in the submodule state GOOD.

"mod_diff_block" represents the "diff_list" formatted as ModuleDiffBlock.

See also section "Usage of "diff_list" or "mod_diff_block"" in chapter @ref CM_OPC_CL_AR_PRE_DATA_IND.

The opcode requires a response.

For the dynamical behaviour see figures in @ref CM_OPC_CL_PRM_UPDATE.

## Result of the indication

Return Code        | Meaning
-------------------|---------
CM_OK              | Success.
******************************************************************************/
typedef struct cm_cl_prm_indata_tag
{
	LSA_UINT16 prm_mode;
	/**< ## IND
	 * Mirror the prm_mode from the corresponding PrmUpdate.req, see @ref CM_OPC_CL_PRM_UPDATE
	 */

	LSA_UINT32 api;
	/**< ## IND
	 * Valid with prm_mode = CM_CL_PRMMODE_SUBMODULE or prm_mode = CM_CL_PRMMODE_COC \n
	 * Indicate the API of the  target submodule or COC access point.
	 */

	LSA_UINT16 slot_nr;
	/**< ## IND
	 * Valid with prm_mode = CM_CL_PRMMODE_SUBMODULE or prm_mode = CM_CL_PRMMODE_COC \n
	 * Indicate the slot number of the concerned submodule or COC access point.
	 */

	LSA_UINT16 subslot_nr;
	/**< ## IND
	 * Valid with prm_mode = CM_CL_PRMMODE_SUBMODULE or prm_mode = CM_CL_PRMMODE_COC \n
	 * Indicate the subslot number of the concerned submodule or COC access point.
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_pd_diff_ap_tag) diff_list;
	/**< ## IND
	 * Hierarchical list of all submodules that do not correspond to the reference configuration. \n
	 * The list is limited to submodules, which are specified by the corresponding CM_OPC_CL_AR_PRM_BEGIN_IND.
	 */

	LSA_UINT16 mod_diff_block_length;
	/**< ## IND
	 * Length of the block mod_diff_block
	 */

	CM_UPPER_MEM_PTR_TYPE mod_diff_block;
	/**< ## IND
	 * Block with all submodules that do not correspond to the reference configuration. \n
	 * The structure of the bock is normative defined as "ModuleDiffBlock". \n
	 * The list is limited to submodules, which are specified by the corresponding @ref CM_OPC_CL_AR_PRM_BEGIN_IND.
	 */

} CM_CL_PRM_INDATA_TYPE;

typedef CM_CL_PRM_INDATA_TYPE * CM_UPPER_CL_PRM_INDATA_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_AR_PRM_INDATA_IND/_RSP */
/** @} ***********************************************************************/


/******************************************************************************
end of CM-CL events
******************************************************************************/
#endif

/******************************************************************************
begin of CM-MC events
******************************************************************************/
#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*****************************************************************************/
/* 066/067 CM_OPC_MC_EVENT_AR_ADDR_INFO_IND/_RSP                             */
/**************************************************************************//**
@defgroup CM_OPC_MC_EVENT_AR_ADDR_INFO_IND_RSP 066/067 CM_OPC_MC_EVENT_AR_ADDR_INFO_IND/_RSP
@ingroup cm-mc-opcodes
@{
@brief The AddrInfo.IND indication is used to signal information about problems in the "startup phase".
@details

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

See @ref CM_OPC_MC_DEVICE_CONTROL, activation and passivation.

## M-provider
Does not use this indication.

## M-consumer
Signalled if the device is passivated, removed, not yet RUN, or MISS. The M-consumer continues running if not passivated or removed.

## Possible reason codes

Reason code           | Explanation
----------------------|-----------------------------------------------------
CM_AR_REASON_LNK_DOWN | Is signaled if no port is in RUN (see /11/ RTC3PSM) @note The reason codes are normative and this one was chosen because there was no better choice. However, "link down" is one possible reason for "not in RUN".
CM_AR_REASON_NARE     | Is signaled if the name of the corresponding M-provider cannot be resolved
CM_AR_REASON_NONE     | Is signaled if the name resolution succeeded. The next indication will be "In Data" (if frames are received within DHT of course).
CM_AR_REASON_MISS     | Is signaled if no frames are received within DHT.
CM_AR_REASON_FRAME    | Is signaled if the M-consumer cannot be activated. The most likely reason for this is a bug in the engineering (mismatch between the PDEV-parameterization and the IOCR-data). Another reason could be a bug in the buffer allocation.
CM_AR_REASON_PAS      | Is signaled if passivated.

@see CM_EVENT_TYPE

## Result of the request

Return Code          | Meaning
---------------------|--------------------------------
CM_OK                | Success.
******************************************************************************/
typedef struct cm_mc_addr_info_tag {

	LSA_UINT16 reason_code;
	/**< ## IND
	 * see @ref cm_ar_reason_enum, see text for possible values
	 */

	CM_MAC_TYPE device_mac;
	/**< ## IND
	 * MAC address of the device. (valid if reason_code == CM_AR_REASON_NONE, otherwise: do not evaluate)
	 */

	LSA_UINT32 device_ip;
	/**< ## IND
	 * @deprecated The usage of this value is deprecated. It reflects the state at the time of the name resolution. A change of the IP-address at the M-provider side may be undetected (no interruption of the cyclic communication) and the value may be out of date!
	 */

} CM_MC_ADDR_INFO_TYPE;

typedef CM_MC_ADDR_INFO_TYPE * CM_UPPER_MC_ADDR_INFO_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_EVENT_AR_ADDR_INFO_IND/_RSP. @note m-consumer only! */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 068/069 CM_OPC_MC_EVENT_AR_IN_DATA_IND/_RSP                               */
/**************************************************************************//**
@defgroup CM_OPC_MC_EVENT_AR_IN_DATA_IND_RSP 068/069 CM_OPC_MC_EVENT_AR_IN_DATA_IND/_RSP
@ingroup cm-mc-opcodes
@{
@brief The AR_InData.IND indication is used to signal the transition from the "startup phase" to the "data phase".
@details
The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

See @ref CM_OPC_MC_DEVICE_CONTROL, activation.

@see CM_EVENT_TYPE

## Result of the request

Return Code          | Meaning
---------------------|--------------------------------
CM_OK                | Success.
******************************************************************************/
typedef struct cm_mc_in_data_tag {

	LSA_UINT32 nothing; /**< don't care - don't evaluate */

} CM_MC_IN_DATA_TYPE;

typedef CM_MC_IN_DATA_TYPE * CM_UPPER_MC_IN_DATA_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_EVENT_AR_IN_DATA_IND/_RSP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 070/071 CM_OPC_MC_EVENT_AR_OFFLINE_IND/_RSP                               */
/**************************************************************************//**
@defgroup CM_OPC_MC_EVENT_AR_OFFLINE_IND_RSP 070/071 CM_OPC_MC_EVENT_AR_OFFLINE_IND/_RSP
@ingroup cm-mc-opcodes
@{
@brief The AR_Offline.IND indication signals that end of the "data phase".
@details
@see @ref CM_EVENT_TYPE

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

See @ref CM_OPC_MC_DEVICE_CONTROL, passivation as well as @ref CM_OPC_MC_DEVICE_REMOVE.

## Result of the request

Return Code          | Meaning
---------------------|--------------------------------
CM_OK                | Success.
******************************************************************************/
typedef struct cm_mc_offline_tag {

	LSA_UINT32 reason_code;
	/**< ## IND
	 * see @ref cm_ar_reason_enum, see documentation for possible values
	 */

} CM_MC_OFFLINE_TYPE;

typedef CM_MC_OFFLINE_TYPE * CM_UPPER_MC_OFFLINE_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_EVENT_AR_OFFLINE_IND/_RSP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 072/073 CM_OPC_MC_EVENT_APDU_STATUS_IND/_RSP                              */
/**************************************************************************//**
@defgroup CM_OPC_MC_EVENT_APDU_STATUS_IND_RSP 072/073 CM_OPC_MC_EVENT_APDU_STATUS_IND/_RSP
@ingroup cm-mc-opcodes
@brief Changes in APDU status are signaled for the user with this indication.
@details
The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

This indication exists only for an M-consumer, not for an M-provider.

The initial APDU status is CM_EVENT_APDU_STATUS_INITIAL_VALUE (Run, Ok, Primary).

@see CM_EVENT_TYPE::mc.apdu_status, and cm_event_apdu_status_enum.

## Result of the request

Return Code          | Meaning
---------------------|--------------------------------
CM_OK                | Success.
******************************************************************************/

/******************************************************************************
end of CM-MC events
******************************************************************************/
#endif


/******************************************************************************
begin of server events
******************************************************************************/
#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE

/*****************************************************************************/
/* 130/131 CM_OPC_SV_AR_CONNECT_IND/_RSP                                     */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_CONNECT_IND_RSP 130/131 CM_OPC_SV_AR_CONNECT_IND/_RSP
@ingroup cm-sv-opcodes
@{
@brief The indication CM_OPC_SV_AR_CONNECT_IND signals that a new AR is being connected. The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

User-requests are allowed after CM_OPC_SV_AR_CONNECT_RSP and before CM_OPC_SV_AR_DISCONNECT_RSP. @todo CM documentation : More Text. Speziell, was bei RTC3 zu beachten ist.

## Session Key (Connection ID) and CM_SV_SESSION_KEY_TO_AR_NR()

The "session_key" identifies a session. A session is an instantiation of an AR (application relationship) in time. An AR is identified by an ARUUID.

A session starts with the indication CM_OPC_SV_AR_CONNECT_IND and ends with the indication CM_OPC_SV_AR_DISCONNECT_IND.

The value of the session key of an explicit AR is never 0 (zero).

The macro CM_SV_SESSION_KEY_TO_AR_NR() is the documented way to convert a "session_key" into the corresponding "ar_nr". The value of the "ar_nr" ranges from 1 to CM_CFG_MAX_SV_ARS.

Note: When an AR gets connected, an arbitrary "ar_nr" out of the range is allocated.

History: In V4.2 an AR was identified by the pair "ar_nr" and "session_key". The plan for V5.0 was to replace the pair by a single value "conn_id" (connection ID). However, it turned out that for some reasons the replacement was not possible. So the parameter named "session_key" is now the connection ID and the parameter "ar_nr" in the alarm-RQB and event-RQB contains the value CM_SV_SESSION_KEY_TO_AR_NR(session_key).

## Special case "nr_of_iocrs == 0":

If the AR allows it (e.g. AR has ARProperties.StartupMode=1), the AR is established without IO-CRs and remains in the RPC-Trigger-Mode (see record index 0xFBFF). The IO controller can read records (ARData, LogBookData, ModuleDiffBlock) to diagnose the problem.

TIA 1678268 (V7.0i1.7): An AR with "pdev-ownership problem" is now rejected. \n
History: TIA 1678268 updates AP01344221 and TIA 1328720 (V6.1i1.4, set up IO-CRs in case of "pdev-ownership problem").

TIA 1868862 (V7.0i1.33): An AR with "send-clock problem" is now rejected. \n
History: TIA 1868862 updates AP01344221.

## Responding rqb.response = CM_ERR_RESOURCE

Usecase: "out of IO-buffers; the EDD-specific parameters are invalid".

If the user changes the value of rqb.response to CM_ERR_RESOURCE, the AR is aborted silently and no CM_OPC_SV_AR_DISCONNECT_IND follows. The EDD-specific parameters are not used.

The PNIOStatus reported in the RPC connect response is: ErrorCode = 0xDB (IODConnectRes), ErrorDecode = 0x81 (PNIO), ErrorCode1 = 255 (User) and ErrorCode2 = 255 (User abort).

## Responding rqb.response = CM_ERR_APPL_RESOURCE

Usecase: "AR parameters violate GSD MaxApplicationXXXLength".

If the user changes the value of rqb.response to CM_ERR_APPL_RESOURCE, the AR is aborted silently and no CM_OPC_SV_AR_DISCONNECT_IND follows.

The PNIOStatus reported in the RPC connect response is: ErrorCode = 0xDB (IODConnectRes), ErrorDecode = 0x81 (PNIO), ErrorCode1 = 2 (faulty IOCRBlockReq) and ErrorCode2 = 8 (parameter DataLength).

History: Introduced with V7.0i2.10 (TIA 2414553)

## Changing the value of "iocr[].cycle_phase"

For the usecase AP00856570 it is allowed to change the value of "iocr[].cycle_phase" in the response to a user-defined value.

## "is_iosar_with_device_access" == CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_YES

The special case of an "easy supervisor" (i.e., an IO supervisor AR with device-access) is described in chapter @ref cm-server-channels-easy-supervisor.

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. a remove-device.
The user has to handle the "late" indication gracefully. For example by responding with CM_ERR_RESOURCE (see above).

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-connect. \n
NAP-Type R1: See additional information in chapter @ref cm-server-channels-r-device-connect.

## ARVendorBlock_cnt, ARVendorBlock_ptr and rqb.response = CM_ERR_ARVENDORBLOCK

@note The feature ARVendorBlock is currently disabled (AP01556507)

An IO controller can send vendor specific information to the IO device by means of one or more ARVendorBlockReq in the connect request PDU. Each ARVendorBlockReq has to be confirmed with a ARVendorBlockRes in the connect response PDU. Refer to [/10/](@ref cm-ref-no-10) and [/11/](@ref cm-ref-no-11).

ARVendorBlock_cnt contains the number of ARVendorBlockReq in the connect request PDU. The maximum number of blocks supported by an IO device is given implicitly by the size of the CLRPC buffer. Refer to CM_DETAIL_TYPE::u.sv.rpc.alloc_len.

The maximum size of an ARVendorBlockRes is CM_SV_ARVENDORBLOCK_RES_MAXSIZE (512 bytes at the time of writing, according to FDIS 61784-2_V23_FINAL__TC2WG11_TO_PNO_CO__Rev15.doc).

If any of the ARVendorBlockReq cannot be interpreted, the indication must be responded with rqb.response = CM_ERR_ARVENDORBLOCK. The AR is aborted silently and no CM_OPC_SV_AR_DISCONNECT_IND follows (compare this to the usecase "Responding rqb.response = CM_ERR_RESOURCE" above).

CM encodes the array of ARVendorBlockReq and ARVendorBlockRes as follows:
 - ARVendorBlock_ptr points to an array of LSA_UINT32.
 - The entries [0..ARVendorBlock_cnt] contain layout information
 - The rest contains the data.

Index                | Array content
---------------------|------------------------------------------------------------
0                    | Offset of 1st block (ARVendorBlockReq and ARVendorBlockRes)
1                    | Offset of 2nd block
...                  | ...
ARVendorBlock_cnt-1  | Offset of last block
ARVendorBlock_cnt    | Total length as a multiple of LSA_UINT32 (offset of "after last block"). Intended usage: HIF; copy all bytes; in both directions.
ARVendorBlock_ptr[0] | Bytes of 1st ARVendorBlockReq and maximal ARVendorBlockRes.
...                  |                                                            |
...                  |                                                            |
...                  |                                                            |
ARVendorBlock_ptr[1] | Bytes of 2nd ARVendorBlockReq and maximal ARVendorBlockRes.
...                  |                                                            |
...                  |                                                            |
...                  |                                                            |
ARVendorBlock_ptr[ARVendorBlock_cnt-1] | Bytes of last ARVendorBlockReq and maximal ARVendorBlockRes.
...                  |                                                            |
...                  |                                                            |
...                  |                                                            |

The BlockType for ARVendorBlockReq is 0x0108.
The BlockType for ARVendorBlockRes is 0x8108.
The only defined BlockVersion for both blocks is V1.0.

The ARVendorBlockRes replaces the ARVendorBlockReq (i.e., it is an in-out parameter). Space is reserved for the maximum ARVendorBlockRes (CM_SV_ARVENDORBLOCK_RES_MAXSIZE).

The following code snippet may be helpful:

@code{.c}

LSA_UINT32 total_bytes = ARVendorBlock_ptr[ar_connect->ARVendorBlock_cnt] * sizeof(ar_connect->ARVendorBlock_ptr[0])

for( unsigned idx = 0; idx < ar_connect->ARVendorBlock_cnt; ++idx )
{
	LSA_UINT8 *blk = (LSA_UINT8 *)&ar_connect->ARVendorBlock_ptr[
	ar_connect->ARVendorBlock_ptr[idx] ];
	// ARVendorBlockReq ::= BlockHeader, APStructureIdentifier, API, [Data*], [Padding*]
	LSA_UINT16 typ = CM_GET16_NTOH(blk, 0); ASSERT(typ == 0x0108);
	LSA_UINT16 len = CM_GET16_NTOH(blk, 2); ASSERT(len >= 1+1+2+4 && ((2+2+len) % 4) == 0);
	LSA_UINT16 ver = CM_GET16_NTOH(blk, 4); ASSERT(ver == 0x0100);
	LSA_UINT16 asi = CM_GET16_NTOH(blk, 6);
	LSA_UINT32 api = CM_GET32_NTOH(blk, 8);
	...
	// do something with the data
	// and make a response
	...
	// ARVendorBlockRes ::= BlockHeader, APStructureIdentifier, API, [Data*], [Padding*]

	LSA_UINT16 datlen = 0; // 0..500 (length of Data*)
	ASSERT(datlen <= 500); // otherwise buffer overflow
	LSA_UINT16 padlen = (((datlen) + 3) & ~3); // padded length
	CM_PUT16_HTON(blk, 0, 0x8108);
	CM_PUT16_HTON(blk, 2, 1+1+2+4 + padlen);
	CM_PUT16_HTON(blk, 4, 0x0100);
	CM_PUT16_HTON(blk, 6, asi); // i.e., unchanged
	CM_PUT32_HTON(blk, 8, api); // i.e., unchanged}
	ASSERT(2+2+1+1 + 2+4 + padlen <= CM_SV_ARVENDORBLOCK_RES_MAXSIZE);
	for( unsigned i = 0; i < datlen; ++i )
	{
		blk[12 + i] = ... // response data
	}
	for( unsigned i = datlen; i < padlen; ++i )
	{
		blk[12 + i] = 0; // padding
	}
}
@endcode

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code          | Meaning
---------------------|--------------------------------
CM_OK                | see text above
CM_ERR_RESOURCE      | see text above
CM_ERR_ARVENDORBLOCK | see text above
CM_ERR_APPL_RESOURCE | see text above
******************************************************************************/

#define CM_SV_SESSION_KEY_TO_AR_NR(session_key)  ((LSA_UINT16)((session_key) & 0xFF)) /**< convert the session key to an AR number */

#define CM_SV_SESSION_KEY_TO_ARSET_NR(session_key)  ((LSA_UINT16)(((session_key) >> 8) & 0x03)) /**< convert the session key to an AR-set number */

#define CM_SV_ARUUID_TO_SRARNR(aruuid)  ((LSA_UINT16)((aruuid).node[5] & 0x7)) /**< extract the "SR-AR number" from ARUUID::Selector */

#define CM_IO_BUFFER_SIZE_ALIGNED(x)          (((x) + 3) & ~3)
#define CM_IO_BUFFER_SIZE_ALIGNED_TRIPLED(x)  (CM_IO_BUFFER_SIZE_ALIGNED(x) * 3)

#define CM_SV_ARVENDORBLOCK_RES_MAXSIZE  (2+2+1+1 + 2+4 + 500) /**< maximum size of the ARVendorBlockRes (FDIS 61784-2_V23_FINAL__TC2WG11_TO_PNO_CO__Rev15.doc) */

typedef struct cm_sv_ar_connect_iocr_tag {

	LSA_UINT16 iocr_ref;
	/**< ## IND
	 * Refer to [/11/](@ref cm-ref-no-11), "IOCRReference", rsp: unchanged(!) @note This is just a number and not usable as an array-index.
	 */

	LSA_UINT16 frame_id;
	/**< ## IND
	 * The "FrameID" of the IOCR.
	 */

	LSA_UINT16 iocr_type;
	/**< ## IND
	 * Refer to [/11/](@ref cm-ref-no-11), "IOCRType", see also defines @ref CM_IOCR_TYPE_RESERVED_0.
	 */

	LSA_UINT32 iocr_properties;
	/**< ## IND
	 *Refer to /11/, "IOCRProperties", see also defines @ref CM_IOCR_PROP_RT_CLASS_MASK, ...
	 */

	LSA_USER_ID_TYPE iocr_user_id;
	/**< ## IND
	 * = 0
	 * ## RSP
	 * User-defined value; This value will be indicated as data_user_id and iocs_user_id later in the @ref CM_OPC_SV_AR_OWNERSHIP_IND. \n
	 * E.g., for an ERTEC400 this could be the pointer to the io-buffer.
	 */

	LSA_UINT16 reduction_ratio;
	/**< ## IND
	 * The "Reduction Ratio" of the IOCR (since V5.2i4.37 / V5.3i2.13 / V6.0i1.11, TIA 216515) (V5.2i4, TIA 216515)
	 */

	LSA_UINT16 cycle_phase;
	/**< ## IND
	 * The "Phase" of the IOCR.
	 * ## RSP
	 * Unchanged or user-defined (intended usage for an altered value is AP00856570).
	 */

	LSA_UINT16 sum_iodata;
	/**< ## IND
	 * The sum of SubmoduleDataLength of all submodules contained in this IOCR (V7.1i1.6, TIA 2612940). \n
	 * Note: The bits SubmoduleProperties::ReduceInputSubmoduleDataLength/ReduceOutputSubmoduleDataLength are not related to this calculation. \n
	 * Intended use case: Checking of the GSD parameters MaxApplicationXxxLength. If ApplicationLengthIncludesIOxS is true, add \c sum_ioxs to this value.
	 */

	LSA_UINT16 sum_ioxs;
	/**< ## IND
	 * The sum of LengthIOPS/LengthIOCS of all submodules contained in this IOCR (V7.1i1.6, TIA 2612940). \n
	 * Note: The bit SubmoduleProperties::DiscardIOXS is not related to this calculation. \n
	 * Intended use case: See \c sum_iodata.
	 */

	LSA_UINT16 io_buffer_size;
	/**< ## IND
	 * IOCRBlockReq::DataLength or less if the HW can "partial data" and/or "autopadding"
	 * @note The "io_buffer_size" may not be the same as the alloc-size.
	 * depending on the HW it has to be incremented by apdu_status_length and/or aligned and/or tripled.
	 * see CM_IO_BUFFER_SIZE_ALIGNED() and CM_IO_BUFFER_SIZE_ALIGNED_TRIPLED()
	 */

	LSA_UINT16 eddx_apdu_status_length;
	/**< ## IND
	 * EDD-specific: 0 or 4 if EDD_HW_TYPE_FEATURE_NO_APDUSTATUS_SEPARATE
	 */

	LSA_UINT32 eddx_data_status_offset;
	/**< ## IND
	 * EDD-specific: see EDD_CSRT_PROV_IO_PARAMS_TYPE::DataStatusOffset, EDD_DATAOFFSET_INVALID if the APDUStatus _is_ part of the io_buffer
	 */

	LSA_UINT32 eddx_data_offset;
	/**< ## RSP
	 * EDD-specific: offset to the "io_buffer" with size "io_buffer_size" + adjustments, see EDD_CSRT_PROV_IO_PARAMS_TYPE::DataOffset and EDD_CSRT_CONS_IO_PARAMS_TYPE::DataOffset
	 */

	LSA_UINT32 eddi_cw_offset;
	/**< ## IND
	 * EDD-specific: see EDD_CSRT_PROV_IO_PARAMS_TYPE::CWOffset
	 */

	LSA_UINT32 eddi_apdu_status_offset;
	/**< ## IND
	 * EDD-specific: see EDD_CSRT_CONS_IO_PARAMS_TYPE::APDUStatusOffset
	 */

	LSA_UINT32 eddi_buffer_properties;
	/**< ## RSP:
	 * EDD-specific: EDD_CSRT_PROV_IO_PARAMS_TYPE::BufferProperties and EDD_CSRT_CONS_IO_PARAMS_TYPE::BufferProperties
	 */

	LSA_UINT16 eddx_cr_number;
	/**< ## RSP:
	 * EDD-specific: see EDD_CSRT_PROV_IO_PARAMS_TYPE::CRNumber and EDD_CSRT_CONS_IO_PARAMS_TYPE::CRNumber \n
	 * EDDS see EDDS_IOBuffer_ProviderAlloc() and EDDS_IOBuffer_ConsumerAlloc(), V5.3i1 AP01403254
	 */

	LSA_UINT8 eddp_forward_mode;
	/**< ## RSP
	 * EDD-specific: see EDD_CSRT_CONS_IO_PARAMS_TYPE::ForwardMode
	 */

	LSA_UINT32 eddp_ct_outdatastart;
	/**< ## RSP
	 * EDD-specific: see EDD_CSRT_CONS_IO_PARAMS_TYPE::CutThrough_OutDataStart
	 */

} CM_SV_AR_CONNECT_IOCR_TYPE; /**< The iocr type used for CM_OPC_SV_AR_CONNECT_IND/_RSP. */

typedef CM_SV_AR_CONNECT_IOCR_TYPE * CM_UPPER_SV_AR_CONNECT_IOCR_PTR_TYPE; /**< The iocr pointer type used for CM_OPC_SV_AR_CONNECT_IND/_RSP. */

#define CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_NO  0
#define CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_YES 1

#define CM_SV_SR_FIRST_AR_NO  0
#define CM_SV_SR_FIRST_AR_YES 1

#define CM_SV_HAS_RSINFOBLOCK_NO  0
#define CM_SV_HAS_RSINFOBLOCK_YES 1

#define CM_SV_WAS_IN_DATA_NO  0
#define CM_SV_WAS_IN_DATA_YES 1

typedef struct cm_sv_ar_connect_tag {

	LSA_UINT32 host_ip;
	/**< ## IND
	 * caller information: network byte order, IP-address (the IOC or IOS)
	 */

	CM_COMMON_MEM_U8_PTR_TYPE cmi_station_name;
	/**< ## IND
	 * caller information: Refer to [/11/](@ref cm-ref-no-11), "CMInitiatorStationName, zero-terminated string.
	 */

	CLRPC_UUID_TYPE cmi_obj_uuid;
	/**< ## IND
	 * caller information: Refer to [/11/](@ref cm-ref-no-11), CMInitiatorObjectUUID
	 */

	CLRPC_UUID_TYPE ar_uuid;
	/**< ## IND
	 * caller information: Refer to [/11/](@ref cm-ref-no-11), ARUUID
	 */

	LSA_UINT16 alarm_send_max_length;
	/**< ## IND
	 * See @ref CM_ALARM_TYPE::alarm_data_length \n
	 * "Real time ARs": Value = AlarmCRBlockReq::MaxAlarmDataLength - CM_ALARM_OFFSET_DATA. \n
	 * "Device access ARs": Value = 0. \n
	 * See too @ref CM_OPC_SV_DEVICE_ADD, parameter "alarm_send_max_length" \n
	 * AR information: = AlarmCRBlockReq::MaxAlarmDataLength - CM_ALARM_OFFSET_DATA
	 */

	LSA_UINT16 ar_type;
	/**< ## IND
	 * AR information: Refer to [/11/](@ref cm-ref-no-11), see defines @ref CM_AR_TYPE_SINGLE, ...
	 */

	LSA_UINT32 ar_properties;
	/**< ## IND
	 * AR information: Refer to [/11/](@ref cm-ref-no-11), see defines @ref CM_AR_PROP_STATE_MASK, ...
	 */

	LSA_UINT8 is_iosar_with_device_access;
	/**< ## IND
	 * AR information: \n
	 * CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_YES .. the AR is an CM_AR_TYPE_SUPERVISOR with CM_AR_PROP_DEVICE_ACCESS_YES \n
	 * CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_NO ... otherwise. \n
	 * See text and chapter @ref cm-server-channels-easy-supervisor.
	 */

	CM_COMMON_MEM_U8_PTR_TYPE ARFSUBlock_ptr;
	/**< ## IND
	 * AR information for FSU (fast startup):  LSA_NULL or pointer to the ARFSUBlock (V5.0i5, AP01034170) \n
	 * Refer to [/11/](@ref cm-ref-no-11), "ARFSUBlock". \n
	 * Functionality introduced in V5.0i5 (AP01034170). \n
	 * See also parameter "can_ARFSUDataAdjust" of @ref CM_OPC_SV_CREATE
	 */

	LSA_UINT32 sr_rdht_msec;
	/**<  ## IND
	 * AR information for CM_AR_TYPE_SINGLE_SYSRED (system redundancy): \n
	 * redundancy data hold time, in milliseconds
	 */

	LSA_UINT32 sr_properties;
	/**< ## IND
	 * AR information for CM_AR_TYPE_SINGLE_SYSRED (system redundancy): \n
	 * SR-Properties. Valid only for CM_AR_TYPE_SINGLE_SYSRED, see CM_SR_PROP_INP_VALID_ON_BACKUP_AR, etc.
	 */

	LSA_UINT8 sr_firstAR;
	/**< ## IND
	 * AR information for CM_AR_TYPE_SINGLE_SYSRED (system redundancy): \n
	 * CM_SV_SR_FIRST_AR_YES... this AR is the "first AR" of the AR-Set.
	 * Making an AR of the AR-Set the Primary-AR invalidates this flag.
	 * Intended usage: R1 device. See chapter @ref cm-server-channels-r-device.
	 * However, the flag may be useful for an S2 device too.
	 * CM_SV_SR_FIRST_AR_NO.... otherwise
	 */

	LSA_UINT32 ARVendorBlock_cnt;
	/**< ## IND
	 * AR vendor-specific information for CM_AR_PROP_STARTUP_MODE_ADVANCED: \n
	 * 0 or number of ARVendorBlockReq/ARVendorBlockRes contained in ARVendorBlock_ptr.
	 */

	LSA_UINT32 * ARVendorBlock_ptr;
	/**< ## IND
	 * AR vendor-specific information for CM_AR_PROP_STARTUP_MODE_ADVANCED: \n
	 * LSA_NULL (if ARVendorBlock_cnt == 0) or pointer to array of ARVendorBlockReq/ARVendorBlockRes. See Text. Functionality introduced in V5.2i3 (AP01354956).
	 */

	LSA_UINT8 has_RSInfoBlock;
	/**< ## IND
	 * AR information for "Reporting System" (only for CM_AR_PROP_STARTUP_MODE_ADVANCED): \n
	 * see text (TIA 1645525) \n
	 * CM_SV_HAS_RSINFOBLOCK_NO... the connect PDU does not contain an RSInfoBlock
	 * CM_SV_HAS_RSINFOBLOCK_YES.. the connect PDU contains an RSInfoBlock - only if advanced AR and only if CM_SV_DEVICE_ADD_TYPE::gsd_ReportingSystemSupported !=0. \n
	 */

	struct {

		LSA_UINT32 reason_code;
		/**< ## IND
		 * see @ref cm_ar_reason_enum
		 */

		LSA_UINT32 reason_pnio_status;
		/**< ## IND
		 * For AP00678183 only. If reason_code == CM_AR_REASON_ASRT this field contains the pnio_status from the asrt-error packet.
		 */

		LSA_UINT8 was_in_data;
		/**< ## IND
		 * CM_SV_WAS_IN_DATA_YES if the AR was aborted after in-data
		 * CM_SV_WAS_IN_DATA_NO  otherwise
		 */

	} disconnect; /**< @attention This field is for @ref CM_OPC_SV_AR_DISCONNECT_IND only. */

	LSA_UINT16 send_clock_31250ns;
	/**< ## IND
	* IOCR information: \n
	* The "Send Clock" of the IOCRs in units of 31.25µs (since V5.2i4.37 / V5.3i2.13 / V6.0i1.11, TIA 216515). \n
	 * @note All IOCRs have the same send-clock, 0 if nr_of_iocrs == 0!
	 */

	LSA_UINT16 nr_of_iocrs;
	/**< ## IND
	 * IOCR information: \n
	 * Number of elements contained in the array "iocr" (0 or 2..n). See special case "nr_of_iocrs == 0" in the text.
	 */

	CM_SV_AR_CONNECT_IOCR_TYPE iocr[1];
	/**< ## IND
	 * IOCR information: \n
	 * iocr[2..n] follow here
	 */

} CM_SV_AR_CONNECT_TYPE;

typedef CM_SV_AR_CONNECT_TYPE * CM_UPPER_SV_AR_CONNECT_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_CONNECT_IND/_RSP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 132/133 CM_OPC_SV_AR_DISCONNECT_IND/_RSP                                  */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_DISCONNECT_IND_RSP 132/133 CM_OPC_SV_AR_DISCONNECT_IND/_RSP
@ingroup cm-sv-opcodes
@brief The indication CM_OPC_SV_AR_DISCONNECT_IND signals that the AR got disconnected. The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

The reason for the disconnection is also entered in the device's log-book (chapter @ref cm-server-channels-device-logbook), which can be queried remotely.

Before signaling the indication, CM-SV has passivated the IOCRs. The buffers can be freed now. Information like "iocr_user_id" or the EDD-specific parameters may be helpful.

@see cm-ar-reasons

@attention This opcode uses the same opcode as @ref CM_OPC_SV_AR_CONNECT_IND. See CM_SV_AR_CONNECT_TYPE::disconnect for the parameters. The other parameters stay the same as they were in CM_OPC_SV_AR_CONNECT_IND.

## Changed timing as of V5.3i1.1 (AP01367655, V5.2i4.8):

The indication CM_OPC_SV_AR_DISCONNECT_IND is deferred until all outstanding responses (CM_OPC_SV_AR_xxx_RSP) to other indications (of this AR) are back.

@note This implies that the disconnect indication cannot be used for cancellation of other running indication. Note too that this kind of usage was not documented formerly either.

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the CM_OPC_SV_AR_DISCONNECT_IND and the user (in another thread context) issues an AR-specific request (i.e., @ref CM_OPC_SV_AR_CONTROL, @ref CM_OPC_SV_AR_ABORT, @ref CM_OPC_SV_AR_RIR_ACK, @ref CM_OPC_SV_AR_APPL_READY, @ref CM_OPC_SV_AR_ALARM_SEND, @ref CM_OPC_SV_AR_ALARM_ACK at the time of writing).
The user has to wait for the confirmations before deleting the AR-specific instance.

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-disconnect. \n
NAP-Type R1: See additional information in chapter @ref cm-server-channels-r-device-disconnect.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/


/*****************************************************************************/
/* 134/135 CM_OPC_SV_AR_OWNERSHIP_IND/_RSP                                   */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_OWNERSHIP_IND_RSP 134/135 CM_OPC_SV_AR_OWNERSHIP_IND/_RSP
@ingroup cm-sv-opcodes
@{
@brief The indication CM_OPC_SV_AR_OWNERSHIP_IND signals an ownership-change. The indication has to be responded. However, there are some requirements that make things not so straight-forward.
@details
@see @ref CM_EVENT_TYPE

If a submodule is pulled or the ownership is rejected, "is_wrong" shall be set to CM_SV_IS_WRONG_YES for clarity reasons.

Refer also to chapter @ref cm-server-channels-parameterization and chapter @ref cm-server-channels-ownership.

The different cases for "ar_context" are described in the subchapters below in the form of pseudo-code.

## IOPS of PDEV submodules (V6.0i2.46, TIA 1403121)

As of V6.0i2.46 the IOPS of a PDEV submodule has to be controlled according to TIA 1403121.

**PULLED:**
Submodule is pulled (not plugged) -> IOPS = BAD.
Note that typically the io-buffer already contains BAD due to buffer-initialization and the rule "on pull -> set IOPS = BAD" in chapter 12.18.

**WRONG:**
Submodule is plugged but ident-numbers don't match -> IOPS = BAD.
See too the subchapter "Parameter "is_wrong" in the Ownership.IND (AP00881547)" below.

**OK / SUBST:**
Submodule is plugged and is the expected one or a substitute -> IOPS = GOOD.
Although it is possible to set the IOPS "now" it is recommended to defer the action until PrmEnd.IND (chapter 12.32).

## PDEV submodules, rules for WRONG, COMPACT, MODULAR (V6.0i2.41)

See Pseudo-Code below for CONTEXT_CONN and CONTEXT_PLUG (especially the text with green background).

@note Only the user has the knowledge to decide if a submodule is WRONG or SUBST. This result is returned to CM in the response. Because of this the logic cannot be implemented in CM. In order to find errors early, CM applies the same rules when processing the ownership response and calls FATAL if there is an inconsistency (as decided in TIA 1449259).

## Pseudo-code for ar_context == CM_SV_AR_CONTEXT_CONN

The IOD-User must have knowledge about the PDev submodule-properties.

@code
// The indication contains all submodules of the expected configuration.
foreach elem in elem[] {
	if( elem->owner_session_key == 0 ) { // ownership not offered
		// see usecases below
		elem->owner_session_key = unchanged (i.e., locked for this AR)
	}
	else { // ownership offered
		sm = GetRealSubmodule(elem->api, elem->slot, elem->subslot)
		if( sm == 0 ) { // pulled
			elem->owner_session_key = unchanged (not zero!)
		}
		else if( sm->IsSuperordinatedLocked() ) {
			elem->owner_session_key = 0 // reject ownership
		}
		else if( sm->IsPDevSubmdodule() && IsPdevLocked(elem[]) ) { // see below
			elem->owner_session_key = 0 // reject ownership
		}
		else if( sm->GetCurrentOwner() == 0 ) { // currently not owned
			sm->SetCurrentOwner(elem->owner_session_key, ... other parameters ...)
			elem->owner_session_key = unchanged // accept ownership
		}
		else if( sm->GetCurrentOwner() == elem->owner_session_key ) {
			// unreachable
		}
		else { // owned by another AR, ownership is taken over by this supervisor AR
			// set IOXS = BAD in the existing AR (idea: pulled logically)
			sm->SetIOXS(BAD)
			sm->RemoveFromIoUpdateList()
			// and make the new AR the current owner
			sm->SetCurrentOwner(elem->owner_session_key, ... other parameters ...)
		}
	}
}

// Helper-Function //
bool IsPdevLocked( elem[] ) {
	port_found = FALSE;
	foreach pdev_sm in PDev { // see CM_OPC_PD_CREATE
		found = find_pdev_sm_in_elem (elem[], pdev_sm->slot_nr, pdev_sm->subslot_nr)
		if ( pdev_sm->IsCompact() ) {
			if ( ! found ) {
				return TRUE // Compact Port not found in AR
			}
			if ( pdev_sm->IsWrong() ) {
				return TRUE // Compact + Wrong
							// Note: the Interface is always of type Compact
			}
		}
		if ( found  &&  pdev_sm.IsPort() ) {
			port_found = TRUE
		}
	}
	if ( ! port_found ) {
		return TRUE // AR without ports
	}
	return FALSE;
}
@endcode

## Pseudo-code for ar_context == CM_SV_AR_CONTEXT_PLUG

System Redundancy ARs: Must check for "late" -> see chapter @ref cm-server-channels-sysred-ownership-plug for the details.

@code
// The indication contains submodules that need to be parameterized.
// Note that this is similar but not equal to CM_SV_AR_CONTEXT_CONN.
foreach elem in elem[] {
	if( elem->owner_session_key == 0 ) { // ownership not offered
		// unreachable
	}
	else { // ownership offered
		sm = GetRealSubmodule(elem->api, elem->slot, elem->subslot)
		if( sm == 0 ) { // already pulled
			elem->owner_session_key = unchanged (not zero!)
		}
		else if( sm->IsSuperordinatedLocked() ) {
			elem->owner_session_key = 0 // reject ownership
		}
		else if( sm->IsPDevSubmdodule() && WasPdevLockedOnConnect() ) { // see CONTEXT_CONN
			elem->owner_session_key = 0 // reject ownership
		}
		else if( sm->GetCurrentOwner() == 0 ) { // currently not owned
			sm->SetCurrentOwner(elem->owner_session_key, ... other parameters ...)
			elem->owner_session_key = unchanged // accept ownership
		}
		else if( sm->GetCurrentOwner() == elem->owner_session_key ) { // currently owned
			elem->owner_session_key = unchanged (plugged-pulled-plugged)
		}
		else { // owned by another AR
			// unreachable
		}
	}
}
@endcode

## Pseudo-code for ar_context == CM_SV_AR_CONTEXT_DISC

@code
// The indication is fired as soon as possible after the AR gets disconnected.
// The indication contains only the submodules that were owned by the AR.
foreach elem in elem[] {
	if( elem->owner_session_key == 0 ) {
		// unreachable
	}
	else {
		sm = GetRealSubmodule(elem->api, elem->slot, elem->subslot)
		if( sm == 0 ) { // pulled
			// ignore (IOXS were set to BAD when pulling)
		}
		else if( sm->GetCurrentOwner() != elem->owner_session_key ) { // late (see take-over)
			// ignore (IOXS were set to BAD on take-over)
		}
		else { // relinquish ownership
			sm->SetIOXS(BAD)
			sm->RemoveFromIoUpdateList()
			sm->SetCurrentOwner(0)
		}
	}
}
@endcode

## Pseudo-code for ar_context == CM_SV_AR_CONTEXT_PULLPDEV

@code
// The indication contains the pulled pdev-submodule
foreach elem in elem[] {
	exp = GetExpectedSubmodule(elem->api, elem->slot, elem->subslot)
	exp->SetIOXS(BAD)
}
@endcode

## Usecase "dynamic adaption of the real configuration"

If "ar_context" == CM_SV_AR_CONTEXT_CONN, then the indication contains all submodules of the expected configuration (i.e, it too contains submodules that are pulled and submodules that are already owned by another AR).

The user may pull or plug submodules before responding the indication. No additional alarms will be sent for the submodules with "owner_session_key != 0".

Refer also to chapter @ref cm-server-channels-parameterization.

## Parameter "is_wrong" in the Ownership.IND (AP00881547)

Usecase: implementing pre-AP00826555 behaviour if the real configuration is not accessible in the context of the Ownership.IND.

The parameter "is_wrong" in the Ownership.IND is preset as follows:

@code
	IF "is pulled"
		is_wrong = CM_SV_IS_WRONG_NO
	ELSIF expected mod_ident != real mod_ident
		is_wrong = CM_SV_IS_WRONG_YES
	ELSIF expected sub_ident != real sub_ident
		is_wrong = CM_SV_IS_WRONG_YES
	ELSE
		is_wrong = CM_SV_IS_WRONG_NO
@endcode

## M-provider and M-consumer submodules

@note M-providers are currently not supported (due to AP00774495).

@note M-consumers are currently not supported (due to AP00774501).

Definition: A submodule is called an M-provider submodule if it is contained in one (or more) M-provider CRs. A submodule is called an M-consumer submodule if it is contained in (exactly) one M-consumer CR. An input/output submodule can be both simultaneously.

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. an AR-abort.
The user has to handle the "late" indication gracefully.

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-ownership. \n
NAP-Type R1: See additional information in chapter @ref cm-server-channels-r-device-ownership.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/
#define CM_IO_BUFFER_INP_DATA_LENGTH(elem_ptr)  ((LSA_UINT16)( ((elem_ptr)->sub_properties & CM_SUB_PROP_REDUCE_INP_LEN_MASK) == CM_SUB_PROP_REDUCE_INP_LEN_YES ? 0 : (elem_ptr)->descr[0/*inp*/].data_length ))
#define CM_IO_BUFFER_OUT_DATA_LENGTH(elem_ptr)  ((LSA_UINT16)( ((elem_ptr)->sub_properties & CM_SUB_PROP_REDUCE_OUT_LEN_MASK) == CM_SUB_PROP_REDUCE_OUT_LEN_YES ? 0 : (elem_ptr)->descr[1/*out*/].data_length ))

#define CM_IO_BUFFER_INP_IOPS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[0/*inp*/].iops_length ))
#define CM_IO_BUFFER_OUT_IOPS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[1/*out*/].iops_length ))

#define CM_IO_BUFFER_INP_IOCS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[0/*inp*/].iocs_length ))
#define CM_IO_BUFFER_OUT_IOCS_LENGTH(elem_ptr)  ((LSA_UINT8)( ((elem_ptr)->sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ? 0 : (elem_ptr)->descr[1/*out*/].iocs_length ))


typedef struct cm_sv_ar_ownership_element_descr_tag {

	LSA_UINT16 data_description;
	/**< ## IND
	 * See defines @ref CM_SUB_DATA_DESCR_TYPE_MASK, ..., refer also to [/11/](@ref cm-ref-no-11).
	 * @note that CM_SUB_DATA_DESCR_TYPE_MASK is consistent with CM_SUB_PROP_TYPE_MASK.
	 */

	LSA_USER_ID_TYPE data_user_id;
	/**< ## IND
	 * The "io_buffer" containing IOData and IOPS, see @ref CM_SV_AR_CONNECT_IOCR_TYPE::iocr_user_id.
	 */

	LSA_UINT16 data_offset;
	/**< ## IND
	 * offset of IOData
	 * @note In case of CM_SUB_PROP_DISCARD_IOXS_YES the offset can be equal to "io_buffer_size".
	 */

	LSA_UINT16 data_length;
	/**< ## IND
	 * Expected length of IOData, for the length of IOData in the "io_buffer" see CM_IO_BUFFER_INP_DATA_LENGTH() and CM_IO_BUFFER_OUT_DATA_LENGTH().
	 */

	LSA_UINT8 iops_length;
	/**< ## IND
	 * Expected length of IOPS (following IOData). \n
	 * iops_offset = data_offset + CM_IO_BUFFER_xxx_DATA_LENGTH() \n
	 * For the length of IOPS in the "io_buffer" see CM_IO_BUFFER_INP_IOPS_LENGTH() and CM_IO_BUFFER_OUT_IOPS_LENGTH().
	 */

	LSA_USER_ID_TYPE iocs_user_id;
	/**< ## IND
	 * The "io_buffer" containing IOCS, see @ref CM_SV_AR_CONNECT_IOCR_TYPE::iocr_user_id
	 */

	LSA_UINT16 iocs_offset;
	/**< ## IND
	 * Offset of IOCS.
	 * @note In case of CM_SUB_PROP_DISCARD_IOXS_YES the offset can be equal to "io_buffer_size".
	 */

	LSA_UINT8 iocs_length;
	/**< ## IND
	 * Expected length of IOCS. \n
	 * For the length of IOCS in the "io_buffer" see CM_IO_BUFFER_INP_IOCS_LENGTH() and CM_IO_BUFFER_OUT_IOCS_LENGTH().
	 */

} CM_SV_AR_OWNERSHIP_ELEMENT_DESCR_TYPE; /**< The element description type used for CM_OPC_SV_AR_OWNERSHIP_IND/_RSP. */

typedef CM_SV_AR_OWNERSHIP_ELEMENT_DESCR_TYPE * CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_DESCR_PTR_TYPE; /**< The element description pointer type used for CM_OPC_SV_AR_OWNERSHIP_IND/_RSP. */

#define CM_SV_IS_WRONG_NO  0
#define CM_SV_IS_WRONG_YES 1

typedef struct cm_sv_ar_ownership_element_tag {

	LSA_UINT32 api;
	/**< ##IND
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ##IND
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/**< ##IND
	 * subslot number \n
	 * For value range, refer to AddDevice. In API 0 the range from 0x8000 - 0x8FFF is allowed additionally for interfaces and ports. \n
	 * For more details see the text below and chapter @ref cm-server-channels-device-model-hierarchy.
	 */

	/***/

	LSA_UINT32 mod_ident;
	/**< ##IND
	 * module ident number
	 */

	LSA_UINT16 mod_properties;
	/**< ##IND
	 * moduleProperties, see defines @ref CM_MOD_PROP_UNCHECKED_0_7, ...
	 */

	LSA_UINT32 sub_ident;
	/**< ##IND
	 * submodule ident number
	 */

	LSA_UINT16 sub_properties;
	/**< ##IND
	 * SubmoduleProperties, see defines @ref CM_SUB_PROP_TYPE_MASK, ...: \n
	 *
	 * CM_SUB_PROP_TYPE_MASK         | descr[]
	 * ------------------------------|--------------
	 * CM_SUB_PROP_TYPE_NO_DATA      | Index 0
	 * CM_SUB_PROP_TYPE_INPUT        | Index 0
	 * CM_SUB_PROP_TYPE_OUTPUT       | Index 1
	 * CM_SUB_PROP_TYPE_INPUT_OUTPUT | Index 0 and 1
	 */

	CM_SV_AR_OWNERSHIP_ELEMENT_DESCR_TYPE descr[2];
	/**< ## IND
	 * Index 0=input description \n
	 * Index 1=output description
	 */

	/***/

	LSA_UINT8 is_wrong;
	/**< ## IND
	 * see text (AP00881547)
	 * ## RSP"
	 * CM_SV_IS_WRONG_NO ... this submodule is OK or a SUBSTITUTE.
	 * CM_SV_IS_WRONG_YES .. this submodule is WRONG (or pulled).
	 * The value of the response is evaluated only if the submodule is plugged and it is not the expected one (i.e., expected module and/or submodule ident number != real module and/or submodule ident number).
	 * In other words: "wrong" can be upgraded to "substitute", but "ok" cannot be downgraded to "wrong".
	 */

	LSA_UINT16 owner_session_key;
	/**< ## IND
	 * see table
	 * ## RSP
	 * see table
	 *
	 * IND            | Context   | User                 | RSP
	 * ---------------|-----------|----------------------|-----------------------------------------------
	 * 0              | CONN      | -                    | unchanged (see usecase dynamic adaption below)
	 * n<sup>1)</sup> | CONN      | reject<sup>2)</sup>  | 0 (superordinated locked) set IOPS/IOCS=BAD
	 * n<sup>1)</sup> | CONN      | accept<sup>2)</sup>  | unchanged (start of parameterization)
	 * n<sup>1)</sup> | PLUG      | reject               | 0 (superordinated locked) set IOPS/IOCS=BAD
	 * n<sup>1)</sup> | PLUG      | accept               | unchanged (start of parameterization)
	 * n<sup>1)</sup> | DISC      | relinquished         | 0 (for clarity reasons) set IOPS/IOCS=BAD
	 * n<sup>1)</sup> | PULL-PDEV | -                    | unchanged set IOPS/IOCS=BAD
	 *
	 * <sup>1)</sup> n == rqb.args.sv.ar_event.session_key. \n
	 * <sup>2)</sup> if the submodule has another owner -> is supervisor-takeover. All other combinations of IND / Context are not possible.
	 *
	 * If 0 is responded, then "is_wrong" will not be evaluated (don't care).
	 */

} CM_SV_AR_OWNERSHIP_ELEMENT_TYPE; /**< The element type used for CM_OPC_SV_AR_OWNERSHIP_IND/_RSP. */

typedef CM_SV_AR_OWNERSHIP_ELEMENT_TYPE * CM_UPPER_SV_AR_OWNERSHIP_ELEMENT_PTR_TYPE; /**< The element pointer type used for CM_OPC_SV_AR_OWNERSHIP_IND/_RSP. */

#define CM_SV_AR_CONTEXT_CONN      1 /**< Indication context is 'connect' */
#define CM_SV_AR_CONTEXT_DISC      2 /**< Indication context is 'disconnect' */
#define CM_SV_AR_CONTEXT_PLUG      3 /**< Indication context is 'plug' */
#define CM_SV_AR_CONTEXT_PULLPDEV  4 /**< Indication context is 'pull PDev' */

typedef struct cm_sv_ar_ownership_tag {

	LSA_UINT16 ar_context;
	/**< ## IND
	 * see text, and CM_SV_AR_CONTEXT_CONN, CM_SV_AR_CONTEXT_DISC, CM_SV_AR_CONTEXT_PLUG and CM_SV_AR_CONTEXT_PULLPDEV
	 */

	LSA_UINT8 set_mrp_off;
	/**< ## IND
	 * always CM_SET_MRP_OFF_NO
	 * ## RSP
	 * If CM_SET_MRP_OFF_YES and no other MRP record was written then CM-PD generates an "MRP off" record (AP00824058). \n
	 * The flag has no effect if MRP records were written or deleted via @ref CM_OPC_SV_AR_PDEVPRM_WRITE (V6.0i2.27 TIA 1323566).
	 * Evaluated only in connect-phase (ar_context == CM_SV_AR_CONTEXT_CONN).
	 * Takes effect only if the AR contains and owns the PDEV.
	 * For R1 devices must be set to CM_SET_MRP_OFF_NO, see @ref cm-server-channels-r-device.
	 */

	LSA_UINT16 nr_of_elems;
	/**< ## IND
	 * Number of elements contained in the array "elem" (1..n).
	 */

	CM_SV_AR_OWNERSHIP_ELEMENT_TYPE elem[1];
	/**< ## IND
	 * elem[2..n] follow here
	 */

} CM_SV_AR_OWNERSHIP_TYPE;

typedef CM_SV_AR_OWNERSHIP_TYPE * CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_OWNERSHIP_IND/_RSP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 136/137 CM_OPC_SV_AR_PRM_END_IND/_RSP                                     */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_PRM_END_IND_RSP 136/137 CM_OPC_SV_AR_PRM_END_IND/_RSP
@ingroup cm-sv-opcodes
@{
@brief The indication CM_OPC_SV_AR_PRM_END_IND signals the end of the parameterization. The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

In the connect phase (CM_SV_AR_CONTEXT_CONN) this indication will be signaled always (as of V5.1i5.62 / V5.2i4.20 / V5.3i1.9). If e.g. all submodules are "pulled", an empty indication (nr_of_elems == 0) will be signaled.

This indication can be used as a trigger to start updating the inputs (usecase AP01503040).

## Elem[] for PDEV submodules (V6.0i2.46, TIA 1403121)

PDEV submodules are submodules where (elem[].subslot_nr & 0xF000) == 0x8000.
For R1-Devices the "right" PDEV matches (elem[].subslot_nr & 0xF000) == 0x9000.

PDEV submodules have to be handled differently since V6.0i2.23. For the PDEV submodules the parameters "appl_ready_follows" and "appl_ready_pending" changed direction from "RSP" to "IND".

The parameter "appl_ready_follows" is always indicated as CM_SV_APPL_READY_FOLLOWS_NO and must be unchanged in the response. This means that for PDEV submodules there is no usecase "tell status later".

The parameter "appl_ready_pending" shows the result of the PDEV parameterization (success or failure) and must be unchanged in the response.

Since V6.0i2.46 the value of "appl_ready_pending" is for information only (TIA 1403121). See ownership indication chapter @ref CM_OPC_SV_AR_OWNERSHIP_IND for setting of the IOPS.

History: As of V6.0i2.23 the user had to set the IOPS according to "appl_ready_pending" (TIA 1193013).

History: As of V5.3i2.1 the behavior of the PDev has changed: If the PDev cannot be parameterized, the faulty parameterization is replaced by the "empty parameterization" (see chapter @ref CM_OPC_PD_PRM_BEGIN)

## Usecase "tell status now"

If at the time of the CM_OPC_SV_AR_PRM_END_RSP the status of a submodule (ready or not-ready) is known, the user sets the flags as follows:

 - appl_ready_follows = CM_SV_APPL_READY_FOLLOWS_NO;
 - appl_ready_pending = is-not-ready ? CM_SV_APPL_READY_PENDING_YES : CM_SV_APPL_READY_PENDING_NO;

In this case the request CM_OPC_SV_AR_APPL_READY for this submodule is omitted.

Note that "application-ready-pending" can be changed later to "application-ready" by means of a return-of-submodule alarm. See chapter @ref CM_OPC_SV_AR_ALARM_SEND.

Telling "application ready" means that valid input-data and IOPS was written into the io-buffer.

@attention Wie ist das bei RTC3? Kann man den Puffer immer Updaten oder geht vor RIR der Puffertausch nicht?

## Usecase "tell status later with CM_OPC_SV_AR_APPL_READY"

If at the time of the CM_OPC_SV_AR_PRM_END_RSP the status of the submodule (ready or not-ready) is not yet known, the user sets the flags as follows:

 - appl_ready_follows set to CM_SV_APPL_READY_FOLLOWS_YES;
 - appl_ready_pending set to CM_SV_APPL_READY_PENDING_YES; // don't care

The user has to call @ref CM_OPC_SV_AR_APPL_READY  later, when the status is known.

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. a remove-submodule.
The user has to handle the "late" indication gracefully.

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-prm-end. \n
NAP-Type R1: The things said for S2 apply to R1 too.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/

#define CM_SV_APPL_READY_FOLLOWS_NO  0
#define CM_SV_APPL_READY_FOLLOWS_YES 1

#define CM_SV_APPL_READY_PENDING_NO  0
#define CM_SV_APPL_READY_PENDING_YES 1

typedef struct cm_sv_ar_prm_end_element_tag {

	LSA_UINT32 api;
	/**< ## IND
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## IND
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## IND
	 * subslot number
	 */

	/***/

	LSA_UINT8 appl_ready_follows;
	/**< ## IND
	 * CM_SV_APPL_READY_FOLLOWS_NO
	 * ## RSP
	 * see usecase in text
	 */

	LSA_UINT8 appl_ready_pending;
	/**< ## IND
	 * CM_SV_APPL_READY_PENDING_YES
	 * ## RSP
	 * see usecase in text
	 */

} CM_SV_AR_PRM_END_ELEMENT_TYPE; /**< The element type used for CM_OPC_SV_AR_PRM_END_IND/_RSP. */

typedef CM_SV_AR_PRM_END_ELEMENT_TYPE * CM_UPPER_SV_AR_PRM_END_ELEMENT_PTR_TYPE; /**< The element pointer type used for CM_OPC_SV_AR_PRM_END_IND/_RSP. */


typedef struct cm_sv_ar_prm_end_tag {

	LSA_UINT16 ar_context; /* see CM_SV_AR_CONTEXT_CONN, ... */

	LSA_UINT16 nr_of_elems;
	/**< ## IND
	 * Number of elements contained in the array "elem" (0..n).
	 */

	CM_SV_AR_PRM_END_ELEMENT_TYPE elem[1];
	/**< ## IND
	 * elem[2..n] follow here
	 */

} CM_SV_AR_PRM_END_TYPE;

typedef CM_SV_AR_PRM_END_TYPE * CM_UPPER_SV_AR_PRM_END_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_PRM_END_IND/_RSP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 138/139 CM_OPC_SV_AR_IN_DATA_IND/_RSP                                     */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_IN_DATA_IND_RSP 138/139 CM_OPC_SV_AR_IN_DATA_IND/_RSP
@ingroup cm-sv-opcodes
@{
@brief The indication CM_OPC_SV_AR_IN_DATA_IND signals the transition into data exchange. The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

This indication does not imply that the IOPS of the outputs are GOOD.

This indication is the trigger to start updating the outputs. Note that the IOPS may be still BAD.

If at the end of the connect phase all submodules are e.g. "pulled", an empty indication (nr_of_elems == 0) will be signaled.
In other words: There is always an indication at the end of the connect phase.

@todo CM documentation: Bild: wann Update-Inputs, wann Update-Outputs, bei Class1/2/3...

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. a remove-submodule.
The user has to handle the "late" indication gracefully.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/
typedef struct cm_sv_ar_in_data_element_tag {

	LSA_UINT32 api;
	/**< ## IND
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## IND
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## IND
	 * subslot number
	 */

} CM_SV_AR_IN_DATA_ELEMENT_TYPE; /**< The element type used for CM_OPC_SV_AR_IN_DATA_IND/_RSP. */

typedef CM_SV_AR_IN_DATA_ELEMENT_TYPE * CM_UPPER_SV_AR_IN_DATA_ELEMENT_PTR_TYPE; /**< The element pointer type used for CM_OPC_SV_AR_IN_DATA_IND/_RSP. */


typedef struct cm_sv_ar_in_data_tag {

	LSA_UINT16 nr_of_elems;
	/**< ## IND
	 * Number of elements contained in the array "elem" (0 or 1..n - see text).
	 */

	CM_SV_AR_IN_DATA_ELEMENT_TYPE elem[1];
	/**< ## IND
	 * elem[2..n] follow here
	 */

} CM_SV_AR_IN_DATA_TYPE;

typedef CM_SV_AR_IN_DATA_TYPE * CM_UPPER_SV_AR_IN_DATA_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_IN_DATA_IND/_RSP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 140/141 CM_OPC_SV_AR_RIR_IND/_RSP                                         */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_RIR_IND_RSP 140/141 CM_OPC_SV_AR_RIR_IND/_RSP
@ingroup cm-sv-opcodes
@{
@brief The indication CM_OPC_SV_AR_RIR_IND signals that the output-CRs (consumers) are received in the "red" phase of the communication cycle (refer to [/11/](@ref cm-ref-no-11), Figure "Definition of the reserved interval"). The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

This indication applies to ARs with ARType = CM_AR_TYPE_SINGLE_RTC3 only.

The IOCRs in the array elem[] have to be included in the IO-Update.

This indication does not imply that the IOPS of the outputs are GOOD.

If the synchronization of the application takes a while, this indication has to be responded immediately without changing "rqb.response" and completion has to be reported later with the request CM_OPC_SV_AR_RIR_ACK (AP00975634).

@verbatim
RiR.ind ->
<- RiR.rsp
...
Synchronize Application
...
Update Inputs / IOPS
...
<- RiR_ACK.req
RiR_ACK.cnf ->
@endverbatim

If there is no need for a synchronization of the application then the "received in red" can be acknowledged immediately by setting "rqb.response" to CM_OK.

@verbatim
RiR.ind ->
...
Update Inputs / IOPS
...
<- RiR.rsp(CM_OK)
@endverbatim

## With V4.0i80.14 the RTClass3 startup has changed (due to AP00738250)

 - The inputs and outputs must not be updated before RIR.IND.
 - The inputs have to be updated before RIR.RSP(OK) or RIR_ACK.
 - This indication too is the trigger to start updating the outputs. Note that the IOPS may be still BAD.
 - The RIR_ACK.REQ can be delayed infinitely.

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. an AR-abort.
The user has to handle the "late" indication gracefully.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_RESOURCE    |                               |
******************************************************************************/
typedef struct cm_sv_ar_rir_element_tag {

	LSA_USER_ID_TYPE iocr_user_id;
	/**< ## IND
	 * see @ref CM_OPC_SV_AR_CONNECT_IND
	 */

} CM_SV_AR_RIR_ELEMENT_TYPE; /**< The element type used for CM_OPC_SV_AR_RIR_IND/_RSP. */

typedef CM_SV_AR_RIR_ELEMENT_TYPE * CM_UPPER_SV_AR_RIR_ELEMENT_PTR_TYPE; /**< The element type used for CM_OPC_SV_AR_RIR_IND/_RSP. */


typedef struct cm_sv_ar_rir_tag {

	CM_RESERVED_INTERVAL_IF_RED_TYPE rsv_interval_red;
	/**< ## IND
	 * Timing parameters for the IO update (V5.2i4.29, AP00964333 and AP01317720). \n
	 * For the meaning of the values, please refer to the EDD documentation / EDD_RESERVED_INTERVAL_IF_RED_TYPE. \n
	 * @note The timing can be optimized by inserting the record PDIRApplicationData (index 0x00010002, see EDD documentation) into the PDEV parameterization of the interface submodule using the opcode CM_OPC_SV_AR_PDEVPRM_WRITE.
	 */

	LSA_UINT16 nr_of_elems;
	/**< ## IND
	 * Number of elements contained in the array "elem" (2..n). \n
	 * History: Up to V7.0i1.31 "nr_of_elems" was always 2 and elem[] was a fixed size array. With V7.0i1.32 (feature n-IOCR, TIA 1684352) elem[] is a variable size array.
	 */

	CM_SV_AR_RIR_ELEMENT_TYPE elem[1];
	/**< ## IND
	 * elem[2..n] follow here
	 */

} CM_SV_AR_RIR_TYPE;

typedef CM_SV_AR_RIR_TYPE * CM_UPPER_SV_AR_RIR_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_RIR_IND/_RSP. */

/** @} ***********************************************************************/


/*****************************************************************************/
/* 142/143 CM_OPC_SV_AR_READ_IND/_RSP                                        */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_READ_IND_RSP 142/143 CM_OPC_SV_AR_READ_IND/_RSP
@ingroup cm-sv-opcodes
@brief The indication CM_OPC_SV_AR_READ_IND signals a read request. The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

@see @ref CM_SV_RECORD_TYPE
@param CM_SV_RECORD_TYPE::api                            **IND** Application process identifier
@param CM_SV_RECORD_TYPE::slot_nr                        **IND** Slot number
@param CM_SV_RECORD_TYPE::subslot_nr                     **IND** Subslot number
@param CM_SV_RECORD_TYPE::record_index                   **IND** Record index
@param CM_SV_RECORD_TYPE::sequence_nr                    **IND** Sequence number (0, 1, 2, ... 65535, 0, 1, 2, ...). \n
																 The Sequence number is common for read and write, see too flag "via_CombinedObjectContainer"! \n
																 This is the sequence number from the IODReadReqHeader (refer to [/11/](@ref cm-ref-no-11)). \n
																 Some indices are handled internally by CM-SV, so the user sees gaps. Because of the RPC property "idempotent" the same RPC may be executed multiple times (idempotent rerun).
@param CM_SV_RECORD_TYPE::via_CombinedObjectContainer    Don't evaluate
@param CM_SV_RECORD_TYPE::data_length                    **IND** CM_RECORD_OFFSET + number of bytes to be read.\n **RSP** CM_RECORD_OFFSET + number of read bytes.\n
																 If cm_pnio_err != CM_PNIO_ERR_NONE, the value CM_RECORD_OFFSET has to be returned.
@param CM_SV_RECORD_TYPE::data                           **IND** Pointer to the data buffer. \n **RSP** The data must be entered stating at offset CM_RECORD_OFFSET.
@param CM_SV_RECORD_TYPE::cm_pnio_err                    **RSP** See text below, cm_err.h and cm_usr.h. Epecially CM_RECORD_READ_NOT_HANDLED and CM_RECORD_READ_NO_IODATA.
@param CM_SV_RECORD_TYPE::ret_val_1                      **RSP** Return value 1, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
@param CM_SV_RECORD_TYPE::ret_val_2                      **RSP** Return value 2, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
@param CM_SV_RECORD_TYPE::readquery_length               **IND** Length of the 'RecordDataReadQuery' block (refer to [/11/](@ref cm-ref-no-11)). \n
																 This field has to be evaluated if CM_SV_CREATE_TYPE::can_Record-DataReadQuery was set to != 0. \n
																 == 0: no block. \n !=0: the block starts at offset CM_RECORD_OFFSET.
@param CM_SV_RECORD_TYPE::is_iosar_with_device_access    Don't evaluate.
@param CM_SV_RECORD_TYPE::target_session_key             **IND** Session key of the target AR if implicit read (else 0)

The data buffer was allocated by CLRPC. It is returned to CLRPC in the course of the response.

Some indices are handled by CM-SV and are not forwarded to the user. See chapter @ref cm-list-of-records.

## Special case "session_key == 0" (implicit AR, implicit-read)

If "session_key == 0" this means that the read request came in over the "implicit AR".

The "implicit AR" is an access point that needs no connection establishment - it is connected implicitly. Refer to [/10/](@ref cm-ref-no-10), chapter "Implicit application relationship".

The "implicit AR" runs in parallel with the "real time ARs" and "device access ARs".

Implicit-read requests are serialized (per device) by CM-SV. The next request is indicated to the user only after the response of the current one is back in CM-SV.

Implicit-read requests use the "target_session_key" to address a specific AR, e.g. for reading RS_GetEvent (0xE060).

## Note on reading the indices 0x8028 (RecordInputDataObjectElement) and 0x8029 (RecordOutput-DataObjectElement)

The following must be observed according to [/10/](@ref cm-ref-no-10) and AP00383155:

For input submodules (even without user data), the "PNIOStatus = 0xDE, 0x80, 0xB0, USER" is to be returned for a read output.

For output submodules, the "PNIOStatus = 0xDE, 0x80, 0xB0, USER" is to be returned for a read input.

Refer to CM_RECORD_READ_NO_IODATA in cm_usr.h.

## RecordDataReadQuery

Can be enabled by setting CM_SV_CREATE_TYPE::can_RecordDataReadQuery to CM_SV_RECORD_DATA_READ_QUERY_YES.

If enabled, the field "readquery_length" has to be evaluated and if != 0, the block has to be handled.

## Special case: reading of (cm-internally handled) diagnosis indices with record_data_length == 0

To test if diagnosis data is present, the caller can try a read with a zero buffer length. If the service is confirmed with PNIOStatus == CM_PNIO_ERR_NONE this means that no diagnosis is present.

## Special case: ARFSUDataAdjust (index 0xE050)

Starting with CM V4.0i40 a read ARFSUDataAdjust is handled internally and this index is no longer indicated to the user. If the record was not written then 0 bytes + Ok is returned.

See also chapter @ref CM_OPC_SV_CREATE (parameter can_ARFSUDataAdjust).

## Special case: I&M0 (index 0xAFF0)

If there is no I&M0 information for the addressed submodule then an empty record has to be returned (i.e., data_length == CM_RECORD_OFFSET and cm_pnio_err == CM_PNIO_ERR_NONE).

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. a remove-submodule.
The user has to handle the "late" indication gracefully.

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-read. \n
NAP-Type R1: The things said for S2 apply to R1 too.

## System Redundancy ARs and aborted parameterization

Due to the asynchronous interface the user may see a read indication on an AR that is no longer the primary or "first" AR.

See too chapter @ref cm-server-channels-sysred.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/


/*****************************************************************************/
/* 144/145 CM_OPC_SV_AR_WRITE_IND/_RSP                                       */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_WRITE_IND_RSP 144/145 CM_OPC_SV_AR_WRITE_IND/_RSP
@ingroup cm-sv-opcodes
@brief The indication CM_OPC_SV_AR_WRITE_IND signals a write request. The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

@see @ref CM_SV_RECORD_TYPE
@param CM_SV_RECORD_TYPE::api							**IND** Application process identifier
@param CM_SV_RECORD_TYPE::slot_nr						**IND** Slot number
@param CM_SV_RECORD_TYPE::subslot_nr					**IND** Subslot number
@param CM_SV_RECORD_TYPE::record_index					**IND** Record index
@param CM_SV_RECORD_TYPE::sequence_nr					**IND** Sequence number (0, 1, 2, ... 65535, 0, 1, 2, ...). \n
																The Sequence number is common for read and write, see too flag "via_CombinedObjectContainer"! \n
																This is the sequence number from the IODReadReqHeader (refer to [/11/](@ref cm-ref-no-11)). \n
																Some indices are handled internally by CM-SV, so the user sees gaps. Because of the RPC property "idempotent" the same RPC may be executed multiple times (idempotent rerun).\n
																If via_CombinedObjectContainer == CM_VIA_COC_YES the sequence number is the number of the container record 0x80B0 (i.e., identical for all contained records).
@param CM_SV_RECORD_TYPE::via_CombinedObjectContainer	**IND** CM_VIA_COC_YES .. record is part of a CombinedObjectContainer \n 0 .. otherwise \n
																If a record is part of a CombinedObjectContainer it does not have its own sequence number, see too "sequence_nr" above. \n
																An AR with ARProperties.CombinedObjectContainer=1 packs all records of the group-members (e.g. the PDEV submodules) into a Combined-ObjectContainer.
																If any record of the container is responded negatively, the parameterization is considered invalid. A default parameterization will take effect and a diagnosis will be set.
																CM_VIA_COC_NO ... otherwise
@param CM_SV_RECORD_TYPE::data_length					**IND** CM_RECORD_OFFSET + number of bytes to write. \n **RSP** CM_RECORD_OFFSET + number of bytes written.\n
															If cm_pnio_err != CM_PNIO_ERR_NONE, the value CM_RECORD_OFFSET has to be returned.
@param CM_SV_RECORD_TYPE::data							**IND** Pointer to the data buffer. The data starts at offset CM_RECORD_OFFSET. \n **RSP** unchanged
@param CM_SV_RECORD_TYPE::cm_pnio_err					**RSP** See text below, cm_err.h and cm_usr.h.Especially CM_RECORD_WRITE_NOT_HANDLED.
@param CM_SV_RECORD_TYPE::ret_val_1						**RSP** Return value 1, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
@param CM_SV_RECORD_TYPE::ret_val_2						**RSP** Return value 2, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
@param CM_SV_RECORD_TYPE::readquery_length				Don't evaluate.
@param CM_SV_RECORD_TYPE::is_iosar_with_device_access	**IND** CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_YES .. if IOSAR with ARProperties.DeviceAccess:=1. \n CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_NO .... otherwise. \n
																See chapter @ref cm-server-channels-easy-supervisor. \n
																It is the application's responsibility to check if it is ok to execute the write.
@param CM_SV_RECORD_TYPE::target_session_key			Don't care.

The data buffer was allocated by CLRPC. It is returned to CLRPC in the course of the response.

## Special case ARFSUDataAdjust (index 0xE050)

Starting with CM V4.0i40 a write ARFSUDataAdjust is preprocessed internally as follows. Whether the record is indicated to the user or not, can be controlled by the parameter can_ARFSUDataAdjust of CreateServer (see chapter @ref CM_OPC_SV_CREATE).

@code{.c}
//On write.IND:
//remove stored record if was written before
if( block structure is wrong ) {
	write.RSP(-, PNIORW / Invalid Type) // same as PDEV would return on ERR_PRM_DATA
}
else if( could not store the record in the AR ) {
	write.RSP(-, PNIO / RMPM / Out of Memory)
}
else { // note: record is stored in the AR for subsequent read request.
	if( ! can_ARFSUDataAdjust ) {
		write.RSP(+)
	}
	else {
		indicate to user
		wait for response
		if( cm_pnio_err != CM_PNIO_ERR_NONE ) {
			remove stored record
		}
		write.RSP(cm_pnio_err)
	}
}
@endcode

## Write-Multiple

"Write-Multiple" means that the controller can transfer more than one record within a single RPC request. "Write-Multiple" is not visible to the user on the server side.
The sequence number in the RPC PDU is incremented for the header record 0xE040 as well as for each embedded record.
This leads to sequence number gaps on the bus that were not there before the feature "write-multiple" was introduced.

## Write-Multiple-Parallelism

This feature is enabled by setting the parameter "write_multiple_parallelism" of CM_OPC_SV_DEVICE_ADD to a convenient value (> 1).

If for example a write-multiple record contains 10 records and write_multiple_parallelism is set to 3 then CM indicates 3 records to the user and then waits.
As soon as a write indication is responded, another record is indicated. The write indications may be responded out of order.

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. a remove-submodule.
The user has to handle the "late" indication gracefully.

System Redundancy ARs:

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-write. \n
NAP-Type R1: The things said for S2 apply to R1 too.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/


/*****************************************************************************/
/* 146/147 CM_OPC_SV_AR_DATA_STATUS_IND/_RSP                                 */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_DATA_STATUS_IND_RSP 146/147 CM_OPC_SV_AR_DATA_STATUS_IND/_RSP
@ingroup cm-sv-opcodes
@{
@brief The indication CM_OPC_SV_AR_DATA_STATUS_IND signals a change in the data-status (of the APDUStatus) of the output-CRs (consumers). The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

The indication has to be enabled by "enable_DataStatusIndication" in @ref CM_OPC_SV_CREATE.

The initial value after connect is CM_DATA_STATUS_INIT (run, ok, primary).

 - The status is "run" if all output-CRs are in "run".
 - The status is "stop" if any output-CR is in "stop".
 - The status is "Station Ok" if all output-CRs are in "Station Ok".
 - The status is "Station Problem" if any output-CR is in "Station Problem".
 - The status is "primary" if all output-CRs are in "primary".
 - The status is "backup" if any output-CR is in "backup".
 - The status is "all backup" if all output-CRs are in "backup".

@note The indication may occur before InData.IND. This means that with InData.IND the current data-status of the output-CRs is known to the user.

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. an AR-abort.
The user has to handle the "late" indication gracefully.

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-data-status. \n
NAP-Type R1: The things said for S2 apply to R1 too.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/
#define CM_DATA_STATUS_INIT       (CM_DATA_STATUS_PRIMARY | CM_DATA_STATUS_RUN | CM_DATA_STATUS_OK) /**< initial value */
#define CM_DATA_STATUS_INIT_SYSRED (0/*backup*/ | CM_DATA_STATUS_RUN | CM_DATA_STATUS_OK) /**< initial value for CM_AR_TYPE_SINGLE_SYSRED */

#define CM_DATA_STATUS_PRIMARY    EDD_CSRT_DSTAT_BIT_STATE      /**< 0=backup 1=primary */
#define CM_DATA_STATUS_REDUNDANCY EDD_CSRT_DSTAT_BIT_REDUNDANCY /**< do not evaluate */
#define CM_DATA_STATUS_BIT2       0x04                          /**< always reported as 0 */
#define CM_DATA_STATUS_BIT3       0x08                          /**< always reported as 0 */
#define CM_DATA_STATUS_RUN        EDD_CSRT_DSTAT_BIT_STOP_RUN   /**< 0=stop 1=run */
#define CM_DATA_STATUS_OK         EDD_CSRT_DSTAT_BIT_STATION_FAILURE /**< 0=failure 1=ok */
#define CM_DATA_STATUS_BIT6       0x40                          /**< always reported as 0 */
#define CM_DATA_STATUS_BIT7       0x80                          /**< always reported as 0 */
#define CM_DATA_STATUS_ALL_BACKUP 0x8000

typedef struct cm_sv_ar_data_status_tag {

	LSA_UINT16 curr; /* ind: current value, see CM_DATA_STATUS_PRIMARY, ... */
	/**< ## IND
	 * current value, see CM_DATA_STATUS_PRIMARY, etc.
	 * @note The changed bits can be calculated by (curr ^ prev).
	 */

	LSA_UINT16 prev; /* ind: previous value */
	/**< ## IND
	 * previous value
	 */

} CM_SV_AR_DATA_STATUS_TYPE;

typedef CM_SV_AR_DATA_STATUS_TYPE * CM_UPPER_SV_AR_DATA_STATUS_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_DATA_STATUS_IND/_RSP. */
/** @} ***********************************************************************/


#if CM_CFG_SV_AR_HAS_MCONS || CM_CFG_EVERYTHING_VISIBLE
/*****************************************************************************/
/* 148/149 CM_OPC_SV_AR_MCONS_IND/_RSP                                       */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_MCONS_IND_RSP 148/149 CM_OPC_SV_AR_MCONS_IND/_RSP
@ingroup cm-sv-opcodes
@{
@brief This indication CM_OPC_SV_AR_MCONS_IND is used to signal the state-changes of a multicast consumer (m-consumer). The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

CM-SV creates the diagnosis entry and the MCR mismatch alarm as a function of the "up / down" messages of the M-consumer.

@note M-consumers are currently not supported (due to AP00774501). This chapter is preliminary information only.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/
#define CM_SV_MCONS_EVENT_ADDR_INFO    1
#define CM_SV_MCONS_EVENT_IN_DATA      2
#define CM_SV_MCONS_EVENT_OFFLINE      3
#define CM_SV_MCONS_EVENT_DATA_STATUS  4

typedef struct cm_sv_ar_mcons_tag {

	LSA_UINT16 cr_ref; /**< cr-reference */

	LSA_UINT16 mc_event; /**< see CM_SV_MCONS_EVENT_ADDR_INFO, ... */

	union {

		struct {

			LSA_UINT16 info_class; /**< CM_ADDR_INFO_CLASS_NARE, CM_ADDR_INFO_CLASS_MISS or CM_ADDR_INFO_CLASS_OK */
			LSA_UINT16 nare_rsp;   /**< see NARE_RSP_xxx */

			CM_MAC_TYPE source_mac; /**< ethernet mac-addr or error-mac */
			LSA_UINT32  source_ip;  /**< ip-addr or error-ip */

		} addr_info;

		struct {

			LSA_UINT8 nothing_so_far;

		} in_data;

		struct {

			LSA_UINT8 nothing_so_far;

		} offline;

		struct {

			LSA_UINT8 curr;
			/**< ## IND
			 * current value, see CM_DATA_STATUS_PRIMARY, ...
			 */

			LSA_UINT8 prev;
			/**< ## IND
			 * previous value
			 */

		} data_status;

	} u;

} CM_SV_AR_MCONS_TYPE;

typedef CM_SV_AR_MCONS_TYPE * CM_UPPER_SV_AR_MCONS_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_MCONS_IND/_RSP. */
/** @} ***********************************************************************/

#endif


/*****************************************************************************/
/* CM_SV_RECORD_TYPE                                                         */
/**************************************************************************//**
@defgroup cm-sv-records CM SV Records
@ingroup cm-records
@{
@brief CM_SV_RECORD_TYPE is used for @ref CM_OPC_SV_AR_READ_IND_RSP and @ref CM_OPC_SV_AR_WRITE_IND_RSP.
******************************************************************************/
#if CM_RECORD_HEADER_SIZE != (64U) || CM_RECORD_OFFSET != (CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE)
#error "check this! see definition above"
#endif

#define CM_VIA_COC_NO  0
#define CM_VIA_COC_YES 1

typedef struct cm_sv_record_tag {

	LSA_UINT32 api;
	/**< application process identifier */

	LSA_UINT16 slot_nr;
	/**< slot number */

	LSA_UINT16 subslot_nr;
	/**< subslot number */

	LSA_UINT16 record_index;
	/**< record index */

	LSA_UINT16 sequence_nr;
	/**< ## IND
	 * sequencing (common for read and write!), see too flag "via_CombinedObjectContainer"
	 */

	LSA_UINT8 via_CombinedObjectContainer;
	/**< ## Read-IND
	 * don't evaluate
	 * ## Write-Ind
	 * CM_VIA_COC_YES .. the record is part of a CombinedObjectContainer
	 * @note Important: in this case "sequence_nr" is the number of the container record 0x80B0 (i.e., identical for all contained records)
	 */

	LSA_UINT32 data_length;
	/**< ## Read-IND
	 * size of 'data' (including CM_RECORD_OFFSET)
	 * ## Read-RSP
	 * CM_RECORD_OFFSET + number of bytes read
	 * ## Write-IND
	 * CM_RECORD_OFFSET + number of bytes to write
	 * ## Write-RSP
	 * CM_RECORD_OFFSET + number of bytes written
	 */

	CM_COMMON_MEM_U8_PTR_TYPE data;
	/**< The CLRPC buffer, user-data starts at offset CM_RECORD_OFFSET. The headers are not hidden to "see" the alignment of the user-data.
	 * ## Read-IND
	 * "empty" buffer
	 * ## Read-RSP
	 * read data stored at offset CM_RECORD_OFFSET
	 * ## Write-IND
	 * data-to-write at offset CM_RECORD_OFFSET
	 * ## Write-RSP
	 * "unchanged"
	 */

	LSA_UINT32 cm_pnio_err;
	/**< ## RSP
	 * see the macro CM_PNIO_ERR_MAKE() in file cm_err.h
	 *
	 * For indices not handled by the CM-user the error codes defined by the following macros shall be returned:
	 * - CM_RECORD_READ_NOT_HANDLED(code2)
	 * - CM_RECORD_WRITE_NOT_HANDLED(code2)
	 *
	 * The value of the 'code2' is user-specific.
	 */

	LSA_UINT16 ret_val_1;
	/**< ## RSP
	 * return value 1
	 */

	LSA_UINT16 ret_val_2;
	/**< ## RSP
	 * return value 2
	 */

	LSA_UINT32 readquery_length;
	/**< ## Read-IND
	 * size of 'RecordDataReadQuery' (see PNIO-Spec) at offset CM_RECORD_OFFSET
	 * ## Write-IND
	 * don't evaluate
	 */

	LSA_UINT8 is_iosar_with_device_access;
	/**< ## Read-IND
	 * don't evaluate
	 * ## Write-IND
	 * CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_YES .. the AR is an CM_AR_TYPE_SUPERVISOR with CM_AR_PROP_DEVICE_ACCESS_YES
	 * CM_SV_IS_IOSAR_WITH_DEVICE_ACCESS_NO ... otherwise
	 */

	LSA_UINT16 target_session_key;
	/**< ## Read-IND
	 * session key of the target AR if implicit read (else 0)
	 * ## Write-IND
	 * don't evaluate
	 */

} CM_SV_RECORD_TYPE;

typedef CM_SV_RECORD_TYPE * CM_UPPER_SV_RECORD_PTR_TYPE; /**< The pointer type used for @ref CM_OPC_SV_AR_READ_IND_RSP and @ref CM_OPC_SV_AR_WRITE_IND_RSP */
/** @} ***********************************************************************/

/*-----------------------------------------------------------------------------
// end of server events
//---------------------------------------------------------------------------*/
#endif


/*****************************************************************************/
/* 051/052 CM_OPC_CL_AR_DATA_STATUS_IND/_RSP                                 */
/**************************************************************************//**
@defgroup CM_OPC_CL_AR_DATA_STATUS_IND_RSP 051/052 CM_OPC_CL_AR_DATA_STATUS_IND/_RSP
@ingroup cm-cl-opcodes
@{
@brief Changes in the APDU status of the input-CRs are signaled with the help of this indication.
@details
@see @ref CM_EVENT_TYPE

The AR is in the status "run" if all CRs are in "run". \n
The AR is in the status "stop" if any CR is in "stop". \n
The AR is in the status "Station Ok" if all CRs are in "Station Ok". \n
The AR is in the status "Station Problem" if any CR is in "Station Problem". \n
The AR is in the status "primary" if all CRs are in "primary". \n
The AR is in the status "backup" if all CRs are in "backup" (“any backup” is not indicated). \n
The AR is in the status "Sysred Ok" if all CRs are in "primary exists". \n
The AR is in the status "Sysred Problem" if any CR is in "no primary exists".

The initial APDU status is CM_EVENT_APDU_STATUS_INITIAL_VALUE.

The initial APDU status for an AR with ARType SYSRED is CM_EVENT_APDU_STATUS_INITIAL_SYSRED_VALUE.

The opcode requires a response. Refer to Request-block circulations, chapter @ref cm-product-architecture-programming-model-general-request-block-circulations.

For the parameter 'data_status' of this opcode, please refer to @ref CM_EVENT_TYPE.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | Success
******************************************************************************/
enum cm_event_apdu_status_enum {
	CM_EVENT_APDU_STATUS_STOP            = 0x0001,
	CM_EVENT_APDU_STATUS_RUN             = 0x0002,
	CM_EVENT_APDU_STATUS_STATION_OK      = 0x0004,
	CM_EVENT_APDU_STATUS_STATION_PROBLEM = 0x0008,
	CM_EVENT_APDU_STATUS_PRIMARY         = 0x0010,
	CM_EVENT_APDU_STATUS_BACKUP          = 0x0020,
	CM_EVENT_APDU_STATUS_SYSRED_OK       = 0x0040,	/* the sysred "ar-set" has a primary ar */
	CM_EVENT_APDU_STATUS_SYSRED_PROBLEM  = 0x0080,
	/***/
	CM_EVENT_APDU_STATUS_BACKUP_HELPER   = 0x8000,	/* internally used only */
	/***/
	CM_EVENT_APDU_STATUS_INITIAL_VALUE        = (CM_EVENT_APDU_STATUS_RUN | CM_EVENT_APDU_STATUS_STATION_OK | CM_EVENT_APDU_STATUS_PRIMARY | CM_EVENT_APDU_STATUS_SYSRED_OK),
	CM_EVENT_APDU_STATUS_INITIAL_SYSRED_VALUE = (CM_EVENT_APDU_STATUS_RUN | CM_EVENT_APDU_STATUS_STATION_OK | CM_EVENT_APDU_STATUS_BACKUP  | CM_EVENT_APDU_STATUS_SYSRED_OK)
};
/** @} ***********************************************************************/


/******************************************************************************
OPCODE:     CM_OPC_PD_PROVIDE_EVENT
INDICATION: CM_OPC_PD_EVENT_*_IND
RESPONSE:   CM_OPC_PD_EVENT_*_RSP

OPCODE:     CM_OPC_CL_DEVICE_PROVIDE_EVENT
INDICATION: CM_OPC_CL_AR_*_IND
RESPONSE:   CM_OPC_CL_AR_*_RSP

OPCODE:     CM_OPC_SV_DEVICE_PROVIDE_EVENT
INDICATION: CM_OPC_SV_AR_*_IND
RESPONSE:   CM_OPC_SV_AR_*_RSP
******************************************************************************/

/*****************************************************************************/
/* 040 CM_OPC_CL_DEVICE_PROVIDE_EVENT                                        */
/**************************************************************************//**
@defgroup CM_OPC_CL_DEVICE_PROVIDE_EVENT 040 CM_OPC_CL_DEVICE_PROVIDE_EVENT
@ingroup cm-cl-opcodes
@brief The EventProvide function is used to provide one event indication resource to a proper device.
@details
@see @ref CM_EVENT_TYPE
@param CM_EVENT_TYPE::device_nr **REQ** Assign this event-resource to this device
@note Rest: don't care!

The "events" signal AR-specific status changes.

Indications change the opcode of the RQB as follows:
- [CM_OPC_CL_AR_ABORT_IND](@ref CM_OPC_CL_AR_ABORT_IND_RSP)
- [CM_OPC_CL_AR_PRE_DATA_IND](@ref CM_OPC_CL_AR_PRE_DATA_IND_RSP)
- [CM_OPC_CL_AR_PRE_OFFLINE_IND](@ref CM_OPC_CL_AR_PRE_OFFLINE_IND_RSP)
- [CM_OPC_CL_AR_IN_DATA_IND](@ref CM_OPC_CL_AR_IN_DATA_IND_RSP)
- [CM_OPC_CL_AR_OFFLINE_IND](@ref CM_OPC_CL_AR_OFFLINE_IND_RSP)
- [CM_OPC_CL_AR_DATA_STATUS_IND](@ref CM_OPC_CL_AR_DATA_STATUS_IND_RSP)

Each indication must be acknowledged with the corresponding response CM_OPC_xxx_RSP.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK_PARAM            | Device doesn't exist.
CM_OK_CANCELLED        | The RQB is to be released. Refer to RemoveDevice.
******************************************************************************/


/*****************************************************************************/
/* 074 CM_OPC_MC_DEVICE_PROVIDE_EVENT                                        */
/**************************************************************************//**
@defgroup CM_OPC_MC_DEVICE_PROVIDE_EVENT 074 CM_OPC_MC_DEVICE_PROVIDE_EVENT
@ingroup cm-mc-opcodes
@brief The EventProvide function is used to provide an event indication resource to a device.
@details
The number of resources that must be provided is a return value of [CM_OPC_MC_DEVICE_ADD](@ref CM_OPC_MC_DEVICE_ADD).

The "events" signal status changes.

Indications change the opcode of the RQB as follows:

 - [CM_OPC_MC_EVENT_AR_ADDR_INFO_IND](@ref CM_OPC_MC_EVENT_AR_ADDR_INFO_IND_RSP)
 - [CM_OPC_MC_EVENT_AR_IN_DATA_IND](@ref CM_OPC_MC_EVENT_AR_IN_DATA_IND_RSP)
 - [CM_OPC_MC_EVENT_AR_OFFLINE_IND](@ref CM_OPC_MC_EVENT_AR_OFFLINE_IND_RSP)
 - [CM_OPC_MC_EVENT_APDU_STATUS_IND](@ref CM_OPC_MC_EVENT_APDU_STATUS_IND_RSP)

Each indication must be acknowledged with the corresponding response CM_OPC_xxx_RSP.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK_PARAM            | Device doesn't exist.
CM_OK_CANCELLED        | The RQB is to be released. Refer to RemoveDevice.
******************************************************************************/


/*****************************************************************************/
/* 107 CM_OPC_SV_DEVICE_PROVIDE_EVENT                                        */
/**************************************************************************//**
@defgroup CM_OPC_SV_DEVICE_PROVIDE_EVENT 107 CM_OPC_SV_DEVICE_PROVIDE_EVENT
@ingroup cm-sv-opcodes
@brief The request CM_OPC_SV_DEVICE_PROVIDE_EVENT provides an event indication-resource to the device.
@details
@see @ref CM_EVENT_TYPE
@param CM_EVENT_TYPE::device_nr **REQ** Device number
@note Rest: don't care!

The user has to provide "nr_of_event_resources" as returned from @ref CM_OPC_SV_DEVICE_ADD.

The request is cancelled, when the device is removed. The request is returned to the user with CM_OK_CANCELLED, before the device-remove request is confirmed.

@note For some indications the RQB contains only pointers (e.g., ar_connect). The additional memory for these indications is managed by CM-SV.

@todo MM CM:SPLIT EVENT TAG!!!

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK_CANCELLED        | Out-parameters are invalid.
CM_ERR_SEQUENCE        | CM-SV is not created. Device is not added.
******************************************************************************/


/*****************************************************************************/
/* CM_EVENT_TYPE                                                             */
/**************************************************************************//**
@defgroup CM_EVENT_TYPE CM_EVENT_TYPE
@ingroup cm-rqbs-group
@{
@brief The CM_EVENT_TYPE forms the arguments ('args') of an event indication.
@details
The individual fields are explained in the corresponding event indication opcode.

These codes are used by the CM client and the CM server.

Used for CM_OPC_PD_PROVIDE_EVENT, CM_OPC_PD_EVENT_*_IND/_RSP, CM_OPC_CL_DEVICE_PROVIDE_EVENT, CM_OPC_CL_AR_*_IND/_RSP, CM_OPC_SV_DEVICE_PROVIDE_EVENT and CM_OPC_SV_AR_*_IND/_RSP.

## Parallel running of events

### CM client and CM multicast

Different ARs are independent of each other and the run (quasi) simultaneously.
The number of independent event-resources is given by DeviceAdd.cnf, see @ref CM_OPC_CL_DEVICE_ADD.
The number of independent alarm-resources is given by DeviceAdd.cnf, see @ref CM_OPC_CL_DEVICE_ADD.

### CM server

See @ref cm-sv-parallelism.
******************************************************************************/
typedef struct cm_event_tag {

	LSA_UINT16 device_nr;
	/**< ## CM client / CM server: IND
	 * Device Number
	 */

	LSA_UINT16 ar_nr;
	/**< ## CM client: IND
	 * AR Number
	 * ## CM server: IND
	 * = CM_SV_SESSION_KEY_TO_AR_NR(session_key)
	 */

	LSA_UINT16 session_key;
	/**< ## CM client: IND
	 * Initial session key or session key from last ArOffline.ind, see @ref CM_OPC_CL_AR_OFFLINE_IND
	 * ## CM server IND
	 * Session key, see @ref CM_OPC_SV_AR_CONNECT_IND
	 */

	union {

		union {

			CM_PD_APPL_READY_TYPE appl_ready; /**< see @ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP */

		} pd; /**< CM PD events */

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
		union {

			CM_CL_AR_ABORT_TYPE  ar_abort; /**< see @ref CM_OPC_CL_AR_PRE_OFFLINE_IND_RSP, @ref CM_OPC_CL_AR_ABORT_IND_RSP, @ref CM_OPC_CL_AR_OFFLINE_IND_RSP */

			CM_CL_AR_IN_DATA_TYPE in_data; /**< see @ref CM_OPC_CL_AR_PRE_DATA_IND_RSP and @ref CM_OPC_CL_AR_IN_DATA_IND_RSP */

			CM_CL_PRM_BEGIN_TYPE prm_begin; /**< see @ref CM_OPC_CL_AR_PRM_BEGIN_IND_RSP */

			CM_CL_PRM_INDATA_TYPE prm_indata; /**< see @ref CM_OPC_CL_AR_PRM_INDATA_IND_RSP */

			LSA_UINT16 data_status; /**< ind see cm_event_apdu_status_enum, @ref CM_OPC_CL_AR_DATA_STATUS_IND_RSP  */

		} cl; /**< CM CL events */
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
		union {

			CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_connect; /**< see @ref CM_OPC_SV_AR_CONNECT_IND_RSP */

			CM_UPPER_SV_AR_CONNECT_PTR_TYPE ar_disconnect; /**< see @ref CM_OPC_SV_AR_DISCONNECT_IND_RSP @note same type as ar_connect */

			CM_UPPER_SV_AR_OWNERSHIP_PTR_TYPE ar_ownership; /**< see @ref CM_OPC_SV_AR_OWNERSHIP_IND_RSP */

			LSA_UINT32 ar_pdevprm; /**< no arguments (placeholder), see @ref CM_OPC_SV_AR_PDEVPRM_IND_RSP */

			CM_UPPER_SV_AR_PRM_END_PTR_TYPE ar_prm_end; /**< see @ref CM_OPC_SV_AR_PRM_END_IND_RSP */

			CM_UPPER_SV_AR_IN_DATA_PTR_TYPE ar_in_data; /**< see @ref CM_OPC_SV_AR_IN_DATA_IND_RSP */

			CM_UPPER_SV_AR_RIR_PTR_TYPE ar_rir; /**< see @ref CM_OPC_SV_AR_RIR_IND_RSP */

			CM_SV_RECORD_TYPE ar_rec; /**< see definition, see @ref CM_OPC_SV_AR_READ_IND_RSP and @ref CM_OPC_SV_AR_WRITE_IND_RSP */

			CM_SV_AR_DATA_STATUS_TYPE ar_data_status; /**< see @ref CM_OPC_SV_AR_DATA_STATUS_IND_RSP */

#if CM_CFG_SV_AR_HAS_MCONS || CM_CFG_EVERYTHING_VISIBLE
			CM_SV_AR_MCONS_TYPE ar_mcons; /**< see @ref CM_OPC_SV_AR_MCONS_IND_RSP */
#endif
		} sv; /**< CM SV events */
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE
		union {

			CM_MC_ADDR_INFO_TYPE  addr_info; /**< see @ref CM_OPC_MC_EVENT_AR_ADDR_INFO_IND */

			CM_MC_IN_DATA_TYPE in_data; /**< see @ref CM_OPC_MC_EVENT_AR_IN_DATA_IND_RSP */

			CM_MC_OFFLINE_TYPE  ar_offline; /**< see @ref CM_OPC_MC_EVENT_AR_OFFLINE_IND_RSP */

			LSA_UINT16 apdu_status; /**< ind see cm_event_apdu_status_enum, see @ref CM_OPC_MC_EVENT_APDU_STATUS_IND_RSP */

		} mc; /**< CM MC events */
#endif

	} u; /**< pdev or client or server or multicast */

} CM_EVENT_TYPE;

typedef CM_EVENT_TYPE * CM_UPPER_EVENT_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_PROVIDE_EVENT, CM_OPC_PD_EVENT_*_IND/_RSP, CM_OPC_CL_DEVICE_PROVIDE_EVENT, CM_OPC_CL_AR_*_IND/_RSP, CM_OPC_SV_DEVICE_PROVIDE_EVENT and CM_OPC_SV_AR_*_IND/_RSP */
/** @}************************************************************************/


/*****************************************************************************/
/* 001 CM_OPC_OPEN_CHANNEL                                                   */
/**************************************************************************//**
@defgroup CM_OPC_OPEN_CHANNEL 001 CM_OPC_OPEN_CHANNEL
@ingroup cm-opcodes
@{
@brief Creates and initializes a new CM instance.
@details
A callback function is to be specified in the request block. All asynchronous functions are answered through this callback function.

Calls the output function CM_GET_PATH_INFO (chapter 2.4.1) to receive information about the configuration of the channel.

The CM_FATAL function is called if the RQB cannot be evaluated (e.g.: NULL pointer).

The CM_FATAL_ERROR output function is called if an error situation cannot be reported to the upper layer.

Refer to cm_open_channel().

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | The channel is opened.
CM_ERR_RESOURCE        | No free channel resource (Refer to CM_CFG_MAX_xxx in cm_cfg.h)
CM_ERR_SYS_PATH        | Invalid Sys-Path.
CM_ERR_ALLOC_LOWER     | No RQB could be allocated for opening the lower layer.
CM_ERR_LOWER_LAYER     | The channel of the lower layer could not be opened.
******************************************************************************/
#define CM_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /**< @note Do not check with \#if, CPP sees type-cast as an undefined symbol and assumes zero... */

typedef LSA_VOID  LSA_FCT_PTR(/*ATTR*/,
CM_UPPER_CALLBACK_FCT_PTR_TYPE)(
	CM_UPPER_RQB_PTR_TYPE rb
);

typedef struct cm_open_channel_tag {
	LSA_HANDLE_TYPE handle;
	/**< ## CNF
	 *CM channel handle of the created CM instance. This handle is to be specified in all further requests.
	 */

	LSA_HANDLE_TYPE handle_upper;
	/**< ## REQ
	 * channel-handle of user, every confirmation of a request sets this handle to rqb.handle
	 */

	LSA_SYS_PATH_TYPE sys_path;
	/**< ## REQ
	 * System path according to [/1/](@ref cm-ref-no-1).
	 */

	CM_UPPER_CALLBACK_FCT_PTR_TYPE cm_request_upper_done_ptr;
	/**< ## REQ
	 * Pointer on the callback function that is called as an answer to a request.
	 * This function is also called in an indication.
	 * The user is responsible for designating code sections, in which he may not be interrupted by CM,
	 * as uninterruptible or for taking other suitable measures.
	 */

} CM_OPEN_CHANNEL_TYPE;

typedef CM_OPEN_CHANNEL_TYPE * CM_UPPER_OPEN_CHANNEL_PTR_TYPE; /**< The pointer type used for CM_OPC_OPEN_CHANNEL. */

typedef CM_OPEN_CHANNEL_TYPE const * CM_UPPER_OPEN_CHANNEL_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_OPEN_CHANNEL. */
/** @}************************************************************************/


/*****************************************************************************/
/* 002 CM_OPC_CLOSE_CHANNEL                                                  */
/**************************************************************************//**
@defgroup CM_OPC_CLOSE_CHANNEL 002 CM_OPC_CLOSE_CHANNEL
@ingroup cm-opcodes
@{
@brief Closes an (opened) communication channel.
@details
Releases the local memory requested in cm_open_channel().

The function CM_FATAL is called if the RQB cannot be evaluated (e.g.: NULL pointer).

The output function CM_FATAL_ERROR is called if an error situation cannot be reported to the upper layer.

Refer also to chapter cm_close_channel().

@note No parameters.

## Result of the request

Return Code            | Meaning
-----------------------|--------------------------------
CM_OK                  | The channel is closed.
CM_ERR_SEQUENCE        | Devices still exist.
CM_ERR_ALLOC_LOWER     | No RQB could be allocated to close the lower layer.
CM_ERR_LOWER_LAYER     | The channel of the lower layer could not be closed.
******************************************************************************/
typedef LSA_VOID_PTR_TYPE * CM_UPPER_CLOSE_CHANNEL_PTR_TYPE; /**< The pointer type used for CM_OPC_CLOSE_CHANNEL. */
/** @}************************************************************************/


/*****************************************************************************/
/* XXX CM_OPC_EVENT_IND_RES_PROVIDE                                          */
/**************************************************************************//**
@defgroup CM_OPC_EVENT_IND_RES_PROVIDE XXX CM_OPC_EVENT_IND_RES_PROVIDE
@ingroup cm-opcodes
@brief Obsolete!
@details Opcode was renamed to @ref CM_OPC_PD_PROVIDE_EVENT with version V7.0i1.46 (TIA 1957290)
******************************************************************************/


/******************************************************************************
begin of physical device opcodes
******************************************************************************/

/*****************************************************************************/
/* 200 CM_OPC_PD_CREATE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_PD_CREATE 200 CM_OPC_PD_CREATE
@ingroup cm-pd-opcodes
@{
@brief PDCreate creates a CM-PD instance.
@details
The required size to allocate this type depends on the number of used ports:

`> alloc_length = sizeof(CM_PD_CREATE_TYPE) + ((map_count - 1) * sizeof(CM_PD_MAP_TYPE));`

If the pointer oem_data is valid:

- the OEM Identification will be visible in DCP
- the I&M5 record gets an entry, which is filled with the given data

Only the values of vendor_id, device_id and SWRevisionPrefix are checked.

The I&M5 record is readable only of specific submodules, see @ref CM_OPC_SV_SUBMODULE_ADD.

The confirmation of PD-Create indicates to the user the number of event and alarm resources.
The user has to provide the resources with the opcodes CM_OPC_PD_PROVIDE_EVENT and  CM_OPC_PD_PROVIDE_ALARM.

With PDRemap the mapping of the port_id may change later, see @ref CM_OPC_PD_REMAP.

Parallel operation of channel-specific services:

At any time only one of the following can run:

- PDCreate
- PDRemap
- PDPrm  service
- PDDelete

see PNIO-Spec:
 - DCP, DevicePropertiesOption/SuboptionOEMDeviceID, OEMDeviceIDBlockRes
 - RecordDataRead, IMData, I&M5 (Index 0xAFF5)

## Sequence Diagram

![Sequence Diagram of CM_OPC_PD_CREATE](cm-pd-create-sequence.svg)

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | PD instance was created.
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_PARAM       | One parameter is out of range
CM_ERR_LOWER_LAYER | Alloc for lower RQB failed.
******************************************************************************/
typedef struct cm_pd_oem_data_tag { /* TIA 1099472 (V6.0i1.48), TIA 1099470 (V5.4i1.24) */

	LSA_UINT16 vendor_id;
	/**< ## REQ
	 * The administrative number vendor_id. \n
	 * See http://www.profibus.com/IM/Man_ID_Table.xml for valid numbers. \n
	 * 0 is not allowed
	 */

	LSA_UINT16 device_id;
	/**< ## REQ
	 * A manufacturer specific number \n
	 * 0 is not allowed
	 */

	LSA_UINT8 IM_Annotation[64];
	/**< ## REQ
	 * Left justified manufacturer specific UTF-8, filled with blanks (0x20), @note Not checked by CM-PD.
	 */

	LSA_UINT8 IM_OrderID[64];
	/**< ## REQ
	 * Left justified manufacturer specific UTF-8, filled with blanks (0x20), @note Not checked by CM-PD.
	 */

	LSA_UINT8 IM_Serial_Number[16];
	/**< ## REQ
	 * This value should uniquely identifies the product from the vendor. Filled with visible characters (0x20..0x7e), @note Not checked by CM-PD.
	 */

	LSA_UINT16 IM_Hardware_Revision;
	/**< ## REQ
	 * 0 .. 0xFFFF (all values valid)
	 */

	LSA_UINT8 SWRevisionPrefix;
	/**< ## REQ
	 * VisibleString[1], one of ('V', 'R', 'P', 'U', 'T')
	 */

	LSA_UINT8 IM_SWRevision_Functional_Enhancement;
	/**< ## REQ
	 * 0 .. 0xFF (all values valid)
	 */

	LSA_UINT8 IM_SWRevision_Bug_Fix;
	/**< ## REQ
	 * 0 .. 0xFF (all values valid)
	 */

	LSA_UINT8 IM_SWRevision_Internal_Change;
	/**< ## REQ
	 * 0 .. 0xFF (all values valid)
	 */

} CM_PD_OEM_DATA_TYPE;

typedef CM_PD_OEM_DATA_TYPE * CM_UPPER_OEM_DATA_PTR_TYPE; /**< The pointer type for CM_PD_OEM_DATA_TYPE. */
typedef CM_PD_OEM_DATA_TYPE const * CM_UPPER_OEM_DATA_CONST_PTR_TYPE; /**< The constant pointer type for CM_PD_OEM_DATA_TYPE. */

#define CM_PDEV_PROP_TYPE_MASK				0x01
#define CM_PDEV_PROP_TYPE_COMPACT			0x00 /**< Within this sublsot_nr  pull and plug is not allowed.	The interface (port_id 0) must always be _COMPACT. @note For each _COMPACT subslot_nr the state _PLUGGED must be given.*/
#define CM_PDEV_PROP_TYPE_MODULAR			0x01 /**< Within this sublsot_nr pull and plug is allowed. (See @ref CM_OPC_PD_SUBMODULE_CONTROL) */

#define CM_PDEV_PROP_STATE_MASK				0x02
#define CM_PDEV_PROP_STATE_PULLED			0x00 /**< This submodule is currently pulled. */
#define CM_PDEV_PROP_STATE_PLUGGED			0x02 /**< This submodule is currently plugged. */

#define CM_PDEV_PROP_CHANGE_PROFIENERGY		0x04

#define CM_PDEV_PROP_RESERVED_CREATE_MASK	0xFC

/** The array containing the mapping information. The "port_id" is the index into the array.
* - map[0] ... the mapping for the interface (port 0)
* - map[1] ... the mapping for port 1
* - and so on
*/
typedef struct cm_pd_map_tag {

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/* Subslot Number (0x8ipp)
	 * @note "pp" needs not be the same as <port_id>.
	 * See chapter @ref cm-pd-channels-pd too.
	 */

	LSA_UINT8 pdev_properties;
	/**< ## REQ
	 * see CM_PDEV_PROP_
	 */

	LSA_UINT8 im0_bits;
	/**< ## REQ
	 * 0 or use the definition of CM_SV_IM0_BITS_xxx, see @ref CM_OPC_SV_SUBMODULE_ADD.
	 */

	/* valid if CM_PDEV_PROP_TYPE_COMPACT or CM_PDEV_PROP_TYPE_MODULAR|CM_PDEV_PROP_STATE_PLUGGED */
	LSA_UINT32  mod_ident;
	/**< ## REQ
	 * Module Ident Number
	 * @note Valid if CM_PDEV_PROP_TYPE_COMPACT or CM_PDEV_PROP_TYPE_MODULAR|CM_PDEV_PROP_STATE_PLUGGED
	 */

	LSA_UINT32  sub_ident;
	/**< ## REQ
	* Submodule Ident Number
	* @note Valid if CM_PDEV_PROP_TYPE_COMPACT or CM_PDEV_PROP_TYPE_MODULAR|CM_PDEV_PROP_STATE_PLUGGED
	*/

} CM_PD_MAP_TYPE;

typedef CM_PD_MAP_TYPE * CM_UPPER_PD_MAP_PTR_TYPE; /**< The pointer type for CM_PD_MAP_TYPE. */

typedef CM_PD_MAP_TYPE const * CM_UPPER_PD_MAP_CONST_PTR_TYPE; /**< The constant pointer type for CM_PD_MAP_TYPE. */

/** The type used for CM_OPC_PD_CREATE. @note Dynamic size! */
typedef struct cm_pd_create_tag {

	LSA_UINT16 max_alarm_data_length;
	/**< ## REQ
	 * Size of the AlarmNotification PDU \n
	 * Value range: CM_ALARM_DATA_LENGTH_MIN to CM_ALARM_DATA_LENGTH_MAX
	 */

	LSA_UINT8 annotation[63+1];
	/**< ## REQ
	 * Text that is entered in the RPC Endpoint Mapper. \n
	 * EPM: zero-terminated string: Refer to "sprintf" format string CLRPC_PNIO_ANNOTATION_FORMAT.
	 */

	CM_UPPER_OEM_DATA_PTR_TYPE  oem_data;
	/**< ## REQ
	 * Optional information for OEM Identification and I&M5 record \n
	 * LSA_NULL: No OEM information. \n
	 * otherwise: Using the OEM information. \n
	 */

	LSA_UINT16 nr_of_event_resources;
	/**< ## REQ
	 * Don't care.
	 * ## CNF
	 * Required event-resources the CM-user must provide with CM_OPC_PD_PROVIDE_EVENT to the device.
	 */

	LSA_UINT16 nr_of_alarm_resources;
	/**< ## REQ
	 * Don't care.
	 * ## CNF
	 * Required alarm-resources the CM-user must provide with CM_OPC_PD_PROVIDE_ALARM to the device.
	 */

	LSA_UINT16 map_count;
	/**< ## REQ
	 * Number of entries in the array "map". \n
	 * The interface and all ports must be mapped. \n
	 * Thus "map_count" must be equal to 1 + EDD_GET_PARAMS::MaxPortCnt (2..n: max(n) == 1 + EDD_CFG_MAX_PORT_CNT)
	 */

	CM_PD_MAP_TYPE  map[1];
	/**< ## REQ
	 * The array containing the mapping information. The "port_id" is the index into the array.
	 * - map[0] ... the mapping for the interface (port 0)
	 * - map[1] ... the mapping for port 1 (edd_port_id)
	 * and so on
	 * @note The CM-user must alloc additional memory for the ports, see also EDD_CFG_MAX_PORT_CNT.
	 */

	/*** nothing here, dynamic size ***/

} CM_PD_CREATE_TYPE;

typedef CM_PD_CREATE_TYPE * CM_UPPER_PD_CREATE_PTR_TYPE; /**< The pointer type for CM_PD_CREATE_TYPE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 201 CM_OPC_PD_DELETE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_PD_DELETE 201 CM_OPC_PD_DELETE
@ingroup cm-pd-opcodes
@{
@brief A CM-PD instance is deleted with PDDelete.
@details
The associated CM-CL, CM-MC or CM-SV instance must have been deleted, before a CM-PD instance can be deleted.

Before confirmation, all request blocks that were provided for indications with CM_OPC_PD_PROVIDE_EVENT and CM_OPC_PD_PROVIDE_ALARM are returned with the response code CM_OK_CANCELLED.

@attention While doing PDDelete, the complete PDEV (i.e., all PDEV sub-components too) must be stopped (close channel). Reason: Consistency of PDEV parameterization.

Parallel operation of channel-specific services:

Refer to @ref CM_OPC_PD_CREATE.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | PD instance was removed.
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_LOWER_LAYER | Stop of LLDP, delay measurement or "OHA" / "EDD" failed

@note No paramters!
******************************************************************************/
typedef LSA_VOID_PTR_TYPE * CM_UPPER_PD_DELETE_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_DELETE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 202 CM_OPC_PD_REMAP                                                       */
/**************************************************************************//**
@defgroup CM_OPC_PD_REMAP 202 CM_OPC_PD_REMAP
@ingroup cm-pd-opcodes
@{
@brief PDRemap changes the mapping of the ports.
@details
This opcode is valid only between the opcodes CM_OPC_PD_CREATE and CM_OPC_PD_DELETE.
The associated CM-CL, CM-MC or CM-SV instance must have been deleted (or never created).
The required size to allocate this type depends on the number of used ports:

> alloc_length = sizeof(CM_PD_REMAP_TYPE) + ((map_count - 1) * sizeof(CM_PD_MAP_TYPE));

If CM-PD responds CM_ERR_xxx, then neither a remapping has occurred nor the CM-PD was marked as "not prmed". The previously existing parameterization and CM-PD-alarm.ind cycle state is still in effect.

With the introduction of CM_OPC_PD_SUBMODULE_CONTROL ports can be pulled or plugged.
PDRemap must not change CM_PDEV_PROP_STATE_xxx, otherwise CM_ERR_PARAM will be returned.

If PDRemap responds with CM_OK, then CM-PD is remapped and marked as "not prmed".
PDRemap has disabled the CM-PD-alarm.ind cycle.
After the CM-PD user has written a valid PRM sequence to CM-PD, the user will get a  PD_PrmApplReady.ind with the new diagnosis state. Additionally, the CM-PD-alarm.ind cycle is enabled too. See also /Parameterization of PDEV/

Parallel operation of channel-specific services:

Refer to @ref CM_OPC_PD_CREATE.

## Sequence Diagram
@todo CM Documentation: Outdated!
![Sequence Diagram of CM_OPC_PD_REMAP](cm-pd-remap-sequence.svg)

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Portmapping changed succesfully.
CM_ERR_PARAM       | - map_count is wrong
|                  | - subslot_nr out of range
CM_ERR_SEQUENCE    | - CM-PD is not created
|                  | - CM-CL, CM-SV or CM-MC is not deleted
******************************************************************************/
typedef struct cm_pd_remap_tag {

	LSA_UINT16 map_count;
	/**< ## REQ
	* Number of entries in the array "map". \n
	* The interface and all ports must be mapped. \n
	* Thus "map_count" must be equal to 1 + EDD_GET_PARAMS::MaxPortCnt (2..n: max(n) == 1 + EDD_CFG_MAX_PORT_CNT)
	*/

	CM_PD_MAP_TYPE  map[1];
	/**< ## REQ
	* The array containing the mapping information. The "port_id" is the index into the array.
	* - map[0] ... the mapping for the interface (port 0)
	* - map[1] ... the mapping for port 1 (edd_port_id)
	* and so on
	* @note Dynamic size! The cm-user must alloc additional memory for the ports, see also EDD_CFG_MAX_PORT_CNT.
	*/

} CM_PD_REMAP_TYPE;

typedef CM_PD_REMAP_TYPE * CM_UPPER_PD_REMAP_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_REMAP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 203 CM_OPC_PD_LINK_STATUS_INFO                                            */
/**************************************************************************//**
@defgroup CM_OPC_PD_LINK_STATUS_INFO 203 CM_OPC_PD_LINK_STATUS_INFO
@ingroup cm-pd-opcodes
@{
@brief With LinkStatusInfo the link status ("auto" (best of all ports) or port-specific) can be tracked.
@details
Pattern: The user tells CM-PD its values of the link status. CM-PD returns the current values if the told values differ from the current values. The user typically starts with "unknown".

The service can be called after PDCreate.CNF and before PDDelete.REQ.

The request is cancelled on CloseChannel.REQ and is returned before CloseChannel.CNF with "cancelled".

On specifying CM_PORT_ID_AUTO, the system makes a "best case" consideration of all ports of the interface. The following procedure is followed in this context:

The link is UP if at least one port is UP.
The link is DOWN if all ports are DOWN.
The field "autoneg" is always CM_LINK_AUTONEG_UNKNOWN.
For details of UP_CLOSED, UP_DISABLED, speed and mode, refer to [/8/](@ref cm-ref-no-8).

Usecase 1 (polling):

The user fills the RQB with the corresponding values for "UNKNOWN". The confirmation is returned immediately with the current values (because the "status" is never unknown in EDD).

Usecase 2 (callback):

The user starts with "UNKNOWN" status and fills the RQB with the corresponding values. If the link status changes (practically immediately for the first request), the confirmation is returned with the current values. The user updates his local values and determines the changes. Finally, he returns the unchanged RQB back to CM. Fresh allocation and filling is possible.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | The result is valid.
CM_OK_CANCELLED    | The result is invalid. On close-channel.
CM_ERR_SEQUENCE    | Request has been made prior to CM_OPC_PD_CREATE or indication resource has already been provided.
CM_ERR_LOWER_LAYER | "port_id" out of range.
******************************************************************************/
#define CM_PORT_ID_AUTO  EDD_PORT_ID_AUTO

#define CM_LINK_STATUS_UNKNOWN      EDD_LINK_UNKNOWN
#define CM_LINK_STATUS_UP           EDD_LINK_UP
#define CM_LINK_STATUS_DOWN         EDD_LINK_DOWN
#define CM_LINK_STATUS_UP_CLOSED    EDD_LINK_UP_CLOSED   /**< same as "down" for normal communication */
#define CM_LINK_STATUS_UP_DISABLED  EDD_LINK_UP_DISABLED /**< same as "down" for normal communication */

#define CM_LINK_SPEED_UNKNOWN  EDD_LINK_UNKNOWN
#define CM_LINK_SPEED_10_M     EDD_LINK_SPEED_10
#define CM_LINK_SPEED_100_M    EDD_LINK_SPEED_100
#define CM_LINK_SPEED_1_G      EDD_LINK_SPEED_1000
#define CM_LINK_SPEED_10_G     EDD_LINK_SPEED_10000

#define CM_LINK_MODE_UNKNOWN      EDD_LINK_UNKNOWN
#define CM_LINK_MODE_HALF_DUPLEX  EDD_LINK_MODE_HALF
#define CM_LINK_MODE_FULL_DUPLEX  EDD_LINK_MODE_FULL

#define CM_LINK_AUTONEG_UNKNOWN   EDD_AUTONEG_UNKNOWN
#define CM_LINK_AUTONEG_ON        EDD_AUTONEG_ON
#define CM_LINK_AUTONEG_OFF       EDD_AUTONEG_OFF

typedef struct cm_link_status_info_tag {

	LSA_UINT16 interface_id;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * Interface-ID, value range 1..16. Refer to @ref CM_OPC_PD_CREATE and EDD_SRV_GET_PARAMS::InterfaceID.
	 */

	LSA_UINT16 port_id;
	/**< ## REQ
	* CM_PORT_ID_AUTO or 1 .. n (see CM_OPC_PD_CREATE::map_count)
	*/

	LSA_UINT8 status;
	/**< ## REQ
	* Refer to cm_link_status_enum
	* ## CNF
	* Current value (but never CM_LINK_STATUS_UNKNOWN)
	*/

	LSA_UINT8 speed;
	/**< ## REQ
	 * Refer to cm_link_speed_enum
	 * ## CNF
	 * Current value \n
	 * Valid if "status" == CM_LINK_STATUS_UP[_CLOSED|_DISABLED], otherwise CM_LINK_SPEED_UNKNOWN
	 */

	LSA_UINT8 mode;
	/**< ## REQ
	 * Refer to cm_link_mode_enum
	 * ## CNF
	 * Current value \n
	 * valid if "status" == CM_LINK_STATUS_UP[_CLOSED|_DISABLED] \n
	 * Otherwise CM_LINK_MODE_UNKNOWN
	 */

	LSA_UINT8 autoneg;
	/**< ## REQ
	 * Refer to CM_LINK_AUTONEG_XXX
	 * ## CNF
	 * Current value \n
	 * valid if "status" == CM_LINK_STATUS_UP[_CLOSED|_DISABLED] \n
	 * Otherwise CM_LINK_AUTONEG_UNKNOWN
	 */

} CM_PD_LINK_STATUS_INFO_TYPE;

typedef CM_PD_LINK_STATUS_INFO_TYPE * CM_UPPER_PD_LINK_STATUS_INFO_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_LINK_STATUS_INFO. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 204 CM_OPC_PD_SYNC_LED_INFO                                               */
/**************************************************************************//**
@defgroup CM_OPC_PD_SYNC_LED_INFO 204 CM_OPC_PD_SYNC_LED_INFO
@ingroup cm-pd-opcodes
@{
@brief The SyncLedInfo service can be used to visualize the synchronization status and the IRT port status with a LED.
@details
@note This indication is not synchronized with parameterization changes. This means that the "off" state is signaled delayed and a fast "on - off - on" change may not be signaled at all.

The service can be called after opening the channel.

The meaning of "sync_led_info" depends, on whether the service is called for the interface or for a port.

## 1) edd_port_id == 0:

Info is requested about clock synchronization (SyncID 0)

status                             | meaning
-----------------------------------|--------------------------------------------------------------------------------------------
CM_PD_SYNC_LED_INFO_NOT_CONFIGURED | synchronization is not configured (parameterization does not contain the PDSyncData record)
CM_PD_SYNC_LED_INFO_NOT_SYNC       | synchronizing but not yet synchronized
CM_PD_SYNC_LED_INFO_SYNC           | the clock (from which the send-clock is derived) is synchronized

The field "is_rate_valid" contains additional information for the state CM_PD_SYNC_LED_INFO_NOT_SYNC.

is_rate_valid           | meaning
------------------------|------------------------------------------------------------------------------------------------------------------
CM_PD_IS_RATE_VALID_NO  | means that no sync frames are being received
CM_PD_IS_RATE_VALID_YES | means that sync frames are being received and the rate was determined (and the station will be synchronized soon)

The fields "rcv_sync_prio" and "local_prio" contains additional information for the state CM_PD_SYNC_LED_INFO_SYNC.

field         | meaning
--------------|-----------------------------------------------------------------------------------------------
rcv_sync_prio | MasterPriority1 from SyncFrame (0 at active master)
local_prio    | MasterPriority1 from SyncRecord (0 at slave) with ActiveBit set at active Master after Startup

More information can be found in the GSY documentation (opcode GSY_OPC_DIAG_INDICATION_PROVIDE).

## 2) edd_port_id != 0:

Info is requested about IRT-operation of the specified port

status                             | meaning
-----------------------------------|--------------------------------------------------------------------------------------------
CM_PD_SYNC_LED_INFO_NOT_CONFIGURED | IRT-operation is not configured (parameterization does not contain the IRData record or this port is not referenced by the IRData record)
CM_PD_SYNC_LED_INFO_NOT_SYNC       | IRT-operation is configured, but the station is either not yet synchronized or the port has an RTClass3 mismatch diagnosis
CM_PD_SYNC_LED_INFO_SYNC           | the local RTClass3_PortStatus == RTCLASS3_RUN

The other fields are "don't evaluate".

The field pll_state is only filled, when the subcomponent GSY is added as lower layer. See CM_CFG_USE_GSY.

Usage:

The CM-user starts with the local status sync_led_info = CM_PD_SYNC_LED_INFO_NOT_CONFIGURED, is_rate_valid = CM_PD_IS_RATE_VALID_NO, rcv_sync_prio = 0 and local_prio = 0 and
fills the first request with these values. If the status changes, the confirmation is returned with the current values. The CM-user updates its local values and
finally returns the RQB with the unchanged arguments (i.e., with the current status) again to CM. New allocation and filling is also possible. The request is cancelled
in case of CloseChannel and is returned with response "cancelled" before the CloseChannel confirmation.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | The result is valid.
CM_OK_CANCELLED    | Request was cancelled by CmCloseChannel.
CM_ERR_SEQUENCE    | Request has been made prior to CM_OPC_PD_CREATE.
******************************************************************************/
enum cm_pd_sync_led_info_enum {
	CM_PD_SYNC_LED_INFO_NOT_CONFIGURED = 0, /**< meaning see text above */
	CM_PD_SYNC_LED_INFO_NOT_SYNC       = 1, /**< meaning see text above */
	CM_PD_SYNC_LED_INFO_SYNC           = 2, /**< meaning see text above */
	CM_PD_SYNC_LED_INFO_MAX /**< for range-checking */
};

#define CM_PD_PLL_STATE_UNKNOWN  0 /**< initial pll_state */

#define CM_PD_IS_RATE_VALID_NO  0
#define CM_PD_IS_RATE_VALID_YES 1

typedef struct cm_pd_sync_led_info_tag {

	LSA_UINT16 edd_port_id;
	/**< ## REQ
	 * 0 = Interface, 1 = Port 1, 2 = Port 2, etc.
	 */

	LSA_UINT16 sync_led_info;
	/**< ## REQ
	* See cm_pd_sync_led_info_enum and text
	* ## CNF
	* Current value
	*/

	LSA_UINT8 is_rate_valid;
	/**< ## REQ
	* See text
	* ## CNF
	* Current value
	*/

	LSA_UINT8 rcv_sync_prio;
	/**< ## REQ
	* See text
	* ## CNF
	* Current value
	*/

	LSA_UINT8 local_prio;
	/**< ## REQ
	* See text
	* ## CNF
	* Current value
	*/

	LSA_UINT8 pll_state;
	/**< ## REQ/CNF
	* See CM_PD_PLL_STATE_UNKNOWN for initialization and gsy_usr.h::GSY_PLL_STATE_SYNC_OFF, etc. for possible values.
	*/

} CM_PD_SYNC_LED_INFO_TYPE;

typedef CM_PD_SYNC_LED_INFO_TYPE * CM_UPPER_PD_SYNC_LED_INFO_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_SYNC_LED_INFO. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_OPC_PD_SUBMODULE_PULL_PLUG                                             */
/**************************************************************************//**
@defgroup CM_OPC_PD_SUBMODULE_PULL_PLUG XXX CM_OPC_PD_SUBMODULE_PULL_PLUG
@ingroup cm-pd-opcodes
@brief Obsolete!
The opcode has been renamed to @ref CM_OPC_PD_SUBMODULE_CONTROL.
******************************************************************************/


/*****************************************************************************/
/* 205 CM_OPC_PD_SUBMODULE_CONTROL                                           */
/**************************************************************************//**
@defgroup CM_OPC_PD_SUBMODULE_CONTROL 205 CM_OPC_PD_SUBMODULE_CONTROL
@ingroup cm-pd-opcodes
@{
@brief The request CM_PD_SUBMODULE_CONTROL pulls a PDev submodule or plugs a PDev submodule or set/reset the PROFIENERGY bit of a PDEV submodule.
@details
Slot_nr and subslot_nr addresses a submodule, which must be mapped by PDCreate.
This submodule must have the type _MODULAR.

With pdev_properties CM_PDEV_PROP_STATE_PULLED the addressed submodule is pulled (PDPull).
The internal IdentInfo is set to NO and the internal mod_ident / sub_ident are set to 0.
A PDPull stops the alarm indication cycle to the PD-User.
If a CMSV is active, PDPull triggers CMSV. If necessary, CMSV starts a pull alarm sequence.

A PDPull removes all ExtChannelDiagnosis that have been passed with PDDiagAdd to this PDev submodule.

With pdev_properties CM_PDEV_PROP_STATE_PLUG the addressed submodule is plugged (PDPlug).
The internal IdentInfo is set to OK and the given mod_ident, sub_ident and the im0_bits will be taken.
A PDPlug.cnf starts the alarm indication cycle to the PD-User.
If a CMSV is active, PDPlug triggers CMSV. If necessary, CMSV starts a plug alarm sequence.


With pdev_properties CM_PDEV_PROP_CHANGE_PROFIENERGY the addressed submodule modifies the internal property according to the parameter "im0_bits":
- CM_SV_IM0_BITS_NOTHING
  - Disable the PROFIENERGY index range
- CM_SV_IM0_BITS_PROFIENERGY
  -All PROFIENERGY indices are forwarded to the CM-User
- This service is allowed only with plugged submodules.

After PDPull.cnf a PDAlarm(Pull).ind for this submodule is indicate to the PD-User.

After PDPlug.cnf a PDAlarm(Plug).ind for this submodule is indicate to the PD-User.
After PDAlarm(Plug).rsp the available diagnosis information is send.

@see [CM_OPC_PD_ALARM_IND](@ref CM_OPC_PD_ALARM_IND_RSP).

If there is currently no alarm-resource for needed PDAlarm(Pull/Plug).ind, this service is delayed, until the previous PDAlarm.rsp is given.


For interaction with CMSV, see @ref CM_OPC_SV_AR_OWNERSHIP_IND and @ref cm-server-channels-ownership-ownership-pdpull-pdplug.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_PARAM       | - sub_nr / subslot_nr not mapped (see PDCreate)
|                  | - the subslot has type _COMPACT
|                  | - not allowed values in im0_bits
CM_ERR_SEQUENCE    | - CM-PD is not created
|                  | - Submodule is already pulled / already plugged
******************************************************************************/
#define CM_PDEV_PROP_RESERVED_CONTROL_MASK 0xF9

typedef struct cm_pd_submodule_control_tag {

	LSA_UINT16 slot_nr;
	/** ## REQ
	 * logical address \n
	 * Slot Number
	 */

	LSA_UINT16 subslot_nr;
	/** ## REQ
	 * logical address \n
	 * Subslot Number (0x8ipp)
	 */

	LSA_UINT8 pdev_properties;
	/** ## REQ
	 * CM_PDEV_PROP_STATE_PULLED: uses: - \n
	 * or CM_PDEV_PROP_STATE_PLUGGED: uses: im0_bits, mod_ident, sub_ident \n
	 * or CM_PDEV_PROP_CHANGE_PROFIENERGY: uses: im0_bits::CM_SV_IM0_BITS_PROFIENERGY
	 */

	LSA_UINT8 im0_bits;
	/** ## REQ
	 * 0 or uses the definition of CM_SV_IM0_BITS_xxx, see @ref CM_OPC_SV_SUBMODULE_ADD.
	 */

	LSA_UINT32  mod_ident;
	/** ## REQ
	 * Module Ident Number \n
	 * Valid for  _PLUGGED submodule
	 */

	LSA_UINT32  sub_ident;
	/** ## REQ
	 * Submodule Ident Number \n
	 * Valid for _PLUGGED submodule
	 */

} CM_PD_SUBMODULE_CONTROL_TYPE;

typedef CM_PD_SUBMODULE_CONTROL_TYPE * CM_UPPER_PD_SUBMODULE_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_SUBMODULE_CONTROL. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 206 CM_OPC_PD_DIAG_ADD                                                    */
/**************************************************************************//**
@defgroup CM_OPC_PD_DIAG_ADD 206 CM_OPC_PD_DIAG_ADD
@ingroup cm-pd-opcodes
@{
@brief The request PDDiagAdd (CM_OPC_PD_DIAG_ADD) adds an ExtChannelDiagnosis to a PDev-submodule.
@details
With the array elem[] one request may add nr_of_elems ExtChannelDiagnosis.

For a successful add
- all submodules must mapped with PDCreate
- all submodules must be plugged
- all arguments must be valid (e.g. channel_properties)
- and memory must be available
or an appropriate error will return.

The following set of parameters composes the key of one diagnosis entry:
(See also @ref CM_OPC_SV_DIAG_ADD2).

> Location part of the key:
> - Api (PDev: always 0)
> - Slot_nr
> - Subslot_nr
> - channel_number (PDev: always 0x8000)
> - channel_properties:: CM_CHPR_ACCU_XXX (Bit 8)
> - channel_properties::CM_CHPR_DIR_XXX (Bit 13 - 15)
>
> Diagnosis part of the key:
> - channel_error_type
> - ext_channel_error_type
>
> For more information on Profinet diagnosis, please refer to the PNIO Diagnosis Guideline [/20/](@ref cm-ref-no-20).

Side effect:
If the PDev is owned by an SV-AR, an appearing diagnosis alarm is issued implicitly via the AR to the IOC. No further action from PD-user is required.

The sequence is as follows: first the request is confirmed (PDDiagAdd.cnf) and then alarms are sent to the SV-AR.

For ExtChannelDiagnosis added by the PD-User, the alarm type "CM_ALARM_TYPE_DIAGNOSIS(1)" is used.

For changing severity of the diagnosis, the PD-User must remove the diagnosis with existing severity and add the diagnosis with new severity.

For every added ExtChannelDiagnosis an appearing PDAlarm.ind is triggered to the PD-User.

Open issue: CM_CHPR_DIR_XXX:
The direction is currently not evaluated (as of 10.12.2015).

Open issue: Against the DiagGuideline:
Diagnosis, where the key already exists, does only replace the ext_channel_add_value. No alarm is triggered.
The update frequency of the ext_channel_add_value (higher then 1Hz) is also not checked.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_PARAM       | - nr_of_elems is zero.
|                  | - PDev-Submodule not mapped (see @ref CM_OPC_PD_CREATE)
|                  | - channel_properties out of range
|                  | - channel_error_type out of range
CM_ERR_SEQUENCE    | - CM-PD is not created
|                  | - PDev-Submodule is not plugged (see @ref CM_OPC_PD_SUBMODULE_CONTROL)
CM_ERR_ALLOC_UPPER | can't alloc diag-entry
******************************************************************************/
typedef struct cm_pd_submod_diag_tag {

/*	LSA_UINT32  api;                    ** API, fix 0 */

	LSA_UINT16 slot_nr;
	/** ## REQ
	 * Slot Number - see @ref CM_OPC_PD_CREATE
	 */

	LSA_UINT16 subslot_nr;
	/** ## REQ
	 * Subslot Number - see @ref CM_OPC_PD_CREATE
	 */

/*	LSA_UINT16 channel_number;         ** ChannelNumber, fix 0x8000 */

	LSA_UINT16 channel_properties;
	/** ## REQ
	 * ChannelProperties, See CM_CHPR_xxx masks:
	 * - CM_CHPR_TYPE_MASK
	 * - CM_CHPR_ACCU_MASK ... only CM_CHPR_ACCU_NO
	 * - CM_CHPR_MAINT_MASK
	 * - CM_CHPR_SPEC_MASK ... only CM_CHPR_SPEC_APP
	 * - CM_CHPR_DIR_MASK
	 * (see @ref cm-channel-properties)
	 */

	LSA_UINT16 channel_error_type;
	/** ## REQ
	 * ChannelErrorType 1..0x7FFF. For values refer to [/11/](@ref cm-ref-no-11).
	 */

	LSA_UINT16 ext_channel_error_type;
	/** ## REQ
	 * ExtChannelErrorType For values refer to [/11/](@ref cm-ref-no-11). Range not checked
	 */
	LSA_UINT32  ext_channel_add_value;
	/** ## REQ
	 * ExtChannelAddValue - Additional Value
	 */

	LSA_VOID_PTR_TYPE void_ptr;
	/** ## REQ
	 * @note do not evaluate (used internally)
	 */
} CM_PD_SUBMOD_DIAG_TYPE;

/** The type used for CM_OPC_PD_DIAG_ADD. @note Dynamic size! */
typedef struct cm_pd_diag_add_tag {

	LSA_UINT16 nr_of_elems;
	/**< ## REQ
	 * Number of elements contained in the array "elem" (> 0)
	 */

	CM_PD_SUBMOD_DIAG_TYPE  elem[1];
	/**< ## REQ
	 * @see CM_PD_SUBMOD_DIAG_TYPE
	 * elem[2..n] follow here
	 */

} CM_PD_DIAG_ADD_TYPE;

typedef CM_PD_DIAG_ADD_TYPE * CM_UPPER_PD_DIAG_ADD_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_DIAG_ADD. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 207 CM_OPC_PD_DIAG_REMOVE                                                 */
/**************************************************************************//**
@defgroup CM_OPC_PD_DIAG_REMOVE 207 CM_OPC_PD_DIAG_REMOVE
@ingroup cm-pd-opcodes
@{
@brief The request PDDiagRemove removes ExtChannelDiagnosis from PDev-submodules.
@details
With the array elem[] one request may remove nr_of_elems ExtChannelDiagnosis. @note Dynamic size!

The key of the diagnosis entry follows the definition of PDDiagAdd.

Side effect:
If the PDev is owned by an SV-AR, a disappearing diagnosis alarm is issued implicitly via the AR to the IOC. No further action from PD-user is required.

For ExtChannelDiagnosis added by the PD-User, the alarm type "CM_ALARM_TYPE_DIAGNOSIS(1)" is used.

For every removed ExtChannelDiagnosis a disappearing PDAlarm.ind is triggered to the PD-User.

If the PDev-submodule is pulled (see PDPull), then all existing ExtChannelDiagnosis, given by the PD-User, of this submodule are removed internally.

If the removing diagnosis is not found, a previous PDPull / PDPlug sequence will assumed, the response keep CM_OK.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_PARAM       | - nr_of_elems is zero.
|                  | - PDev-Submodule not mapped (see @ref CM_OPC_PD_CREATE)
|                  | - channel_properties out of range
|                  | - channel_error_type out of range
CM_ERR_SEQUENCE    | - CM-PD is not created
|                  | - PDev-Submodule is not plugged (see @ref CM_OPC_PD_SUBMODULE_CONTROL)
******************************************************************************/
typedef struct cm_pd_diag_remove_tag {

	LSA_UINT16 nr_of_elems;
	/**< ## REQ
	 * Number of elements contained in the array "elem" (> 0)
	 */

	CM_PD_SUBMOD_DIAG_TYPE  elem[1];
	/**< ## REQ
	 * @see CM_PD_SUBMOD_DIAG_TYPE
	 * elem[2..n] follow here
	 */

} CM_PD_DIAG_REMOVE_TYPE;

typedef CM_PD_DIAG_REMOVE_TYPE * CM_UPPER_PD_DIAG_REMOVE_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_DIAG_REMOVE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 210 CM_OPC_PD_PRM_READ                                                    */
/**************************************************************************//**
@defgroup CM_OPC_PD_PRM_READ 210 CM_OPC_PD_PRM_READ
@ingroup cm-pd-opcodes
@{
@brief The Real, Adjust, Expected and Diagnosis records can be read in the PNIO format (refer to [/11/](@ref cm-ref-no-11)) from the PDEV with this service.
@details
The supported record indices are describing in [/17/](@ref cm-ref-no-17).

The current parameterization is used to read the adjust, expected and diagnosis records. The indication PrmApplReady.ind signals the current parameterization, see CM_OPC_PD_EVENT_APPL_READY_IND.

Special case: reading of diagnosis indices with record_data_length == 0:
If PNIOStatus is CM_PNIO_ERR_NONE, then there are no diagnosis data available.

Starting with PDDiagAdd / PDDiagRemove the following index will read from PDev locally:
0x800A, 0x800B, 0x800C, 0x8010, 0x8011, 0x8012, 0x8013
If a CMSV-channel exists, the index 0xF00C and 0xF80C are forwarded to CMSV,
Otherwise these indexes will also be read from PDev locally.

If the addressed submodule is "PULLED", see @ref CM_OPC_PD_SUBMODULE_CONTROL, then the cm_pnio_err is set to "CM_PNIO_FAULTY_Record__InvalidSlotSubslot(8)".

## Parallel operation of channel specific services

Refer to @ref CM_OPC_PD_CREATE.

@see @ref CM_PD_PRM_RDWR_TYPE

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | If cm_pnio_err == CM_PNIO_ERR_NONE, then the record was read.
CM_ERR_PARAM       | Parameter error.
******************************************************************************/

/** @brief SendClock-record ::= definition see @ref cm_record_index_classify */
enum cm_sendclock_prop_enum {
	CM_SENDCLOCK_PROP_SC_VAR_RR_VAR	= 0x0000, /**< send_clock: variable  reduction_ratio: variable */
	CM_SENDCLOCK_PROP_SC_VAR_RR_FIX	= 0x0001, /**< send_clock: variable  reduction_ratio: fixed    */
	CM_SENDCLOCK_PROP_SC_FIX_RR_VAR	= 0x0002, /**< send_clock: fixed     reduction_ratio: variable */
	CM_SENDCLOCK_PROP_SC_FIX_RR_FIX	= 0x0003  /**< send_clock: fixed     reduction_ratio: fixed    */
};

#define CM_PD_MK_REMANENT_NO  0
#define CM_PD_MK_REMANENT_YES 1

#define CM_PD_IS_LOCAL_NO  0
#define CM_PD_IS_LOCAL_YES 1

typedef struct cm_pd_prm_rdwr_tag {

	LSA_UINT16 slot_nr;
	/**< ## REQ:
	 * ### READ/WRITE: Slot Number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ:
	* ### READ/WRITE: Subslot Number
	*/

	LSA_UINT32  record_index;
	/**< ## REQ
	 * ### READ
	 * see PNIO-Spec
	 * ### WRITE
	 * Record Index <= 0xFFFF are defined by PROFINET IO, Record_index > 0xFFFF are non-standardized index. All supported indexes are described in [/17/](@ref cm-ref-no-17).
	 */

	LSA_UINT32  record_data_length;
	/**< ## REQ
	 * ### READ
	 * Length of buffer
	 * ### WRITE
	 * Length of record in bytes
	 * ## CNF
	 * ### READ
	 * Length of record in bytes
	 */
	CM_COMMON_MEM_U8_PTR_TYPE  record_data;
	/**< ## REQ
	 * ### READ
	 * Pointer to the buffer
	 * ### WRITE
	 * Pointer to the record
	 * ## CNF
	 * ### READ
	 * the read data
	 */

	LSA_UINT32  cm_pnio_err;
	/**< ## CNF
	 * ### READ/WRITE
	 * Valid for rqb.response == CM_OK. \n
	 * Format corresponds to standardized PNIOStatus format. \n
	 * Refer also to cm_err.h.
	 *
	 * No error: CM_PNIO_ERR_NONE.
	 *
	 * Error                             | Error Code
	 * ----------------------------------|--------------------------------
	 * Index unknown:	                 | ErrorCode1=0xB0, ErrorCode2 = 0
	 * Slot/subslot does not match index | ErrorCode1=0xB0, ErrorCode2 = 1
	 * Slot/subslot not mapped	         | ErrorCode1=0xB2
	 * Block structure is  inconsistent  | ErrorCode1=0xB1
	 * Other errors (Alloc, ...)         | ErrorCode1=0xA9
	 */

	LSA_UINT16 ret_val_1;
	/**< ## CNF
	 * ### READ/WRITE
	 * return value 1, Valid for rqb.response == CM_OK, always 0.
	 */
	LSA_UINT16 ret_val_2;
	/**< ## CNF
	 * ### READ/WRITE
	 * return value 2, Valid for rqb.response == CM_OK, always 0.
	 */

	LSA_UINT8  mk_remanent;
		/**< ## REQ
		 * ### WRITE
		 * the user has to specify this flag for the opcodes (AP00738255, CM V4.1i2.1)
		 * - CM_OPC_PD_PRM_WRITE
		 * - CM_OPC_SV_AR_PDEVPRM_WRITE
		 *
		 * CM_PD_MK_REMANENT_NO ... the record will **not** be made remanent \n
		 * CM_PD_MK_REMANENT_YES .. the record will be made remanent
		 */

	LSA_UINT8  is_local;
		/**< this flag is set internally (users need not set it) for the opcodes
		 * @internal
		 * ## REQ
		 * - CM_OPC_PD_PRM_READ
		 * - CM_OPC_PD_PRM_WRITE
		 *
		 * the user has to specify this flag for the opcode (AP00585108, CM V4.0i70.4)
		 * - CM_OPC_SV_AR_PDEVPRM_WRITE
		 *
		 * @note Semantics of the flag have changed with CM V4.1i2.1 (AP00738255)
		 *
		 * CM_PD_IS_LOCAL_NO ... the record will be checked with normative rules \n
		 * CM_PD_IS_LOCAL_YES .. the record will be checked with 'local' rules
		 * @endinternal
		 */

	LSA_UINT32  cls;
		/**< this field is used internally only (users need not set it)
		 * @internal
		 * ## REQ
		 * see CM_REC_IDX_MASK, etc.
		 * - CM_OPC_PD_PRM_READ_SERVER
		 * - CM_OPC_PD_PRM_WRITE_SERVER
		 * - CM_OPC_PD_PRM_READ_CLIENT
		 * @endinternal
		 */

} CM_PD_PRM_RDWR_TYPE;

typedef CM_PD_PRM_RDWR_TYPE * CM_UPPER_PD_PRM_RDWR_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_READ and CM_OPC_PD_WRITE. */

typedef CM_PD_PRM_RDWR_TYPE const * CM_UPPER_PD_PRM_RDWR_CONST_PTR_TYPE; /**< The const type used for CM_OPC_PD_READ and CM_OPC_PD_WRITE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 211 CM_OPC_PD_PRM_BEGIN                                                   */
/**************************************************************************//**
@defgroup CM_OPC_PD_PRM_BEGIN 211 CM_OPC_PD_PRM_BEGIN
@ingroup cm-pd-opcodes
@{
@brief PrmBegin requires the ownership of the PDEV for parameterization purpose.
@details
If the ownership can take over, it prepares itself and all PDEV subcomponents for a new parameterization process.

The end of the parameterization process is signaled with PrmApplReady.ind. To do this, one indication resource must be provided prior to PrmBegin (see CM_ERR_RESOURCE) through @ref CM_OPC_PD_PROVIDE_ALARM.

The parameterization process of the PDEV takes place in the following sequence (PrmSequence):
- PDPrmBegin
- PDPrmWrite (0 to n times)
- PDPrmEnd (is confirmed immediately)
- PDPrmApplReady indication
- Optional: PDPrmApplReady indication, as often as the newly tailored parameterization needs to be written internally (New behavior since v5.3i2.1)

Refer to the sequence diagram [below](@ref cm-pd-prm-begin-sequence-diagram).

Special case 0 records (empty parameterization): The PDEV components assume a well defined default status in this case. Refer to documentation of EDDS, EDDI, EDDP, MRP, GSY, POF and OHA.

PrmBegin responds with CM_ERR_SEQUENCE on any of the following conditions:
- The PDEV is currently owned by a remote AR.
- Any client device has been added.
- A parameterization process is currently running.
In these cases, the ownership can't take over and the parameterization process can't take place.

Only within a parameterization sequence PrmWrite is allowed to write PRM records. For a list of known PRM records look at [/17/](@ref cm-ref-no-17). Otherwise only NONPRM records are allowed.

PrmEnd indicates the end of the parameterization process and activates the written parameterization. See also @ref CM_OPC_PD_PRM_END.

The whole sequence is finished with the PrmApplReady.ind. This indication signals the initial diagnosis data of the PDEV submodules, see [CM_OPC_PD_EVENT_APPL_READY_IND](@ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP).
This first PrmApplReady.ind release also the ownership of the PDEV.

Further update to this initial diagnosis will be done with the help of PDAlarm.ind.

Any further optional PrmApplReady.ind initializes the diagnosis again. Any following PDAlarm.ind updates this diagnosis.

@note The ownership of the PDEV must be released before the CM-SV-Device, which contains the PDEV, may be activated (see CM_SV_DEVICE_CONTROL_CMD_ACTIVATE).

# States of the PDev

![PDev-PrmState](cm-pd-pdev-states-prm.svg)

![Possible states within "PDevDiagnosis"](cm-pd-pdev-states-pdevdiagnosis.svg)

PDevReadinessTrigger:
Under others dependent on the readiness of the PDev, one of the following configurations is enabled within PDev:

## "Given parameterization"
This parameterization is used, if the PDev is ready.
The set of PrmRecords is given to PDev within one PrmSequence.
This parameterization may be modified by PrmEnd:

- set_mrp_off: If set to CM_SET_MRP_OFF_YES and no other MRP record were written, then CM-PD generates an "MRP off" record.

## "Remanent parameterization"
PDSyncPLL, PNIdentification, PDNRTFeedInLoadLimitation: If any of these records is written from CM-PD-user, then this record will be kept and merged with every REMOTE PRM record set. If the respective record is merged, then its mk_remanent flag is set to CM_PD_MK_REMANENT_YES to keep this record in the remanent storage. This will be done, until the CM-PD-user writes a new PRM sequence (without this record).

## "Empty parameterization"
This parameterization is used, if the PDev is not ready, or the "given parameterization" causes a parameterization fault.
Only the PrmRecord PDSendClock is written to the PDev. The SendClockFactor is taken from the given parameterization.

## "Default parameterization"
This parameterization is used, if the DCP has triggered a "ResetToFactory".
Only the PrmRecord PDSendClock is written to the PDev. The SendClockFactor 32 is used.
With EDDS the configured SendClockFactor is used.


Within a MasterProject, the given parameterization may be adapted according the requirements given by the tailoring process.  See chapter @ref cm-pd-channels-series-machines.

A given parameterization is known as MasterProject, if the PrmRecord PDMasterTailorData is written to the interface-submodule.

## READiness of the PDev

Described in chapter @ref cm-pd-channels-series-machines-readiness.

# Sequence diagram # {#cm-pd-prm-begin-sequence-diagram}

![Sequence diagram Parameterization](cm-pd-parameterization-sequence-diagram.svg)

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Ready for new parameterization.
CM_ERR_RESOURCE    | There is no indication resource.
CM_ERR_SEQUENCE    | Can't get ownership of PDEV (CM-SV, PDEV-device is active).
******************************************************************************/
typedef struct cm_pd_prm_begin_tag {

	struct {
		LSA_UINT16 slot_nr; /* do not evaluate (used internally) */
		LSA_UINT16 subslot_nr; /* do not evaluate (used internally) */
	} sv; /**< Only for CM_OPC_PD_PRM_BEGIN_SERVER (HIF: do nothing). @attention do not evaluate (used internally) */

} CM_PD_PRM_BEGIN_TYPE;

typedef CM_PD_PRM_BEGIN_TYPE * CM_UPPER_PD_PRM_BEGIN_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_PRM_BEGIN. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 212 CM_OPC_PD_PRM_WRITE                                                   */
/**************************************************************************//**
@defgroup CM_OPC_PD_PRM_WRITE 212 CM_OPC_PD_PRM_WRITE
@ingroup cm-pd-opcodes
@brief With this service the "adjust" and "check" records in the PNIO format  (refer to [/11/](@ref cm-ref-no-11)) are written to the PDEV. The supported record indices are listed in [/17/](@ref cm-ref-no-17)).

There are two kinds of record indices:
1. PRM-records
  These records are only allowed within a PRM-phase
2. NONPRM-records
  These records are allowed within and outside the PRM-phase.
  Even if the PDEV is owned by the CMSV, NONPRM records can be written.

PRM-records are collected in the "given parameterization" and become enabled with PDPrmEnd.

NONPRM-records are written immediate to the responsible sub-component.

# Parallel operation of channel specific services

Refer to @ref CM_OPC_PD_CREATE.

# SendClock record # {#cm-pd-prm-write-send-clock-record}

The SendClock is part of the parameterization. With CM V3.12i40 the opcode CM_OPC_PD_SET_SENDCLOCK was converted into a record (CM: AP00383340, EDD: AP00383360, GSY: AP00383363). For the definition of the record please refer to /17/.

## Usecase "startup" and "central PDev" (PDPrmEnd initiated from CM-PD-User)

If the SendClock record is not written by the user, CM-PD creates one internally:
- EDDS:
  - SendClockFactor = EDD_SRV_GET_PARAMS::CycleBaseFactor
  - SendClockProperties = 2 (SC_FIX, RR_VAR)
- EDDI and EDDP:
  - SendClockFactor = 1ms
  - SendClockProperties = 1 (SC_VAR, RR_FIX)

## Usecase "remote parameterization" (PDPrmEnd initiated from CMSV)

SendClockFactor and SendClockProperties are derived from "IODConnectReq"

Ad SendClockProperties:

An IO Controller will typically use 3 (SC_FIX, RR_FIX). If adaption is necessary, it can be done explicitly by means of "local adaption" (see @ref CM_OPC_CL_DEVICE_ADD).

An IO Controller with integrated IO Device will typically use 2 or 3.

An IO Device will typically let CM-PD create the record (i.e., empty remanence after reset to factory settings).

# MultiplePDEV record

If the response of CM_OK is given, then all records are written successfully. Otherwise the response of the first faulty record is given.

# Remanent records

PDSyncPLL, PNIdentification, PDNRTFeedInLoadLimitation show a special behavior.
If any of these records is written from CM-PD-user, then this record will be kept and merged with every REMOTE PRM record set. If the respective record is merged, then its mk_remanent flag is set to CM_PD_MK_REMANENT_YES to keep this record in the remanent storage. This will be done, until the CM-PD-user writes a new PRM sequence (without this record) or a reset-to-factory occurs.

See also CM_PD_SYNC_LED_INFO_TYPE::pll_state.

@see @ref CM_PD_PRM_RDWR_TYPE

# Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | If cm_pnio_err == CM_PNIO_ERR_NONE, the record was written.
CM_ERR_PARAM       | Parameter error.
CM_ERR_SEQUENCE    | Call sequence not as specified, e.g. user is not owner of PDEV.
CM_ERR_ALLOC_UPPER | Cannot allocate internal upper RQB.
******************************************************************************/


/*****************************************************************************/
/* 213 CM_OPC_PD_PRM_END                                                     */
/**************************************************************************//**
@defgroup CM_OPC_PD_PRM_END 213 CM_OPC_PD_PRM_END
@ingroup cm-pd-opcodes
@{
@brief The end of the parameterization is signaled with the PDPrmEnd service.
@details
The alarms are stopped for the user as soon as the request reaches CM-PD.

The "Lists of parameterization" will be calculated, and the required lists will be written to the subcomponents.

After then the "expected" to "real" comparison is activated.

This request is confirmed directly after the re-parameterization is initiated.

The end of the re-parameterization is signaled with PDPrmApplReady.ind. This indication contains also the initial difference from the expected-to-real comparison.


If an error is confirmed, the user was not owner of the PDev.

Refer also to sequence diagram: @ref cm-pd-prm-begin-sequence-diagram.

With "is_master_project == CM_IS_MASTER_PROJECT_YES" the special record PDMasterTailorData was written to the given parameterization.
Typically den PDev is not ready yet (see @ref cm-pd-channels-series-machines-readiness) and the user has to expect a sequence of at least two PrmApplReady.ind. The first with response CM_OK_ADAPTION_NEEDED, the second with CM_OK

The complete sequence is described in chapter @ref cm-pd-channels-series-machines.

## Parallel operation of channel specific services
Refer to @ref CM_OPC_PD_CREATE.

## Record-lists required by parameterization

See also TIA# 982509.

PDPrmEnd uses four record lists for parameterization.

### Given parameterization
If no SendClock record was written to the "Given parameterization", PDPrmEnd add the default SendClock record as described in PDPrmWrite.

If no MRP record was written and PDPrmEnd.set_mrp_off is CM_SET_MRP_OFF_YES, then the PDInterfaceMrpDataAdjust record with MRPRole = off is added.

### Remanent parameterization
If any record has been stored in a prior parameterization to the remanent parameterization list, and the new parameterization is given remotely (see CM-SV), the contents of the two parameterization lists will be merged.

After then PDPrmEnd calculate the record for the remaining two lists.

### Empty parameterization
This list consists only of the SendClock record.
SendClockFactor and SendClockProperties are the same as in the "given parameterization"

### Default parameterization
This list consists only the default SendClock record as described in PDPrmWrite for "startup", see @ref cm-pd-prm-write-send-clock-record.

The currently activated parameterization is signaled with the PDPrmApplReady.ind, see @ref CM_OPC_PD_EVENT_APPL_READY_IND.

@see @ref CM_PD_PRM_RDWR_TYPE

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | The parameterization is consistent in itself and was activated.
CM_ERR_SEQUENCE    | Call sequence not as specified, e.g. not owner of the PDEV.
******************************************************************************/

#define CM_IS_MASTER_PROJECT_NO  0
#define CM_IS_MASTER_PROJECT_YES 1

#define CM_SET_MRP_OFF_NO  0
#define CM_SET_MRP_OFF_YES 1

typedef struct cm_pd_prm_end_tag {

	LSA_UINT8 set_mrp_off;
	/**< ## REQ
	 * If CM_SET_MRP_OFF_YES and no other MRP record was written, then CM-PD generates an "MRP off" record (AP00824058). For the "MRP off" record see PDInterfaceMrpDataAdjust with MRP_Role = OFF.
	 * CM_SET_MRP_OFF_NO otherwise.
	 */
	LSA_UINT8 is_master_project;
	/**< ## CNF
	 * CM_IS_MASTER_PROJECT_YES if the record PDMasterTailorData was written to the given parameterization.
	 * CM_IS_MASTER_PROJECT_NO  otherwise.
	 */

	/** @cond INTERNAL */
	struct { /** only for CM_OPC_PD_PRM_END_SERVER (HIF: do nothing) */
		LSA_VOID_PTR_TYPE void_ptr; /**< do not evaluate (used internally) */
	} sv;
	/** @endcond */

} CM_PD_PRM_END_TYPE;

typedef CM_PD_PRM_END_TYPE * CM_UPPER_PD_PRM_END_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_PRM_END. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 220 CM_OPC_PD_PTCP_MASTER_CONTROL                                         */
/**************************************************************************//**
@defgroup CM_OPC_PD_PTCP_MASTER_CONTROL 220 CM_OPC_PD_PTCP_MASTER_CONTROL
@ingroup cm-pd-opcodes
@{
@brief This service forwards the given parameters to the GSY component.
@details
It is used to request the start or stop of the PTCP time master. For more information see GSY.doc.

## Parallel operation of channel-specific services

This is a queued service within the CMPD.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | GSY returned OK.
CM_ERR_SEQUENCE    | Sequence not as specified (before PD-create, or GSY not included; see CM_CFG_USE_GSY).
CM_ERR_LOWER_LAYER | GSY reports an error, further information is found within gsy_response.
******************************************************************************/
typedef struct cm_pd_ptcp_master_control_tag
{
	LSA_UINT32 mode;
	/**< ## REQ
	 * see GSY_RQB_MASTER_CONTROL_TYPE::Mode
	 */

	LSA_UINT16 utc_offset;
	/**< ## REQ
	 * see GSY_RQB_MASTER_CONTROL_TYPE::UTCOffset
	 */

	LSA_UINT16 gsy_response;
	/**< ## CNF
	 * Valid if CM_ERR_LOWER_LAYER: the forwarded response code from GSY. For more information see GSY.doc.
	 */

} CM_PD_PTCP_MASTER_CONTROL_TYPE;

typedef CM_PD_PTCP_MASTER_CONTROL_TYPE * CM_UPPER_PD_PTCP_MASTER_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_PTCP_MASTER_CONTROL. */

typedef CM_PD_PTCP_MASTER_CONTROL_TYPE const * CM_UPPER_PD_PTCP_MASTER_CONTROL_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_PD_PTCP_MASTER_CONTROL. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 230 CM_OPC_PD_PRM_BEGIN_SERVER                                            */
/**************************************************************************//**
@internal
@defgroup CM_OPC_PD_PRM_BEGIN_SERVER 230 CM_OPC_PD_PRM_BEGIN_SERVER
@ingroup cm-pd-opcodes
@brief Internal opcode between CM-SV and CM-PD.
@see CM_OPC_PD_PRM_BEGIN
@endinternal
******************************************************************************/


/*****************************************************************************/
/* 231 CM_OPC_PD_PRM_READ_SERVER                                             */
/**************************************************************************//**
@internal
@defgroup CM_OPC_PD_PRM_READ_SERVER 231 CM_OPC_PD_PRM_READ_SERVER
@ingroup cm-pd-opcodes
@brief Internal opcode between CM-SV and CM-PD.
@see CM_OPC_PD_PRM_READ
@endinternal
******************************************************************************/


/*****************************************************************************/
/* 232 CM_OPC_PD_PRM_WRITE_SERVER                                             */
/**************************************************************************//**
@internal
@defgroup CM_OPC_PD_PRM_WRITE_SERVER 232 CM_OPC_PD_PRM_WRITE_SERVER
@ingroup cm-pd-opcodes
@brief Internal opcode between CM-SV and CM-PD.
@see CM_OPC_PD_PRM_WRITE
@endinternal
******************************************************************************/


/*****************************************************************************/
/* 233 CM_OPC_PD_PRM_END_SERVER                                              */
/**************************************************************************//**
@internal
@defgroup CM_OPC_PD_PRM_END_SERVER 233 CM_OPC_PD_PRM_END_SERVER
@ingroup cm-pd-opcodes
@brief Internal opcode between CM-SV and CM-PD.
@see CM_OPC_PD_PRM_END
@endinternal
******************************************************************************/


/*****************************************************************************/
/* 234 CM_OPC_PD_PRM_RELINQUISH_SERVER                                       */
/**************************************************************************//**
@cond INTERNAL
@defgroup CM_OPC_PD_PRM_RELINQUISH_SERVER 234 CM_OPC_PD_PRM_RELINQUISH_SERVER
@ingroup cm-pd-opcodes
@{
@brief Internal opcode between CM-SV and CM-PD.
******************************************************************************/
 typedef struct cm_pd_prm_relinquish_tag { /* internal (HIF: do nothing) */

	LSA_UINT16 partner_session_key; /**< SR-AR: partner (not down), otherwise: 0 (TIA 1892236) */

} CM_PD_PRM_RELINQUISH_TYPE;

typedef CM_PD_PRM_RELINQUISH_TYPE * CM_UPPER_PD_PRM_RELINQUISH_PTR_TYPE; /**< The pointer type used for CM_OPC_PD_PRM_RELINQUISH_SERVER. */
/** @} ***********************************************************************/
/** @endcond *****************************************************************/


/*****************************************************************************/
/* 235 CM_OPC_PD_PRM_READ_CLIENT                                             */
/**************************************************************************//**
@internal
@defgroup CM_OPC_PD_PRM_READ_CLIENT 235 CM_OPC_PD_PRM_READ_CLIENT
@ingroup cm-pd-opcodes
@brief Internal opcode between CM-CL and CM-PD.
@see CM_OPC_PD_PRM_READ
@endinternal
******************************************************************************/


/******************************************************************************
end of physical device opcodes
******************************************************************************/


/******************************************************************************
begin of client opcodes
******************************************************************************/
#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*****************************************************************************/
/* 020 CM_OPC_CL_CREATE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_CL_CREATE 020 CM_OPC_CL_CREATE
@ingroup cm-cl-opcodes
@{
@brief Configures the "client channel".
@details
The CreateClient function is used to configure the "client channel" (refer to [Channel Details](@ref CM_DETAIL_TYPE) as such.
Resources for "max_devices" devices will be allocated.

The Object UUID required for RPC is derived from the Vendor_id and Device_id (= Device_Ident_Number) as well as the instance.

CreateClient sets the initial global DataStatus of all client-providers to the value given by initial_data_status. As with ControlClient, only the DataStatus RUN or STOP is allowed.
After CreateClient, the global DataStatus of the providers may be changed with ControlClient, see @ref CM_OPC_CL_CONTROL.

## Interconnection with CM-PD:

PDCreate must execute before CreateClient.

## Comment on .alarm_reject:

The alarm is already rejected CM-Internally depending on the alarm type and alarm_reject.

The PNIOStatus of a 'rejected' alarms is set to CM_PNIO_ERR_AlarmTypeNotSupported (Refer to cm_err.h).

The AlarmType 0x0000 and the range 0x0100 to 0xFFFF is always rejected (reserved).

The user must set the corresponding bits in .alarm_reject for the IEC PAS reserved range of 0x0011 to 0x001F, if the alarm is to be rejected (refer to CM_ALARM_RESERVED_GROUP in cm_usr.h).

The 'manufacturer specific' range of 0x0020 bis 0x007F is rejected if Bit 0 is set.

The 'reserved for profiles' range of 0x0080 to 0x00FF must be rejected by the user if the alarm type is not supported (AlarmAck with cm_pnio_err = CM_PNIO_ERR_AlarmTypeNotSupported, Refer to cm_err.h).

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Client instance was created
CM_ERR_SEQUENCE    | Call sequence not as specified
CM_ERR_PARAM       | Parameter out of-range
CM_ERR_LOWER_LAYER | "Register" in RPC failed
******************************************************************************/

#define CM_CL_MAX_DEVICE_NR 2047 /**< max StationNumber from ES */

#define CM_IOXS_REQUIRED_NO  0
#define CM_IOXS_REQUIRED_YES 1

typedef struct cm_cl_create_tag {

	LSA_UINT16 max_devices;
	/**< ## REQ
	 * Max. number of managing devices. \n
	 * Value range: 1 .. CM_CFG_MAX_CL_DEVICES
	 */

	LSA_UINT8 annotation[63+1];
	/**< ## REQ
	 * Zero-terminated string that is entered in RPC Endpoint Mapper. \n
	 * Refer to "sprintf" format string CLRPC_PNIO_ANNOTATION_FORMAT.
	 */

	LSA_UINT16 vendor_id;
	/**< ## REQ
	 * Manufacturer ID \n
	 * EPM: for object uuid of IO controller
	 */

	LSA_UINT16 device_id;
	/**< ## REQ
	 * Device ID \n
	 * EPM: for object uuid of IO controller
	 */


	LSA_UINT16 instance;
	/**< ## REQ
	 * Device instance \n
	 * EPM: for object uuid of IO controller
	 */

	LSA_UINT32 alarm_reject;
	/**< ## REQ
	 * CM acknowledges the marked alarms Internally and does not forward them to the CM. \n
	 * Bit 0 .."manufacturer specific" alarm \n
	 * Bit 1 - 31 .. (1 << CM_ALARM_TYPE_xxx) \n
	 * Refer also to CM_ALARM_MANUFACTURER_SPECIFIC_GROUP and CM_ALARM_RESERVED_GROUP.
	 */

	LSA_UINT16 initial_data_status;
	/**< ## REQ
	 * The initial global datastatus of providers. Only CM_CL_CONTROL_APDU_STATUS_RUN and CM_CL_CONTROL_APDU_STATUS_STOP is allowed. \n
	 * Refer to cm_cl_control_apdu_status_enum and @ref CM_OPC_CL_CONTROL
	 */

	LSA_UINT8 IOXS_Required;
	/**< ## REQ
	 * CM_IOXS_REQUIRED_YES .. SubmoduleProperties.DiscardIOXS is not supported \n
	 * CM_IOXS_REQUIRED_NO ... SubmoduleProperties.DiscardIOXS is supported \n
	 * corresponds to the MDD key "IOXS_Required" (V6.0i2.1x, TIA 1218229)
	 */

	LSA_UINT32 hardware_type;
	/**< ## CNF
	 * See EDD_RQB_GET_PARAMS_TYPE::HardwareType
	 */

	LSA_UINT16 interface_id;
	/**< ## CNF
	 * 1 .. 16 \n
	 * see EDD_RQB_GET_PARAMS_TYPE::InterfaceID
	 */

} CM_CL_CREATE_TYPE;

typedef CM_CL_CREATE_TYPE * CM_UPPER_CL_CREATE_PTR_TYPE; /** The pointer type used CM_OPC_CL_CREATE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 021 CM_OPC_CL_DELETE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_CL_DELETE 021 CM_OPC_CL_DELETE
@ingroup cm-cl-opcodes
@{
@brief Deletes a CM client instance.
@details
All devices must have been removed in advance before a CM client instance can be deleted.

The confirmation is delayed, until the outstanding ControlClient.cnf and all outstanding ReadRecordImplizit.cnf are done.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Client instance was cancelled.
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_PARAM       | Parameter out of-range
CM_ERR_LOWER_LAYER | "Unregister" in RPC failed
******************************************************************************/
typedef LSA_VOID * CM_UPPER_CL_DELETE_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_DELETE, no parameters */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 022 CM_OPC_CL_CONTROL                                                     */
/**************************************************************************//**
@defgroup CM_OPC_CL_CONTROL 022 CM_OPC_CL_CONTROL
@ingroup cm-cl-opcodes
@{
@brief With CM_CL_CONTROL_NOS_ASSIGNMENT_ON and _OFF the assignment of the NameOfStation, done by an AR startup, can be adjusted.
@details
CM_CL_CONTROL_NOS_ASSIGNMENT_ON is the default state. This is set by CreateClient(). \n
In conjunction with the nare_properties and the device_alias_list given by AddDevice() the necessary dcp-set commands will be given (_ON) or not (_OFF).

After the confirmation of this request, the next AR startup behaves like adjusted.

## Parallel operation:
At any given time, only one ControlClient() service can run.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | Ok.
CM_ERR_SEQUENCE    | Only one RQB can handled at any time.
CM_ERR_ALLOC_LOWER | Could not allocate lower memory
CM_ERR_LOWER_Layer | An error occurred in the lower layer
CM_ERR_PARAM       | Parameter out of-range
CM_ERR_NOT_YET     | 0xFE, lsa-components may use 0xc0 - 0xfe
******************************************************************************/
enum cm_cl_control_enum {
	CM_CL_CONTROL_PASSIVATE = 0,
	CM_CL_CONTROL_ACTIVATE,
	CM_CL_CONTROL_APDU_STATUS,
	CM_CL_CONTROL_NOS_ASSIGNMENT_ON,
	CM_CL_CONTROL_NOS_ASSIGNMENT_OFF
};

/** default apdu_tag after create:
* (CM_CL_CONTROL_APDU_STATUS_RUN)
*
* default nos_assignment after create:
* CM_CL_CONTROL_NOS_ASSIGNMENT_ON
*/
enum cm_cl_control_apdu_status_enum {
	CM_CL_CONTROL_APDU_STATUS_STOP = 0x0001,
	CM_CL_CONTROL_APDU_STATUS_RUN = 0x0002
};

typedef struct cm_cl_control_tag {

	LSA_UINT16 control_tag;
	/**< ## REQ
	 * CM_CL_CONTROL_APDU_STATUS \n
	 * Changes the global data status, according to cm_cl_control_apdu_status_enum.
	 *
	 * CM_CL_CONTROL_PASSIVATE \n
	 * CM_CL_CONTROL_ACTIVATE \n
	 * => CM_ERR_NOT_YET \n
	 * An "activate / passivate all devices" must be done by CM user through CM_OPC_CL_DEVICE_CONTROL requests.
	 *
	 * CM_CL_CONTROL_NOS_ASSIGNMENT_OFF \n
	 * CM_CL_CONTROL_NOS_ASSIGNMENT_ON \n
	 * Adjust the NOS-assignment of the AR startups.
	 */

	LSA_UINT16 apdu_status;
	/**< ## REQ
	 * Refer to cm_cl_control_apdu_status_enum
	 */

} CM_CL_CONTROL_TYPE;

typedef CM_CL_CONTROL_TYPE * CM_UPPER_CL_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_CONTROL, no parameters */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 023 CM_OPC_CL_DEVICE_ADD                                                  */
/**************************************************************************//**
@defgroup CM_OPC_CL_DEVICE_ADD 023 CM_OPC_CL_DEVICE_ADD
@ingroup cm-cl-opcodes
@{
@brief A remote device is transferred to the CM client with the AddDevice function.
@details
CM includes helper-functions that assist in building the ARCB. See @ref CM_ARCB_TYPE

The PDEV must already have been parameterized before AddDevice (otherwise: ERR_SEQUENCE). That means, the send clock must be known.
If the PDEV SendClock is not equal to the IOCR SendClock (CM_ARCB_IOCR_TYPE::send_clock), then the "local adaption" must be used (CM_ARCB_ADD_TYPE::iocr_local_list).

The user allocates a device_nr for each remote IO device and addresses the remote IO device in the following via this device handle.

Only one AR can be added for each device.

The valid range of the device_nr lies between 1 and CM_CFG_MAX_CL_DEVICES. The count of the available resources is given by CM_CL_CREATE::max_devices.

The device handle implies the addressing elements.\n
DCP Name / DNS Name / IP Address / MAC Address => is described in "dev_addr".\n
UDP port of the endpoint mapper => specified by PNIO \n
"IPNIO_Device" Interface UUID and Version => Specified by PNIO. \n
Device Object UUID / Device_Ident_Number / Instance => is described in "dev_rpc"

The call of AddDevice allocates all required resources and confirms immediately.
The CM-user must provide as many event- and alarm-resources, as noted in the confirmation with nr_of_event_resources, nr_of_alarm_resources_low and nr_of_alarm_resources_high.
These resources must be provided to the device (NOTE: until v5.0i4.xx these resources must be provided to the client channel).
After providing all resources, the CM-user may call ControlDevice(activate) for connection startup.
The type of connection startup is determined from the number of ARs and the individual "ar_properties".

For the IOCR Classes 1 and 2, the determined "frame_ids" are returned in the CM_ARCB_IOCR_TYPE.

The transfer order of the PRM records is defined in [/10/](@ref cm-ref-no-10). \n
If CM_CL_DEVICE_PROPERTIES_MULTIPLE_WRITE is defined, then the PRM records are transferred with an IODWriteMultipleReq service.

The smallest meaningful value of "max_supported_record_length" (4152) results from the CLRPC 'must-receive-fragment-size' and the requirement of supporting at least 3 fragments.

'must-receive-fragment size' == 1464  // without Ethernet / SNAP / IP / UDP Header \n
1464 - 80 = 1384 // RPCHeader is not submitted to the user. \n
1384 * 3 = 4152 // There should be 3 fragments \n
=> NDRDataRequest should be 4152 bytes in size.

=> PROFINETIOServiceReqPDU should be (4152-20 = 4132) bytes in size. \n
=> The largest write or read record (RecordDataWrite / RecordDataRead) is then (4132 - 64 = 4068) bytes in size.

If a COC access point is defined within the SubmoduleProperties table, see @ref CM_ARCB_SUBMODULE_PROPERTIES_TYPE, all PRM records of this COC group are combined to one or more CombinedObjectContailer records.
Additionaly, bit CM_AR_PROP_COMBINED_OBJECT_CONTAINER in "ar_properties" is set CM internally for CM_OPC_SV_AR_CONNECT_IND to indicate the usage of COC. Engineering must not set CM_AR_PROP_COMBINED_OBJECT_CONTAINER.
The COC record is subsequently transfered instead of the individual PRM records (also transferred within a IODWriteMultipleReq).

'is_master_project' is set to CM_IS_MASTER_PROJECT_YES, if the records PDMasterTailorData has given to this device. This record has special meaning, and is not transferred to the remote device.
Instead it is used, to 'tailor' some other records of the device.
See chapter @ref cm-pd-channels-series-machines. This means, the AR-Abort.ind (or AR-XxxOffline.ind) delivers additional ErrorCauses, see [/18/](@ref cm-ref-no-18), see ErrorCauses starting with "CM_ERROR_CAUSE_MP_".

Cross reference: \n
In the details pointer of CM-SV, the NDRDataRequest length must be specified with detailptr.u.sv.alloc_len = 4152.

The ARCB is subjected to a feasibility test before the CNF. Refer also to [/11/](@ref cm-ref-no-11). An invalid ARCBs is rejected with CM_ERR_PARAM, the "pnio_status" is set additionally.

@note The SendClockFactor of the AR must match, or must have a local adaption to, the PDSendClock record of the PDEV-channel.

@note Current count of event resources is calculated as follows:
- 2 event resources for InData.ind and Abort.ind or Offline.ind or PreOffline.ind.
- 1 event resource for PreData.ind if ARtype is RTClass3 or AR has advanced startup.
- 1 event resource for DataStatus.ind if appointed with run_mode.
- 1 event resource for PrmBegin.ind or PrmInData.ind if ARtype is SYSRED.

@note Current count of alarm resources is calculated as follows:
- 1 alarm resource for alarm priority low.
- 1 alarm resource for alarm priority high if property CM_ALCR_PROP_PRIO_ONLY_LOW is not set.

@note For partial data access and buffer size, refer to chapter 4.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | Device was added.
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_RESOURCE    | The lower layer does not have any cyclical resources (e.g. compiled with "NO SRT").
CM_ERR_PARAM       | Parameter wrong (e.g. out-of-range), see pnio_status for details
CM_ERR_ALLOC_LOCAL | Not adequate local memory
******************************************************************************/
/* see engineering tool "DeviceMode" */
#define CM_CL_DEVICE_MODE_SCAN_CYCLE_10MS_MASK     0x000000FF /**< scan-cycle for (fsu-)devices: 10..200 * 10ms */

#define CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_MASK  0x00000100 /**< see DefaultGateway */
#define CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_NO    0x00000000
#define CM_CL_DEVICE_MODE_USE_DEFAULT_ROUTER_YES   0x00000100 /**< use individual default router for the device */

#define CM_CL_DEVICE_MODE_IS_FSU_MASK              0x00000200 /**< see "Establishing of an AR (fast startup procedure)" aka FSU */
#define CM_CL_DEVICE_MODE_IS_FSU_NO                0x00000000
#define CM_CL_DEVICE_MODE_IS_FSU_YES               0x00000200

#define CM_CL_DEVICE_MODE_IS_ACCELERATED_MASK      0x00000400 /**< see "Establishing of an AR (accelerated procedure)" aka ASU */
#define CM_CL_DEVICE_MODE_IS_ACCELERATED_NO        0x00000000
#define CM_CL_DEVICE_MODE_IS_ACCELERATED_YES       0x00000400

#define CM_CL_DEVICE_MODE_OPTIMIZED_IP_CHECK_MASK  0x00000800
#define CM_CL_DEVICE_MODE_OPTIMIZED_IP_CHECK_NO    0x00000000 /**< check uniqueness of ip-addr (if known) */
#define CM_CL_DEVICE_MODE_OPTIMIZED_IP_CHECK_YES   0x00000800 /**< check uniqueness of ip-addr only with set-ip-addr */

#define CM_CL_DEVICE_MODE_CHECK_DEVICE_ID_MASK     0x00001000 /**< AP01148513 */
#define CM_CL_DEVICE_MODE_CHECK_DEVICE_ID_NO       0x00000000 /**< legacy, do not check xxx_id, set NoS if device-NoS is empty */
#define CM_CL_DEVICE_MODE_CHECK_DEVICE_ID_YES      0x00001000 /**< check vendor_id, device_id and instance_id prior to set NoS */

#define CM_CL_DEVICE_MODE_NOS_OVERWRITE_MASK       0x00002000 /**< AP01569103 */
#define CM_CL_DEVICE_MODE_NOS_OVERWRITE_NO         0x00000000 /**< legacy, set NoS if device-NoS is empty */
#define CM_CL_DEVICE_MODE_NOS_OVERWRITE_YES        0x00002000 /**< set NoS of device found at topological position */

#define CM_CL_DEVICE_MODE_RESERVED_MASK            0xFFFFC000 /**< these bits must be 0 */

/* see engineering tool "DeviceProperties" */
#define CM_CL_DEVICE_PROPERTIES_MULTIPLE_WRITE          0x00000001 /**< 1: Multiple Write Supported */
#define CM_CL_DEVICE_PROPERTIES_SYSTEM_REDUNDANCY_MASK  0x00000006 /**< ignored */
#define CM_CL_DEVICE_PROPERTIES_CIR_MASK                0x00000008 /**< ignored */
#define CM_CL_DEVICE_PROPERTIES_CAN_SRD_BACKUP_ALARM    0x00000010 /**< 1: IOC is allowed to send alarm ack (CM_PNIO_ERR_AlarmAck_IOCARSR_backup) */
#define CM_CL_DEVICE_PROPERTIES_RESERVED_MASK           0xFFFFFFE0 /**< 5-31 reserved */

/* RunMode */
#define CM_CL_RMODE_DISABLE_DATA_STATUS_IND  0x00000040
#define CM_CL_RMODE_RESERVED_MASK            0xFFFFFFBF /**< these bits must be 0 */

typedef struct cm_cl_device_add_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * Device number \n
	 * local identification of this IO-Device; 1..CM_CFG_MAX_CL_DEVICES
	 */

	LSA_UINT32 device_mode;
	/**< ## REQ
	 * Enhanced settings of the device (given by the planning tool):
	 *
	 * CM_CL_DEVICE_MODE_SCAN_CYCLE_10MS_MASK: \n
	 * ASU-devices: which scan cycle is to be used (100ms to 2sec). \n
	 * 0 .. use value calculated from DetailPtr::u.cl.min_reconnect_timeout.
	 *
	 * CM_CL_DEVICE_MODE_USE_GATEWAY_MASK: \n
	 * Only CM_CL_DEVICE_MODE_USE_GATEWAY_NO is allowed
	 *
	 * CM_CL_DEVICE_MODE_IS_FSU_MASK (aka FSU-device): \n
	 * If set to YES, the optimised startup procedure will be used. \n
	 * Maximally CM_CFG_MAX_CL_FSU_DEVICES FSU-devices can be added.
	 *
	 * CM_CL_DEVICE_MODE_IS_ACCELERATED_MASK (aka ASU-device): \n
	 * If set to YES, the the accelerated startup procedure will be used.
	 *
	 * CM_CL_DEVICE_MODE_OPTIMIZED_IP_CHECK_MASK: \n
	 * If NO, as soon as the ip-address of the device is known, it will be tested for uniqueness.\n
	 * If YES, the uniqueness is only tested, when the IP-address shall be set.
	 *
	 * CM_CL_DEVICE_MODE_CHECK_DEVICE_ID_MASK: \n
	 * Legacy devices uses NO. \n
	 * YES: The remote device_id is checked against the given device_id. \n
	 * The startup of the device is rejected with ErrorCause 16, if the device_id is different (also vendor_id or instance).
	 *
	 * CM_CL_DEVICE_MODE_NOS_OVERWRITE_MASK:
	 * If the device is found via the alias-list and the property is set to
	 * NO, then the given NoS is set to the device only if the remote NoS is empty
	 * YES, then the given NoS is set to the device if the remote NoS differs from the given.
	 */

	LSA_UINT32 device_properties;
	/**< ## REQ
	 * - CM_CL_DEVICE_PROPERTIES_MULTIPLE_WRITE
	 * - CM_CL_DEVICE_PROPERTIES_SYSTEM_REDUNDANCY_MASK
	 * - CM_CL_DEVICE_PROPERTIES_CIR_MASK
	 * - CM_CL_DEVICE_PROPERTIES_CAN_SRD_BACKUP_ALARM \n
	 *   only allowed for AR_TYPE CM_AR_TYPE_SINGLE_SYSRED
	 * - CM_CL_DEVICE_PROPERTIES_RESERVED_MASK
	 */

	LSA_UINT32 run_mode;
	/**< ## REQ
	 * Enhanced settings of the device (given by the application):
	 *
	 * CM_CL_RMODE_DISABLE_DATA_STATUS_IND \n
	 * This mode disables the DataStatus.ind. The nr_of_event_resources will adjusted according this mode.
	 */

	LSA_UINT16 cmi_activity_timeout100ms;
	/**< ## REQ
	 * CM Initiator activity timeout; base: 100ms\n
	 * Timeout value of the IO device for monitoring controller inactivity.
	 */

	CM_ARCB_NARE_TYPE dev_addr;
	/**< address info of this device.
	 * Refer to @ref CM_ARCB_NARE_TYPE
	 */

	CM_ARCB_RPC_TYPE dev_rpc;
	/**< various RPC info.
	 * Refer to @ref CM_ARCB_RPC_TYPE.
	 */

	CM_UPPER_ARCB_ADD_PTR_TYPE arcb;
	/**< ## REQ
	 * Refer to @ref CM_ARCB_ADD_TYPE.
	 */

	LSA_UINT32 pnio_status;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * In case of a CM_ERR_PARAM error, A PNIO confirming description of the missing location is contained here, provided the error was found within an AR.
	 */

	LSA_UINT16 nr_of_event_resources;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * Required event-resources the CM-user must provide with CM_OPC_CL_DEVICE_PROVIDE_EVENT to the device.
	 */

	LSA_UINT16 nr_of_alarm_resources;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * Required alarm-resources the CM-user must provide with CM_OPC_CL_DEVICE_PROVIDE_ALARM to the device.
	 */

	LSA_UINT8 is_master_project;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * CM_IS_MASTER_PROJECT_YES if the record PDMasterTailorData is given to this device.
	 * CM_IS_MASTER_PROJECT_NO  otherwise.
	 */

	LSA_UINT32 device_ip;
	/**< ## REQ
	 * Don't care
	 * ## CNF
	 * Same ip-address as described in service @ref CM_OPC_CL_AR_ABORT_IND
	 */

} CM_CL_DEVICE_ADD_TYPE;

typedef CM_CL_DEVICE_ADD_TYPE * CM_UPPER_CL_DEVICE_ADD_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_DEVICE_ADD */

typedef CM_CL_DEVICE_ADD_TYPE const * CM_UPPER_CL_DEVICE_ADD_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_CL_DEVICE_ADD */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 024 CM_OPC_CL_DEVICE_REMOVE                                               */
/**************************************************************************//**
@defgroup CM_OPC_CL_DEVICE_REMOVE 024 CM_OPC_CL_DEVICE_REMOVE
@ingroup cm-cl-opcodes
@{
@brief The device and all the ARs in it are deleted with the help of RemoveDevice.
@details
A device must not be passivated to be removed.

RemoveDevice cancels running or created ARs. This happens on the one hand through the withdrawal of the IO CR (provider / consumer) and on the other hand through "cancellation" (refer to [/3/](@ref cm-ref-no-3)) of a running RPC.
RPC indications that barely arrive before the "RPC Unregister" are "rejected" (Refer to [/3/](@ref cm-ref-no-3)).

The network interaction "RTA-Error" can be observed.

An organized shutdown (with the help of "RPC-Release") can be achieved through DeviceControl (passivate).

@note RemoveDevice does not send an RPC_D_Release.

The confirmation of RemoveDevice is delayed until all outstanding event- or alarm- indications for this device are responded.
Prior to the confirmation the RQB-list of the device is released with CM_OK_CANCEL, see CM_OPC_CL_DEVICE_PROVIDE_EVENT and CM_OPC_CL_DEVICE_PROVIDE_ALARM.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | Device was deleted.
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_PARAM       | Parameter wrong (e.g. out-of-range)
******************************************************************************/
typedef struct cm_cl_device_remove_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * The device to be removed.
	 */

} CM_CL_DEVICE_REMOVE_TYPE;

typedef CM_CL_DEVICE_REMOVE_TYPE * CM_UPPER_CL_DEVICE_REMOVE_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_DEVICE_REMOVE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 025 CM_OPC_CL_DEVICE_CONTROL                                              */
/**************************************************************************//**
@defgroup CM_OPC_CL_DEVICE_CONTROL 025 CM_OPC_CL_DEVICE_CONTROL
@ingroup cm-cl-opcodes
@{
@brief The ControlDevice function can be used to activate, passivate or abort the devices.
Furthermore ControlDevice can set the data status of the ARs.
@details
The activation sets the state to active and starts the AR startup procedure. The call is closed immediately.

Activation of an already active device means "do nothing".

When the device should activate and there are not enough event- or alarm-resources assigned to the device, this service is confirmed with CM_ERR_SEQUENCE.

A passivation sets the state to passive and stops an existing AR (with the help of "RPC-Release"). The call is not closed immediately, but only with the response of the device (or after a timeout).
The network interaction "RPC-Release" followed by an "RTA-Error" can be observed.

Passivation of an already passive device means "do nothing".

With "abort" a running AR can be shut down. \n
In state "active" the network interaction "RTA-Error" followed by a startup sequence can be observed. \n
In state "passive" this is a "do nothing".

With CM_CL_DEVICE_CONTROL_APDU_STATUS the data status of an active AR can be set to STATION_OK or STATION_PROBLEM. See also @ref CM_OPC_SV_AR_DATA_STATUS_IND.

<b> Interactions of DevicePassivate/DeviceRemove and CM_OPC_CL_AR_OFFLINE_IND: </b>
CM_OPC_CL_AR_OFFLINE_IND is send as fast as possible, but after passivating the consumer- and provider-frames.

<b> Interactions of the device state and CM_OPC_CL_AR_OFFLINE_IND </b>
The user may delay the Offline.rsp for an arbitrary time. With the response the state determines the further behaviour. For details see the client state diagrams.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | A new control (e.g. activate) cancelled this control (e.g. passivate).
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_PARAM       | Wrong parameter (e.g. out-of-range)
******************************************************************************/
#define CM_CL_DEVICE_CONTROL_PASSIVATE    1
#define CM_CL_DEVICE_CONTROL_ACTIVATE     2
#define CM_CL_DEVICE_CONTROL_APDU_STATUS  3
#define CM_CL_DEVICE_CONTROL_ABORT        4 /**< AR abort reason CM_AR_REASON_ABORT */
#define CM_CL_DEVICE_CONTROL_ABORT_MTOT   5 /**< AR abort reason CM_AR_REASON_MTOT */

/** values can be or-ed together! */
enum cm_cl_ar_control_enum {
#if 0/*see SYSRED and eddi_BufferRequest_xxx*/
	CM_CL_AR_CONTROL_PRIMARY         = 0x0001,
	CM_CL_AR_CONTROL_BACKUP          = 0x0002,
	CM_CL_AR_CONTROL_RUN             = 0x0004,
	CM_CL_AR_CONTROL_STOP            = 0x0008,
#endif
	CM_CL_AR_CONTROL_STATION_OK      = 0x0010,
	CM_CL_AR_CONTROL_STATION_PROBLEM = 0x0020
};

typedef struct cm_cl_device_control_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * Device Number, identification of this IOD
	 */

	LSA_UINT16 control_tag;
	/**< ## REQ
	 * Refer to CM_CL_DEVICE_CONTROL_xxx
	 */

	LSA_UINT16 apdu_status;
	/**< ## REQ
	 * Valid, if control_tag = CM_CL_DEVICE_CONTROL_APDU_STATUS. Refer to cm_cl_ar_control_enum
	 */

} CM_CL_DEVICE_CONTROL_TYPE;

typedef CM_CL_DEVICE_CONTROL_TYPE * CM_UPPER_CL_DEVICE_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_DEVICE_CONTROL */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_CL_RECORD_TYPE                                                         */
/**************************************************************************//**
@defgroup cm-cl-records CM CL Records
@ingroup cm-records
@{
@brief CM_CL_RECORD_TYPE is used for @ref CM_OPC_CL_RECORD_READ, @ref CM_OPC_CL_RECORD_READ_QUERY, @ref CM_OPC_CL_RECORD_WRITE, @ref CM_OPC_CL_RECORD_WRITE_MULTIPLE.
******************************************************************************/
#if CM_RECORD_HEADER_SIZE != (64U) || CM_RECORD_OFFSET != (CLRPC_PNIO_OFFSET + CM_RECORD_HEADER_SIZE)
#error "check this! see definition above"
#endif

typedef struct cm_cl_record_tag {

	CM_LIST_TYPED_ENTRY_TYPE(cm_record_tag) link;
	/**< only CM_OPC_CL_RECORD_WRITE_MULTIPLE */

	LSA_UINT32 api;
	/**< ## REQ
	 * record addressing
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * record addressing
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * record addressing
	 */

	LSA_UINT16 record_index;
	/**< ## REQ
	 * record addressing
	 */

	LSA_UINT16 sequence_nr;
	/**< ## REQ
	 * sequencing (common for read and write!), see too flag "via_CombinedObjectContainer"
	 */

	LSA_UINT32 data_length;
	/**< ## Read-REQ
	 * size of 'data' (including CM_RECORD_OFFSET)
	 * ## Read-CNF
	 * CM_RECORD_OFFSET + number of bytes read \n
	 * ## Write-REQ
	 * CM_RECORD_OFFSET + number of bytes to write \n
	 * ## Write-CNF
	 * CM_RECORD_OFFSET + number of bytes written
	 */

	CM_COMMON_MEM_U8_PTR_TYPE data;
	/**< The CLRPC buffer, user-data starts at offset CM_RECORD_OFFSET. The headers are not hidden to "see" the alignment of the user-data.
	 * ## Read-REQ
	 * "empty" buffer
	 * ## Read-CNF
	 * read data stored at offset CM_RECORD_OFFSET
	 * ## Write-REQ
	 * data-to-write at offset CM_RECORD_OFFSET
	 * ## Write-CNF
	 * "unchanged"
	 */

	LSA_UINT32 cm_pnio_err;
	/**< ## CNF
	 * see the macro CM_PNIO_ERR_MAKE() in file cm_err.h
	 */

	LSA_UINT16 ret_val_1;
	/**< ## CNF
	 * return value 1
	 */

	LSA_UINT16 ret_val_2;
	/**< ## CNF
	 * return value 2
	 */

	LSA_UINT32 readquery_length;
	/**< ## Read-REQ
	 * size of 'RecordDataReadQuery' (see PNIO-Spec) at offset CM_RECORD_OFFSET
	 * ## Write-REQ
	 * don't care
	 */

} CM_CL_RECORD_TYPE;

typedef CM_CL_RECORD_TYPE * CM_UPPER_CM_CL_RECORD_PTR_TYPE; /**< The pointer type used for @ref CM_OPC_CL_RECORD_READ, @ref CM_OPC_CL_RECORD_READ_QUERY, @ref CM_OPC_CL_RECORD_WRITE, @ref CM_OPC_CL_RECORD_WRITE_MULTIPLE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 026 CM_OPC_CL_RECORD_READ                                                 */
/**************************************************************************//**
@defgroup CM_OPC_CL_RECORD_READ 026 CM_OPC_CL_RECORD_READ
@ingroup cm-cl-opcodes
@{
@brief The ReadRecord function can be used to read a record from an IOD which has been made known to the controller via CM_OPC_CL_DEVICE_ADD.
@details
For details of the record indices, refer to chapter @ref cm-appendix-c.

The call is closed after the response of the device or after a timeout.

The user of this service gets the status defined in the PROFINET IO as result in cm_pnio_err. Additionally, the IO device can return specific error codes with Ret_Val_1 and Ret_Val_2.

If an AR has been established RPCOperationNmb 2 (Read) is used for reading records from the remote device, elsewise the read from the device is executed with RPC service RPCOperationNmb 5 (Read Implicit).

If the engineering system has configured an IP address, the controller has calculated an IP address for a device in multi deployable configurations or a previous name resolution has been successful, no DCP identify is issued for finding the device as long as no read times out with CM_ERR_TIMEOUT.

If a call fails with CM_ERR_TIMEOUT, all pending read requests with index <0xF000 will terminate with CM_ERR_TIMEOUT.

If an AR is aborted, all pending read requests with index <0xF000 will terminate with CM_ERR_ABORT.

Read requests failing with CM_ERR_LOWER_RPC will not cancel all other pending read requests. Subsequently reads for indices < 0xF000 will be returned with CM_ERR_ABORT since an INDATA AR will be aborted.

For reads to indices >=0xF000 a failure will only terminate the active call. All other pending calls will be executed as soon as possible.

Reading a record without setting an IP suite to the controller will fail with CM_ERR_SEQUENCE.
A read from a device that has never been activated and not been named before will fail with CM_ERR_LOWER_NARE.

In the context of series and modular machines, reading of records is not advised, as long as the tailoring has not been completed (i.e. setting an IP suite and name or writing PDInstanceTailorData).

@cond INTERNAL
Internal: For the underlying Read Implicit RPC call the AR UUID is set to NIL, and the Target AR UUID is set to the AR UUID of the added device. The name resolution is executed with the dev_addr provided with device_add.
@endcond

## Result of the request

Return Code             | Meaning
------------------------|--------------------------------------
CM_OK                   | Read from the RPC view was successful. Data buffer contains valid data. cm_pnio_err is valid.
CM_OK_CANCELLED         | A pending read request was cancelled because of a device remove.
CM_ERR_ABORT            | Only for record indices <0xF000: The read was cancelled because the AR is aborted and the device is currently not readable.
CM_ERR_PARAM            | Parameters are invalid (e.g., out-of-range, device not added).
CM_ERR_SEQUENCE         | Controller has got no valid IP suite or device add or remove is in progress.
CM_ERR_SESSION          | Only for record indices <0xF000: Records with an index <0xF000 can not be read from passive devices.
CM_ERR_TIMEOUT          | RPC has reported a timeout error.
CM_ERR_LOWER_NARE       | NARE has reported an error.
CM_ERR_LOWER_RPC        | CLRPC has reported an error.
******************************************************************************/
typedef struct cm_cl_record_read_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * Device Number, identification of this IOD
	 */

	CM_CL_RECORD_TYPE rec;
	/**<
	 * @see @ref CM_CL_RECORD_TYPE
	 * @param CM_CL_RECORD_TYPE::link              **REQ** Don't care.
	 * @param CM_CL_RECORD_TYPE::api               **REQ** Application process identifier
	 * @param CM_CL_RECORD_TYPE::slot_nr           **REQ** Slot Number
	 * @param CM_CL_RECORD_TYPE::subslot_nr        **REQ** Subslot Number
	 * @param CM_CL_RECORD_TYPE::record_index      **REQ** Record Index
	 * @param CM_CL_RECORD_TYPE::sequence_nr       **REQ** Don't care
	 * @param CM_CL_RECORD_TYPE::data_length       **REQ** CM_RECORD_OFFSET + number of bytes to read \n **CNF** CM_RECORD_OFFSET + number of bytes read
	 * @param CM_CL_RECORD_TYPE::data              **REQ** Pointer to the data buffer \n The allocated buffer size must be equal to (CM_RECORD_OFFSET + number of bytes to be read). \n **CNF** The read data are in the buffer from offset CM_RECORD_OFFSET.
	 * @param CM_CL_RECORD_TYPE::cm_pnio_err       **CNF** Refer to chapter @ref cm-appendix-c and the macro CM_PNIO_ERR_MAKE() in file cm_err.h
	 * @param CM_CL_RECORD_TYPE::ret_val_1         **CNF** Return value 1, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
	 * @param CM_CL_RECORD_TYPE::ret_val_2         **CNF** Return value 2, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
	 * @param CM_CL_RECORD_TYPE::readquery_length  **REQ** Don't care \n **CNF** 0 (field is internally used by CM)
	 */

} CM_CL_RECORD_READ_TYPE;

typedef CM_CL_RECORD_READ_TYPE * CM_UPPER_CL_RECORD_READ_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_RECORD_READ */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 027 CM_OPC_CL_RECORD_READ_QUERY                                           */
/**************************************************************************//**
@defgroup CM_OPC_CL_RECORD_READ_QUERY 027 CM_OPC_CL_RECORD_READ_QUERY
@ingroup cm-cl-opcodes
@brief The ReadRecordQuery can be used to read a record of an IOD.
@details
The RecordDataReadQueryblock (refer to [/11/](@ref cm-ref-no-11) is transferred to the IOD in ReadRequest.

For all not mentioned parameters, please refer to @ref CM_OPC_CL_RECORD_READ.
@param CM_CL_RECORD_TYPE::readquery_length  **REQ** Length of the RecordDataReadQuery (see [/11/](@ref cm-ref-no-11)) block. (i.e. 2+2 + RecordDataReadQuery::BlockLength) \n
													The block has to be written into the buffer starting at offset CM_RECORD_OFFSET and thus "data_length" has to be >= CM_RECORD_OFFSET + readquery_length.
******************************************************************************/


/*****************************************************************************/
/* CM_OPC_CL_RECORD_READ_IMPLICIT                                            */
/**************************************************************************//**
@defgroup CM_OPC_CL_RECORD_READ_IMPLICIT XXX CM_OPC_CL_RECORD_READ_IMPLICIT
@ingroup cm-cl-opcodes
@brief Removed
@deprecated Removed with version 7.0i1.32
******************************************************************************/


/*****************************************************************************/
/* CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY                                      */
/**************************************************************************//**
@defgroup CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY XXX CM_OPC_CL_RECORD_READ_IMPLICIT_QUERY
@ingroup cm-cl-opcodes
@brief Removed
@deprecated Removed with version 7.0i1.32
******************************************************************************/


/*****************************************************************************/
/* 030 CM_OPC_CL_RECORD_WRITE                                                */
/**************************************************************************//**
@defgroup CM_OPC_CL_RECORD_WRITE 030 CM_OPC_CL_RECORD_WRITE
@ingroup cm-cl-opcodes
@{
@brief A record can be written in an IOD that has an AR, by the function WriteRecord.
@details
For details of the record indices, refer to chapter @ref cm-appendix-c.

The call is not terminated immediately, but after the response of the device (or after a timeout).

The user of this service receives the status defined in PROFINET IO as result in the "cm_pnio_err".
Additionally, the IO device return specific error codes with Ret_Val_1 and Ret_Val_2.

The AR is aborted in rqb.response == CM_ERR_LOWER_LAYER.
Further information is available in the associated CmClientAddrInfo.Ind.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | Write was successful from the RPC view. Data buffer contains valid data. Cm_pnio_err is valid.
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_PARAM       | Parameters are invalid (e.g., out-of-range).
CM_ERR_ABORT       | The RPC was cancelled, because the AR is cancelled.
CM_ERR_SESSION     | The AR was created afresh in the meantime.
CM_ERR_TIMEOUT     | RPC has reported a timeout error.
CM_ERR_COMM        | RPC has reported a communication error.
CM_ERR_BUSY        | The RPC Server of IOD signaled "busy" (i.e., the call can/should be repeated later).
CM_ERR_LOWER_LAYER | CLRPC has reported an error or the PDU could not be parsed.
******************************************************************************/
typedef struct cm_cl_record_write_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * Device Number, identification of this IOD
	 */

	LSA_UINT16 session_key;
	/**< ## REQ
	 * Session key, refer to AR_Offline.IND
	 */

	CM_CL_RECORD_TYPE rec;
	/**<
	 * @see @ref CM_CL_RECORD_TYPE
	 * @param CM_CL_RECORD_TYPE::link                      Don't care.
	 * @param CM_CL_RECORD_TYPE::api               **REQ** Application process identifier
	 * @param CM_CL_RECORD_TYPE::slot_nr           **REQ** Slot Number
	 * @param CM_CL_RECORD_TYPE::subslot_nr        **REQ** Subslot Number
	 * @param CM_CL_RECORD_TYPE::record_index      **REQ** Record Index. Index 0xE040 is not allowed.
	 * @param CM_CL_RECORD_TYPE::sequence_nr       **REQ** Don't care
	 * @param CM_CL_RECORD_TYPE::data_length       **REQ** CM_RECORD_OFFSET + number of bytes to be written \n **CNF** CM_RECORD_OFFSET + number of bytes written, not checked
	 * @note CLRPC supports on the basis of LSA compliant alloc macros up to maximum (64KB - CM_RECORD_OFFSET) use data.
	 * @param CM_CL_RECORD_TYPE::data              **REQ** Pointer to the data buffer \n
	 *                                                     The allocated buffer size must be (CM_RECORD_OFFSET + number of bytes to be written). \n
	 *                                                     The data to be written must be entered in the buffer from offset CM_RECORD_OFFSET.
	 * @param CM_CL_RECORD_TYPE::cm_pnio_err       **CNF** Refer to chapter @ref cm-appendix-c and the macro CM_PNIO_ERR_MAKE() in file cm_err.h
	 * @param CM_CL_RECORD_TYPE::ret_val_1         **CNF** Return value 1, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
	 * @param CM_CL_RECORD_TYPE::ret_val_2         **CNF** Return value 2, refer to [/11/](@ref cm-ref-no-11). Optional, return 0 if not used.
	 * @param CM_CL_RECORD_TYPE::readquery_length          Don't evaluate
	 */

} CM_CL_RECORD_WRITE_TYPE;

typedef CM_CL_RECORD_WRITE_TYPE * CM_UPPER_CL_RECORD_WRITE_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_RECORD_WRITE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 031 CM_OPC_CL_RECORD_WRITE_MULTIPLE                                       */
/**************************************************************************//**
@defgroup CM_OPC_CL_RECORD_WRITE_MULTIPLE 031 CM_OPC_CL_RECORD_WRITE_MULTIPLE
@ingroup cm-cl-opcodes
@brief The WriteRecordMultiple function can be used to write several records with a single RPC.
@details
How many records are combined in an RPC is specified by AddDevice (@ref CM_OPC_CL_DEVICE_ADD).

@attention This CM version defines only the opcode. Functionality has not yet been implemented!

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_ERR_NOT_YET     | Not implemented yet.
******************************************************************************/


/*****************************************************************************/
/* 034 CM_OPC_CL_LOGBOOK_READ                                                */
/**************************************************************************//**
@defgroup CM_OPC_CL_LOGBOOK_READ 034 CM_OPC_CL_LOGBOOK_READ
@ingroup cm-cl-opcodes
@{
@brief Each device in the CM client has a so-called log-book.
@details
The log-book is a (circulation) list with a maximum CM_CFG_MAX_CL_LOG_ENTRIES entries per device (refer to chapter @ref cm-product-components-cfg).
The circulation list is stored in the CM channel so that already removed devices can also be queries. \n
The circulation list is initialized in CmClAddDevice().

Each lbentry_type is entered once for each session_key (log-book-sequence).
The old values are overwritten with the current values in case of recurring entries, (detected from the identical session_key) (time_stamp, data_1, data_2).

A new log-book sequence starts with lbentry_type .._OFFLINE, refer also to @ref CM_OPC_CL_AR_OFFLINE_IND_RSP.

The following entries can be there for each session:

lbentry_type      | data_1                  | data_2
------------------|-------------------------|------------------------
.._LINK_ERR       | cm_link_speed_enum      | cm_link_mode_enum
.._NARE_OK        | IP-address, host format | last 4 Bytes of the MAC-address
.._CONNECT_ERR    | pnio_status             | nca_status
.._PRM_ERR        | pnio_status             | nca_status
.._DOWN_ERR       | pnio_status             | cm_ar_reason_enum
.._IN_DATA        | IP-address, host format | 0
.._OFFLINE        | pnio_status             | cm_ar_reason_enum

The following entries still exist for session_key = 0:

lbentry_type      | data_1                  | data_2
------------------|-------------------------|------------------------
.._ADD_ERR        | pnio_status             | != 0
.._ADD_OK         | CM_AR_TYPE_xxx          | CM_AR_PROP_xxx

The entries, available in the CM, are returned in the .CNF, sorted by the time stamp. The most current entry has the index 0.

The output macro /CM_GET_CYCLE_COUNTER/ is used for the time_stamp.

@todo Open: Shall the number of entries in ClAddDevice() be specified?

@todo Open: Optionally, the APDU-Status.IND, Alarm.IND etc. can also be marked => increased memory demand, always only the last entry versus N-entries, ...


## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | Success.
CM_ERR_PARAM       | Parameter is wrong (e.g., out-of-range)
CM_ERR_SEQUENCE    | Call sequence not as specified.
******************************************************************************/
enum cm_cl_logbook_lbentry_type_enum {
	CM_CL_LBENTRY_TYPE_ADD_ERR     =  1,
	CM_CL_LBENTRY_TYPE_ADD_OK      =  2,
	CM_CL_LBENTRY_TYPE_LINK_ERR    =  3,
	CM_CL_LBENTRY_TYPE_NARE_ERR    =  4,
	CM_CL_LBENTRY_TYPE_NARE_OK     =  5,
	CM_CL_LBENTRY_TYPE_CONNECT_ERR =  6,
	CM_CL_LBENTRY_TYPE_PRM_ERR     =  7,
	CM_CL_LBENTRY_TYPE_DOWN_ERR    =  8,
	CM_CL_LBENTRY_TYPE_IN_DATA     =  9,
	CM_CL_LBENTRY_TYPE_OFFLINE     = 10,

	CM_CL_LBENTRY_TYPE_MAX         = 11
};

typedef struct cm_cl_logbook_entry_tag {

	LSA_UINT64 time_stamp;

	LSA_UINT16 session_key;

	LSA_UINT16 lbentry_type;
	/**< see cm_cl_logbook_lbentry_type_enum */

	LSA_UINT32 data_1;

	LSA_UINT32 data_2;

} CM_CL_LOGBOOK_ENTRY_TYPE;

typedef CM_CL_LOGBOOK_ENTRY_TYPE * CM_UPPER_CL_LOGBOOK_ENTRY_PTR_TYPE;

typedef struct cm_cl_logbook_read_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * Device Number
	 */

	LSA_UINT64 curr_time_stamp;
	/**< ## CNF
	 * For current value of the "cycle counters", refer to /CM_GET_CYCLE_COUNTER/
	 */

	LSA_UINT16 entry_count;
	/**< ## REQ
	 * Size of the entry array
	 * ## CNF
	 * Number of populated array elements. (0 <= index < entry_count) \n
	 * No entries are made in entry_count == 0
	 */

	CM_UPPER_CL_LOGBOOK_ENTRY_PTR_TYPE entry;
	/**< entry[entry_count]
	 * ## CNF
	 * Array of ...
	 */

} CM_CL_LOGBOOK_READ_TYPE;

typedef CM_CL_LOGBOOK_READ_TYPE * CM_UPPER_CL_LOGBOOK_READ_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_LOGBOOK_READ */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 053 CM_OPC_CL_PRM_UPDATE                                                  */
/**************************************************************************//**
@defgroup CM_OPC_CL_PRM_UPDATE 053 CM_OPC_CL_PRM_UPDATE
@ingroup cm-cl-opcodes
@{
@brief PrmUpdate has three main features. Prm_mode = AR, SUBMODULE or COC
@details
## Prm_mode = AR
Write the current parameterization to the device.
The prm_submodule_list must be empty.

## Prm_mode = SUBMODULE
Replace the current parameterization of one submodule, and write the new parameterization of this submodule to the device. \n
prm_submod_list must contain exactly one submodule with its respective record list.
An empty record list means that all records present will be deleted.

@note A PrmUpdate of a submodule which belongs to a COC-group is not allowed.

## Prm_mode = COC
Update the current parameterization of all the submodules contained within the COC-group and write the configuration of these submodules to the device. \n
prm_submod_list must contain all submodules of a single COC-group with their respective record lists. The prm_submod_list need not special order of the given submodules. \n
An empty record list means that all records of this submodule will be deleted.

@note On the network only the access point of the COC group is observed in the corresponding protocol element (see ControlCommand(PrmBegin).req)

@note any record_index > 0xFFF or the record_index 0xE040 or the record_index 0x80B0 is not allowed.

@note the submodules of the COC-group are descripting within the block "SubmoduleProperties", see /CM_ARCB_SUBMODULE_PROPERTIES_TYPE/.

Only one PrmUpdate can run at any given time.

The writing of the parameterization is further called "prm-sequence".
The common prm-sequence is descripting in the chart /Sequence chart: PrmUpdate after InData.ind/.

In AR-State passive, the prm_records of the given submodules will be taken over and no prm-sequence will started.

In AR-State active, the dynamic behavior depends on the current state of the AR.
After AR-InData.ind the prm-sequence starts only after PrmBegin.rsp.
If any other prm-sequence is currently running (e.g. a plug-alarm), the PrmUpdate-service is delayed.

If the submodule state of the ModuleDiffBlock, given by PrmBegin.rsp, is Wrong or Pulled, no prm record of this submodule will be written to the device.

## Sequence chart: PrmUpdate pre InData.ind

![sequence chart: PrmUpdate pre InData.ind](cm-sequence-chart-prm-update-pre-indata.svg)

## Sequence chart: Sequence chart: PrmUpdate after InData.ind

![sequence chart: PrmUpdate after InData.ind](cm-sequence-chart-prm-update-after-indata.svg)
This figure shows a complete prm sequence.

## Sequence chart: PrmUpdate delaying

![sequence chart: PrmUpdate delaying](cm-sequence-chart-prm-update-delaying.svg)

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------------
CM_OK              | The new parameterization is accepted and will be written as soons as possible.
CM_ERR_PARAM       | device / ar doesn't exist \n ar_type is not CM_AR_TYPE_SINGLE_SYSRED \n the submodule is no contained in expected list \n PrmUpdateCOC for serial machine not allowed \n not allowed record_index
CM_ERR_SEQUENCE    | A previous PrmUpdate is still running.
CM_OK_CANCELLED    | AR is aborted during PrmUpdate (any reason, as reported with the Abort.Ind).
******************************************************************************/
typedef struct cm_cl_prm_submodule_tag
{
	CM_LIST_TYPED_ENTRY_TYPE(cm_cl_prm_submodule_tag) link;
	/**< link to next entry, see list-macros in cm_list.h */

	LSA_UINT32 api;
	/**< ## REQ
	 * Application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 *Slot Number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * Subslot Number
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_arcb_record_tag) prm_record_list;
	/**< ## REQ
	 * A list of prm records, the new parameterization of this submodule. See @ref CM_ARCB_RECORD_TYPE
	 */

} CM_CL_PRM_SUBMODULE_TYPE;

typedef CM_CL_PRM_SUBMODULE_TYPE * CM_UPPER_CL_PRM_SUBMODULE_PTR_TYPE; /**< The pointer Type used for CM_CL_PRM_SUBMODULE_TYPE */

typedef CM_CL_PRM_SUBMODULE_TYPE const * CM_UPPER_CL_PRM_SUBMODULE_CONST_PTR_TYPE; /**< The const pointer Type used for CM_CL_PRM_SUBMODULE_TYPE */

/* prm_mode */
#define CM_CL_PRMMODE_AR         0x0001 /**< prm all submodules of the ar */
#define CM_CL_PRMMODE_SUBMODULE  0x0002 /**< prm a single submodule */
#define CM_CL_PRMMODE_COC        0x0003 /**< prm a COC group, especially the pdev submodules */

typedef struct cm_cl_prm_update_tag
{
	LSA_UINT16 device_nr;
	/**< ## REQ
	 * Device Number
	 */

	LSA_UINT16 prm_mode;
	/**< ## REQ
	 * Which submodules/records will be written, see CM_CL_PRMMODE_xxx \n
	 * CM_CL_PRMMODE_AR = write the prm-records of all submodules \n
	 * CM_CL_PRMMODE_SUBMODULE = write the prm-records of the given submodule\n
	 * CM_CL_PRMMODE_COC = write the prm-records of the COC submodules
	 */

	CM_LIST_TYPED_ENTRY_TYPE(cm_cl_prm_submodule_tag) prm_submod_list;
	/**< ## REQ
	 * List of prm_submod entries for the PDEV submodules.
	 * Valid if prm_mode =
	 * - CM_CL_PRMMODE_SUBMODULE: new prm-records for one submodule
	 * - CM_CL_PRMMODE_COC:       new prm-records for coc submodules
	 */
} CM_CL_PRM_UPDATE_TYPE;

typedef CM_CL_PRM_UPDATE_TYPE * CM_UPPER_CL_PRM_UPDATE_PTR_TYPE; /**< The pointer type used for CM_OPC_CL_PRM_UPDATE */

typedef CM_CL_PRM_UPDATE_TYPE const * CM_UPPER_CL_PRM_UPDATE_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_CL_PRM_UPDATE */
/** @} ***********************************************************************/


/******************************************************************************
end of client opcodes
******************************************************************************/
#endif


/******************************************************************************
begin of server opcodes
******************************************************************************/
#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE

/*****************************************************************************/
/* 100 CM_OPC_SV_CREATE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_SV_CREATE 100 CM_OPC_SV_CREATE
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_CREATE creates and configures a CM server (CM-SV) instance.
@details
The channel must have the path-type CM_PATH_TYPE_SERVER (see Channel Details, @ref CM_GET_PATH_INFO).

The CM-PD instance must have been created (see @ref CM_OPC_PD_CREATE).

The request sets the channel-global data-status to the default value (primary, run and station-ok; see also chapter @ref CM_OPC_SV_CONTROL).

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Another request is still running.
CM_ERR_PARAM       | Parameter(s) out of range.
******************************************************************************/

#define CM_SV_RECORD_DATA_READ_QUERY_NO  0
#define CM_SV_RECORD_DATA_READ_QUERY_YES 1

#define CM_SV_AR_FSU_DATA_ADJUST_NO  0
#define CM_SV_AR_FSU_DATA_ADJUST_YES 1

#define CM_SV_ALARM_RESPONDER_NO  0
#define CM_SV_ALARM_RESPONDER_YES 1

#define CM_SV_DATA_STATUS_INDICATION_NO  0
#define CM_SV_DATA_STATUS_INDICATION_YES 1

typedef struct cm_sv_create_tag {

	LSA_UINT8 can_RecordDataReadQuery;
	/**< ## REQ
	 * CM_SV_RECORD_DATA_READ_QUERY_NO ... reject a record-read with a RecordDataReadQuery-block \n
	 * CM_SV_RECORD_DATA_READ_QUERY_YES .. accept a record-read with a RecordDataReadQuery-block and the CM-user has to handle the RecordData-ReadQuery correctly, of course.
	 * @see @ref CM_OPC_SV_AR_READ_IND, CM_SV_RECORD_TYPE::readquery_length.
	 */

	LSA_UINT8 can_ARFSUDataAdjust;
	/**< ## REQ
	 * CM_SV_AR_FSU_DATA_ADJUST_NO ... the ARFSUDataAdjust record is handled internally \n
	 * CM_SV_AR_FSU_DATA_ADJUST_YES .. the ARFSUDataAdjust record will be indicated to the CM-user has to handle the ARFSUDataAdjust record correctly, of course. \n\n
	* The IEC 61158 V2.3 (see [/11/](@ref cm-ref-no-11)) introduced the "advanced startup mode". \n
	* Advanced mode ARs transport the fast-startup information not via the 0xE050 record, but via the ARFSUBlock of the Connect-PDU. See too chapter @ref CM_OPC_SV_AR_CONNECT_IND. \n
	* For advanced mode ARs the meaning of this flag is: An AR with an ARFSUBlock is accepted only if this flag is  !=0 (AP01034170).
	* @see @ref CM_OPC_SV_AR_READ_IND and [CM_OPC_SV_AR_WRITE_IND_RSP](@ref CM_OPC_SV_AR_WRITE_IND) for more information about the handling of the ARFSUDataAdjust record.
	*/

	LSA_UINT8 enable_AlarmResponder;
	/**< ## REQ
	 * CM_SV_ALARM_RESPONDER_NO ... all received alarms will be acknowledged internally with CM_PNIO_ERR_AlarmTypeNotSupported (AP01035514) \n
	 * CM_SV_ALARM_RESPONDER_YES .. all received alarms will be indicated to the cm-user (who will acknowledge them)
	 */

	LSA_UINT8 enable_DataStatusIndication;
	/**< ## REQ
	 * CM_SV_DATA_STATUS_INDICATION_NO ... CM_OPC_SV_AR_DATA_STATUS_IND is not indicated to the cm-user \n
	 * CM_SV_DATA_STATUS_INDICATION_YES .. CM_OPC_SV_AR_DATA_STATUS_IND is indicated to the cm-user
	*/

	LSA_UINT8 IOXS_Required;
	/**< ## REQ
	 * CM_IOXS_REQUIRED_NO ... SubmoduleProperties.DiscardIOXS is supported and the cm-user will handle it correctly \n
	 * CM_IOXS_REQUIRED_YES .. SubmoduleProperties.DiscardIOXS is not supported \n\n
	 * corresponds with the GSDML key "IOXS_Required" (V6.0i2.1x, TIA 1218229)
	 */

	LSA_UINT32 hardware_type;
	/**< ## CNF
	 * @see EDD_RQB_GET_PARAMS_TYPE::HardwareType etc.
	 */

	LSA_UINT16 interface_id;
	/**< ## CNF
	 * 1..16, see EDD_RQB_GET_PARAMS_TYPE::InterfaceID.
	 */

} CM_SV_CREATE_TYPE;

typedef CM_SV_CREATE_TYPE * CM_UPPER_SV_CREATE_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_CREATE. */

typedef CM_SV_CREATE_TYPE const * CM_UPPER_SV_CREATE_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_SV_CREATE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 101 CM_OPC_SV_DELETE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_SV_DELETE 101 CM_OPC_SV_DELETE
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DELETE deletes a CM-SV instance.
@details
All devices must have been removed (see chapter @ref CM_OPC_SV_DEVICE_REMOVE).

The request unregisters the RPC interface. The channel-global data-status is not reset.

@note No parameters!

## Async issues

None (the CM-SV instance has no indications).

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Another request is still running. Not all devices are removed.
******************************************************************************/
typedef LSA_VOID * CM_UPPER_SV_DELETE_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* 102 CM_OPC_SV_CONTROL                                                     */
/**************************************************************************//**
@defgroup CM_OPC_SV_CONTROL 102 CM_OPC_SV_CONTROL
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_CONTROL sets the channel-global data-status. The initial value after create is "run".
@details
The bit "station failure/ok" cannot be set by the user; it is controlled per AR by CM-SV.

The global data-status after CM_OPC_SV_CREATE is "run".

For more details, please refer to the EDD opcode EDD_SRV_SRT_PROVIDER_SET_STATE. All ARs of all devices of a CM-SV instance form a "group".

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Another request is still running.
CM_ERR_PARAM       | Parameter(s) out of range.
******************************************************************************/
#define CM_SV_CONTROL_CMD_DATA_STATUS_STOP 0x0001 /**< cmd-bit 0 */
#define CM_SV_CONTROL_CMD_DATA_STATUS_RUN  0x0002 /**< cmd-bit 1 */

typedef struct cm_sv_control_tag {

	LSA_UINT16 cmd;
	/**< ## REQ
	 * see CM_SV_CONTROL_CMD_DATA_STATUS_STOP, ...
	 */

} CM_SV_CONTROL_TYPE;

typedef CM_SV_CONTROL_TYPE * CM_UPPER_SV_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_CONTROL. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 103 CM_OPC_SV_DEVICE_ADD                                                  */
/**************************************************************************//**
@defgroup CM_OPC_SV_DEVICE_ADD 103 CM_OPC_SV_DEVICE_ADD
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DEVICE_ADD adds a device to a CM-SV instance.
@details
The "device_nr" is selected by the user (an in-parameter). The device is subsequently addressed by this number.

The request registers the RPC interface and object with the "annotation". The UUID of the RPC object is derived from the "vendor_id", the "device_id" (= DeviceIdentNumber), and the "instance".

A device is passive after it is added.

Before a device can be activated (see @ref CM_OPC_SV_DEVICE_CONTROL), the user has to provide the required indication resources. See chapter @ref CM_OPC_SV_DEVICE_PROVIDE_EVENT and @ref CM_OPC_SV_DEVICE_PROVIDE_ALARM.

A device can have "max_ars_IOC" simultaneous "IO controller ARs", "max_ars_IOS" simultaneous "IO supervisor ARs", and "max_ars_DAC" simultaneous "device access ARs".

# Usecase for parameter "parking_mode CM_SV_PARKING_MODE_YES" # {#cm-sv-decice-add-usecase-parking-mode}

The initial PDEV-parameterization (see chapter @ref cm-pd-channels-programming-model-parameterization) must not be interrupted by a remote parameterization. This is achieved by starting the Ethernet interface in "PHY-power off" mode and by activating the device only after completion of the PDEV-parameterization.

Now, there is a small window between activation of the Ethernet interface (which takes place somewhere between PDPrmEnd.REQ and PDPrmEnd.CNF) and the activation of the device.

If within this window a connect-request comes in, it will be rejected with a PNIOStatus "RMPM / Out of AR resources", because the device is still passive.

With FSU (fast startup) this behaviour is unwanted. The first connect-request after link-up has to be accepted.

Solution: A FSU device can set the parameter "parking_mode" to CM_SV_PARKING_MODE_YES. "Parking" means that a passive device will queue incoming connect-requests until activation.

@code
// the ethernet interface is in "PHY-Power off" mode
// connect-requests cannot come in

CM_OPC_SV_DEVICE_ADD / parking_mode = CM_SV_PARKING_MODE_YES
// the device is passive but connect-requests
// will be parked (queued) until activation

CM_OPC_SV_DEVICE_PROVIDE_EVENT
CM_OPC_SV_DEVICE_PROVIDE_ALARM
// provide event and alarm resources

CM_OPC_SV_SUBMODULE_ADD
// real config (including the PDEV) is plugged

// PDEV parameterization
CM_OPC_PD_PRM_BEGIN
CM_OPC_PD_PRM_WRITE, ...
CM_OPC_PD_PRM_END.REQ
// the ethernet interface becomes active, connect-requests may come in
CM_OPC_PD_PRM_END.CNF

CM_OPC_PD_EVENT_APPL_READY_IND
CM_OPC_PD_EVENT_APPL_READY_RSP
// now the PDEV is parameterized

CM_OPC_SV_DEVICE_CONTROL / CM_SV_DEVICE_CONTROL_CMD_ACTIVATE
// device is active, any parked connect-requests will be executed now
@endcode

# Usecase System Redundancy (SRD_enabled) # {#cm-sv-decice-add-usecase-srd}

NAP-Type S2: Please refer to chapter @ref cm-server-channels-sysred. \n
NAP-Type R1: see flag "R1_enabled" below.

# Usecase Configuration in Run (CIR_enabled) # {#cm-sv-decice-add-usecase-cir}

CIR is based on SRD. See the flag SRD_enabled.

Please refer to chapter cm-server-channels-cir.

# Usecase R1 (R1_enabled) ## {#cm-sv-decice-add-usecase-r1}

R1 is based on SRD. See the flag SRD_enabled.

For more details regarding R1 please refer to chapter @ref cm-server-channels-r-device.

# Interface and port submodules:

Starting with V6.0i2.25 the pdev-submodules are added internally (formerly: via AddSubmodule) in the device with parameter "contains_pdev" set to CM_SV_CONTAINS_PDEV_YES.

The information is fetched from CM-PD. See @ref CM_OPC_PD_CREATE.

Interface and port submodules are plugged into the API "0" (refer to [/10/](@ref cm-ref-no-10)).
Interfaces have the subslot number range 0x8i00, i = 0 to 15.
Ports have the subslot number range 0x8ipp, pp = 1 to 255.

They are therefore also denoted as "8ipp-submodules". The mapping of 8ipp-submodules to real interfaces and ports is described in chapters @ref cm-pd-channels-programming-model-addressing and @ref CM_OPC_PD_CREATE.

A CM channel is assigned to exactly one interface through the LSA-Path down to EDD.

In EDD each interface is assigned an EDD Interface ID (1 to 16). EDD Port IDs are numbered starting at 1.

The mapping of the EDD Interface ID to the "i" in "8ipp" is i := EDD Interface ID - 1.
The mapping of the EDD Port ID to the "pp" in "8ipp" is purely applicative.

The mapping "8ipp -> EDD Port ID" is done in @ref CM_OPC_PD_CREATE.

@attention There is no direct interconnection between the EDD Interface ID and the parameter "nic_id" of the Channel Details! See @ref CM_GET_PATH_INFO for more information.

# Parallelism

@see @ref cm-sv-parallelism

# Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is already added.
CM_ERR_PARAM       | Parameter(s) out of range.
CM_ERR_ALLOC_LOCAL | Out of local-mem.
******************************************************************************/

#define CM_SV_CONTAINS_PDEV_NO  0
#define CM_SV_CONTAINS_PDEV_YES 1

#define CM_SV_CENTRAL_PDEV_NO  0
#define CM_SV_CENTRAL_PDEV_YES 1

#define CM_SV_PARKING_MODE_NO  0
#define CM_SV_PARKING_MODE_YES 1

#define CM_SV_DEVICE_SRD_ENABLED_NO  0
#define CM_SV_DEVICE_SRD_ENABLED_YES 1

#define CM_SV_DEVICE_CIR_ENABLED_NO  0
#define CM_SV_DEVICE_CIR_ENABLED_YES 1

#define CM_SV_DEVICE_R1_ENABLED_NO  0
#define CM_SV_DEVICE_R1_ENABLED_YES 1

#define CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_NO  0
#define CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_YES 1

#define CM_SV_DEVICER_SKIP_CHECK_100MBPS_FDX_NO  0
#define CM_SV_DEVICER_SKIP_CHECK_100MBPS_FDX_YES 1

#define CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_NO  0 /* see GSDML/SharedDeviceSupported (TIA 1587591) */
#define CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_YES 1 /* see GSDML/SharedDeviceSupported (TIA 1587591) */

#define CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_NO  0 /* see GSDML/ReportingSystem (TIA 1645525) */
#define CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_YES 1 /* see GSDML/ReportingSystem (TIA 1645525) */

#define CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_NO  0 /* see GSDML/attribute to be defined yet (TIA 1722142) */
#define CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_YES 1 /* see GSDML/attribute to be defined yet (TIA 1722142) */

typedef struct cm_sv_device_add_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * Device number, Value range: 1..CM_CFG_MAX_SV_DEVICES
	 */

	/***/

	LSA_UINT16 vendor_id;
	/**< ## REQ
	 * Vendor ID (assigned by PROFIBUS International)
	 */

	LSA_UINT16 device_id;
	/**< ## REQ
	 * Device ID (vendor specific)
	 */

	LSA_UINT16 instance;
	/**< ## REQ
	 * Device Instance number (device specific)
	 */

	LSA_UINT8 annotation[63+1];
	/**< ## REQ
	 * This text will be entered in the RPC Endpoint Mapper.
	 * Zero-terminated string.
	 * See "sprintf" format string CLRPC_PNIO_ANNOTATION_FORMAT.
	 */

	/***/

	LSA_UINT16 max_ars_IOC;
	/**< ## REQ
	 * 0..9, "IO controller AR" (AP00774491, AP01379821) \n
	 * Number of simultaneous "IO controller ARs" (ARs with IOCRs and IOCRProperties.RTClass == 1/2/3/UDP and ARType == "IOCAR…").\n\n
	 * max_ars_IOC + max_ars_IOS + max_ars_DAC >= 1 \n
	 * max_ars_IOC + max_ars_IOS + max_ars_DAC <= CM_CFG_MAX_SV_ARS (see cm_cfg.h) \n\n
	 * the "implicit AR" exists always (thus the name).
	 */

	LSA_UINT16 max_ars_IOS;
	/**< ## REQ
	 * 0..9, "IO supervisor AR" (AP01034486, AP01379821) \n
	 * Number of simultaneous "IO supervisor ARs" (ARs with IOCRs and IOCRProperties.RTClass == 1/2/3/UDP and ARType == "IOSAR"). \n\n
	 * Must be set to 0. IO supervisor ARs are currently not supported (due to AP01034486).
	 * max_ars_IOC + max_ars_IOS + max_ars_DAC >= 1 \n
	 * max_ars_IOC + max_ars_IOS + max_ars_DAC <= CM_CFG_MAX_SV_ARS (see cm_cfg.h) \n\n
	 * the "implicit AR" exists always (thus the name).
	 */

	LSA_UINT16 max_ars_DAC;
	/**< ## REQ
	 * 0..n, "IOSAR with device access" (AP00774491) \n
	 * Number of simultaneous "device access ARs" (ARs without IOCRs and with ARType == "IOSAR" and ARProperties.DeviceAccess == 1) \n\n
	 * max_ars_IOC + max_ars_IOS + max_ars_DAC >= 1 \n
	 * max_ars_IOC + max_ars_IOS + max_ars_DAC <= CM_CFG_MAX_SV_ARS (see cm_cfg.h) \n\n
	 * the "implicit AR" exists always (thus the name).
	 */

	LSA_UINT16 max_slot_nr;
	/**< ## REQ
	 * Value range 0..0x7FFF
	 */

	LSA_UINT16 max_subslot_nr;
	/**< ## REQ
	 * Value range 0..0x7FFF \n
	 * The range 0x8000-0x8FFF (interfaces and ports) is allowed implicitly, refer to @ref CM_OPC_SV_SUBMODULE_ADD.
	 */

	LSA_UINT16 max_nr_of_subslots;
	/**< ## REQ
	 * Value range 0..0xFFFF (AP00774491) \n
	 * The submodules of an AR (expected config) are checked against these bounds: \n
	 * If slot_nr > max_slot_nr, then the AR is rejected. \n
	 * If subslot_nr > max_subslot_nr, then the AR is rejected. \n
	 * If number of submodules > max_nr_of_subslots, then the AR is rejected. \n
	 * @note There is no "max_api".
	 * I.e., memory usage is bounded by O(max_nr_of_subslots * (max_ars_IOC + max_ars_IOS))
	 */

	LSA_UINT16 alarm_send_max_length;
	/**< ## REQ
	 * Size of the biggest alarm sent \n
	 * 0..1404 (= CM_ALARM_DATA_LENGTH_MAX - CM_ALARM_OFFSET_DATA)
	 * ## CNF
	 * The effective value (different from .REQ in case of rounding up) \n
	 * Usecase: the user specifies the size of the biggest alarm he will ever send (see @ref CM_OPC_SV_AR_ALARM_SEND). \n
	 * A value less than 172 (= CM_ALARM_DATA_LENGTH_MIN - CM_ALARM_OFFSET_DATA) will be rounded up. \n
	 * If alarm_send_max_length > CM_SV_AR_CONNECT_TYPE::alarm_send_max_length (see @ref CM_OPC_SV_AR_CONNECT_IND) then the AR is rejected. \n
	 * @note Parameter was introduced with V5.2i1.25 (AP01232054)
	 */

	LSA_UINT8 contains_pdev;
	/**< ## REQ
	 * CM_SV_CONTAINS_PDEV_NO ... the device is not the pdev-device.\n
	 * CM_SV_CONTAINS_PDEV_YES .. the device is the pdev-device (and the pdev-submodules will be added internally). \n
	 * AP00995373
	 */

	LSA_UINT8 central_pdev;
	/**< ## REQ
	 * CM_SV_CENTRAL_PDEV_NO ... the PDEV can be owned and parameterized by an AR (if the device "contains_pdev"). \n
	 * CM_SV_CENTRAL_PDEV_YES .. the PDEV cannot be owned by an AR and is parameterized via CM-PD only (AP01066848).
	 */

	LSA_UINT8 parking_mode;
	/**< ## REQ
	 * for FSU (fast startup) devices only (AP00654135, AP00975630) \n
	 * the use-case is described here: @ref cm-sv-decice-add-usecase-parking-mode \n
	 * CM_SV_PARKING_MODE_NO ... normal mode \n
	 * CM_SV_PARKING_MODE_YES .. parking mode \n
	 */

	LSA_UINT16 write_multiple_parallelism;
	/**< ## REQ
	 * number of parallel CM_OPC_SV_AR_WRITE_IND per AR (AP00786836, CM V4.2i2.1) \n
	 * this parameter is effective for "real time ARs" only (not for "device access ARs") \n\n
	 * 0 ... reserved \n
	 * 1 ... one write-indication at a time (compatibility mode) \n
	 *       The cm-user sees no difference between a write-single and a write-multiple. \n\n
	 * n ... at most n write-indications at a time \n
	 *       The cm-user must be able to handle the parallel indications. The write-indications can be responded out-of-order.
	 */

	LSA_UINT8 SRD_enabled;
	/**< ## REQ
	 * CM_SV_DEVICE_SRD_ENABLED_NO ... normal mode \n
	 * CM_SV_DEVICE_SRD_ENABLED_YES .. functionality "System Redundancy" enabled \n
	 * The flag "contains_pdev" must be CM_SV_CONTAINS_PDEV_YES.
	 * @see @ref cm-sv-decice-add-usecase-srd
	 */

	LSA_UINT8 CIR_enabled;
	/**< ## REQ
	 * CM_SV_DEVICE_CIR_ENABLED_NO ... normal mode \n
	 * CM_SV_DEVICE_CIR_ENABLED_YES .. functionality "Configuration in Run" enabled \n
	 * CIR can be enabled only if SRD is enabled. \n
	 * Functionality CIR enabled with V6.0i1.40 (2014-07-30).
	 * @see @ref cm-sv-decice-add-usecase-cir
	 */

	LSA_UINT8 R1_enabled;
	/**< ## REQ
	 * CM_SV_DEVICE_R1_ENABLED_NO ... normal mode \n
	 * CM_SV_DEVICE_R1_ENABLED_YES .. functionality "R1" enabled \n
	 * R1 can be enabled only if SRD is enabled. \n
	 * The flag "parking_mode" must be CM_SV_PARKING_MODE_NO. \n
	 * The flag "enable_AlarmResponder" must be CM_SV_ALARM_RESPONDER_NO.
	 *
	 * History: Functionality R1 was enabled (but was not yet feature-complete) with V6.0i2.8 (2015-02-16)
	 *
	 * @see @ref cm-sv-decice-add-usecase-r1
	 */

	LSA_UINT32 R1_bridge_max;
	/**< ## REQ
	 * maximum "transport unit" of the R1-Bridge \n
	 * set to 0 if R1_enabled == CM_SV_DEVICE_R1_ENABLED_NO \n
	 * set to 0x0000FF80 for MaxSupportedRecordSize == 0xFF00 (or less) \n
	 * set to 0x00010080 for MaxSupportedRecordSize == 0xFFFF (or less)
	 */

	LSA_UINT8 PDevPrmInd_enabled;
	/**< ## REQ
	 * CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_NO ... normal mode \n
	 * CM_SV_DEVICE_PDEV_PRM_IND_ENABLED_YES .. functionality "PDEVPrm indication" enabled \n
	 * The flag "contains_pdev" must be CM_SV_CONTAINS_PDEV_YES. \n
	 * The flag "central_pdev" must be CM_SV_CENTRAL_PDEV_NO. \n
	 * The flag "R1_enabled" must be CM_SV_DEVICE_R1_ENABLED_NO.
	 *
	 * History: Functionality introduced with V6.0i2.2x (TIA 1193013). \n
	 * History: Support for S2 introduced with V7.0i2.05 (TIA 2227906).
	 *
	 * See @ref CM_OPC_SV_AR_PDEVPRM_IND for the usecase.
	 */

	LSA_UINT8 skip_check_100Mbps_FDX;
	/**< ## REQ
	 * CM_SV_DEVICER_SKIP_CHECK_100MBPS_FDX_NO ... normative behavior \n
	 * CM_SV_DEVICER_SKIP_CHECK_100MBPS_FDX_YES .. non-normative behavior: skip "PdevCheckFailed" of CMDEV (see PNIO-Spec)
	 * TIA 1963788 (V6.1i1.3x)
	 */

	LSA_UINT16 min_device_interval_31250ns;
	/**< ## REQ
	 * 0 ... no limitation \n
	 * n ... see GSDML/MinDeviceInterval e.g., 32 -> 32*31250ns = 1ms (AP01451871) \n
	 * If AR::send_clock * AR::reduction_ratio < min_device_interval then the AR is rejected.
	 */

	LSA_UINT16 gsd_MaxInputLength;
	/**< ## REQ
	 * Less or equal to 1440. See GSDML/MaxInputLength (per I-CR).
	 * TIA 1453731, see GSDML/NumberOfAR ... see max_ars_IOC.
	 */

	LSA_UINT16 gsd_MaxOutputLength;
	/**< ## REQ
	 * Less or equal 1440. See GSDML/MaxOutputLength (per O-CR).
	 * TIA 1453731, see GSDML/NumberOfAR ... see max_ars_IOC.
	 */

	LSA_UINT16 gsd_MaxDataLength;
	/**< ## REQ
	 * Equal to or greater than the highest value of MaxInputLength or MaxOutputLength. See GSDML/MaxDataLength or calculated as defined in GSDML (per device).
	 * TIA 1453731, see GSDML/NumberOfAR ... see max_ars_IOC.
	 */

	LSA_UINT16 gsd_NumberOfInputCR;
	/**< ## REQ
	 * Equal to or greater than "max_ars_IOC", see GSDML/NumberOfInputCR (per device)
	 */

	LSA_UINT16 gsd_NumberOfOutputCR;
	/**< ## REQ
	 * Equal to or greater than "max_ars_IOC", see GSDML/NumberOfOutputCR (per device)
	 */

	LSA_UINT16 gsd_NumberOfAdditionalInputCR;
	/**< ## REQ
	 * See GSDML/NumberOfAdditionalInputCR (per AR).
	 * TIA 1453731, see GSDML/NumberOfAR ... see max_ars_IOC.
	 */

	LSA_UINT16 gsd_NumberOfAdditionalOutputCR;
	/**< ## REQ
	 * See GSDML/NumberOfAdditionalOutputCR (per AR).
	 * TIA 1453731, see GSDML/NumberOfAR ... see max_ars_IOC.
	 */

	LSA_UINT16 gsd_NumberOfMulticastProviderCR;
	/**< ## REQ
	 * See GSDML/NumberOfAdditionalMulticastProviderCR (per AR; note that "Additional" is misleading).
	 * TIA 1453731, see GSDML/NumberOfAR ... see max_ars_IOC.
	 */

	LSA_UINT16 gsd_NumberOfMulticastConsumerCR;
	/**< ## REQ
	 * See GSDML/NumberOfMulticastConsumerCR (per AR).
	 * TIA 1453731, see GSDML/NumberOfAR ... see max_ars_IOC.
	 */

	LSA_UINT8 gsd_SharedDeviceSupported;
	/**< ## REQ
	 * CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_NO ... SharedDevice not supported \n
	 * CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_YES .. SharedDevice supported \n
	 * See GSDML/SharedDeviceSupported (TIA 1587591). \n
	 * If CM_SV_DEVICE_GSD_SHARED_DEVICE_SUPPORT_YES then "max_ars_IOC" must be greater or equal to 2.
	 */

	LSA_UINT8 gsd_ReportingSystemSupported;
	/**< ## REQ
	 * CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_NO ... ReportingSystem not supported \n
	 * CM_SV_DEVICE_GSD_REPORTING_SYSTEM_SUPPORT_YES .. ReportingSystem supported \n
	 * See GSDML/ReportingSystem (TIA 1645525).
	 */

	LSA_UINT8 gsd_AssetManagementSupported;
	/**< ## REQ
	 * CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_NO ... AssetManagement not supported \n
	 * CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_YES .. AssetManagement supported \n
	 * See GSDML/AssetManagementSupported (TIA 1722142). \n
	 * If CM_SV_DEVICE_GSD_ASSET_MANAGMENET_SUPPORT_YES, the AMFilterData block is appended to the returned I&M0FilterData blocks when reading index 0xF840. The user can read the AssetManagementData record (Index 0xF880) in this case. \n
	 * On record read indication for index 0xF880, the device must create the AssetManagementData record (AMR) and fill the record with the AssetManagementBocks describing its assets. \n
	 * A record read with index 0xF880 returns 0xB0 'invalid index' if AssetManagement is not supported.
	 * If AssetManagement is supported, reading of index 0xF880 is possible over an IO-AR, a device access AR, and the implicit AR.
	 */

	LSA_UINT16 nr_of_event_resources;
	/**< ## CNF
	 * The cm-user must provide this many CM_OPC_SV_DEVICE_PROVIDE_EVENT resources. See @ref CM_OPC_SV_DEVICE_PROVIDE_EVENT.
	 */

	LSA_UINT16 nr_of_alarm_resources;
	/**< ## CNF
	 * The cm-user must provide this many CM_OPC_SV_DEVICE_PROVIDE_ALARM resources. See @ref CM_OPC_SV_DEVICE_PROVIDE_ALARM.
	 */

} CM_SV_DEVICE_ADD_TYPE;

typedef CM_SV_DEVICE_ADD_TYPE * CM_UPPER_SV_DEVICE_ADD_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_DEVICE_ADD. */

typedef CM_SV_DEVICE_ADD_TYPE const * CM_UPPER_SV_DEVICE_ADD_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_SV_DEVICE_ADD. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 104 CM_OPC_SV_DEVICE_REMOVE                                               */
/**************************************************************************//**
@defgroup CM_OPC_SV_DEVICE_REMOVE 104 CM_OPC_SV_DEVICE_REMOVE
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DEVICE_REMOVE removes a device.
@details
The request aborts all ARs with reason CM_AR_REASON_RMV. Then it removes all subslots and diagnoses and unregisters the RPC interface and object.

On the IO controller side, the AR monitoring reports a failure as a consequence.

## Async issues

The user must reckon with indications (and respond them) until he sees the confirmation of the request, because of the asynchronous nature of the RQB interface.

## Network interactions

Running RPCs (ApplReady or ReadyForClass3 callback) are cancelled. This may take a few seconds.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Another request is still running. CM-SV is not created.
******************************************************************************/
typedef struct cm_sv_device_remove_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

} CM_SV_DEVICE_REMOVE_TYPE;

typedef CM_SV_DEVICE_REMOVE_TYPE * CM_UPPER_SV_DEVICE_REMOVE_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_DEVICE_REMOVE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 105 CM_OPC_SV_DEVICE_CONTROL                                              */
/**************************************************************************//**
@defgroup CM_OPC_SV_DEVICE_CONTROL 105 CM_OPC_SV_DEVICE_CONTROL
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DEVICE_CONTROL activates or passivates a device.
@details
An active device accepts RPC calls (connect, write, ...).

A passive device rejects all RPC calls with PNIOStatus "RMPM / Out of AR resources".

Activation of an active device means "do nothing".

Passivation of a passive device means "do nothing".

## Passivating an active device

If an active device is passivated, all ARs are aborted with reason CM_AR_REASON_PAS.

On the IO controller side, the AR monitoring reports a failure as the consequence.

## Async issues

Activation: None.

Passivation: The user must reckon with indications (and respond them) until he sees the confirmation of the request, because of the asynchronous nature of the RQB interface.

## Network interactions

Activation: None.

Passivation: Running RPCs (ApplReady or ReadyForClass3 callback) are cancelled. This may take a few seconds.

## PDEV device (CM_OPC_SV_DEVICE_ADD, contains_pdev == CM_SV_CONTAINS_PDEV_YES)

When the device with "contains_pdev == CM_SV_CONTAINS_PDEV_YES" is activated, it takes over control of the PDEV.

If now a PDEV-AR is established, the AR will parameterize the PDEV.
@note A system redundancy AR can re-parameterize the PDEV in the "data phase" (i.e. after the in-data indication) by a "PrmBegin/End" sequence or in case of an R1 device by the plug alarm for the "remote interface".

Whenever the PDEV is parameterized a [CM_OPC_PD_EVENT_APPL_READY_IND](@ref CM_OPC_PD_EVENT_APPL_READY_IND_RSP) will be signaled on the CM-PD channel.

History: It is this way since the feature "machine tailoring" (V5.3i2.01). Before that nothing was signaled on the CM-PD channel if the PDEV was parameterized via an AR.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Another request is still running. "Activate", but not enough indication resources provided. "Parking", but device is already active. PDEV is in use by user (CM-PD interface).
CM_ERR_PARAM       | Parameter(s) out of range.
******************************************************************************/
#define CM_SV_DEVICE_CONTROL_CMD_PASSIVATE  0 /**< passivate the device */
#define CM_SV_DEVICE_CONTROL_CMD_ACTIVATE   1 /**< activate the device */

typedef struct cm_sv_device_control_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 cmd;
	/**< ## REQ
	 * see CM_SV_DEVICE_CONTROL_CMD_PASSIVATE, CM_SV_DEVICE_CONTROL_CMD_ACTIVATE
	 */

} CM_SV_DEVICE_CONTROL_TYPE;

typedef CM_SV_DEVICE_CONTROL_TYPE * CM_UPPER_SV_DEVICE_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_DEVICE_CONTROL. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 106 CM_OPC_SV_DEVICE_LED_INFO                                             */
/**************************************************************************//**
@defgroup CM_OPC_SV_DEVICE_LED_INFO 106 CM_OPC_SV_DEVICE_LED_INFO
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DEVICE_LED_INFO tracks the diagnosis state (diagnosis, maintenance, qualified), the expected-config-versus-real-config state and the AR state of a device.
@details
The request is cancelled, when the device is removed. The request is returned to the user with CM_OK_CANCELLED before the device-remove request is confirmed.

The intended usage is controlling the BF-, SF- and MAINT-LED (thus the name).

Pattern: The user tells CM-SV its values of the led info. CM-SV returns the current values if the told values differ from the current values.

## Usecase

The user starts with state "no diagnosis and no info" and fills the RQB with diag_info := LSA_FALSE, mcons_info := LSA_FALSE, maint_info := 0, substate_info := 0.
If the diagnosis or info state changes, the RQB with the current values is returned to the user. The user updates his local state and does some action.
Finally, he returns the unchanged RQB (i.e., the current status) to CM.

## Async issues

Note that the user possibly sees CM_OK after issuing the device-remove request. In this case an event other than the device-remove request has triggered the LED-info.
However, the user knows that the device is being removed and is supposed to treat it as "cancelled".

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Out-parameters are valid.
CM_OK_CANCELLED    | Out-parameters are invalid.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added.
******************************************************************************/
#define CM_SV_DEVICE_LED_INFO_OWNED       0x0001 /**< expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_OWNED */
#define CM_SV_DEVICE_LED_INFO_ARP         0x0002 /**< expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_APPL_READY_PENDING */
#define CM_SV_DEVICE_LED_INFO_LOCKED_SO   0x0004 /**< expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_LOCKED_SUPERORDINATED */
#define CM_SV_DEVICE_LED_INFO_LOCKED_IOC  0x0008 /**< expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_LOCKED_IOCONTROLLER */
#define CM_SV_DEVICE_LED_INFO_LOCKED_IOS  0x0010 /**< expected-config: at least one submodule is CM_SUB_STATE_AR_INFO_LOCKED_IOSUPERVISOR */

#define CM_SV_DEVICE_LED_INFO_OK          0x0100 /**< expected-config: at least one submodule is CM_SUB_STATE_IDENT_OK */
#define CM_SV_DEVICE_LED_INFO_SUBST       0x0200 /**< expected-config: at least one submodule is CM_SUB_STATE_IDENT_SUBSTITUTE */
#define CM_SV_DEVICE_LED_INFO_WRONG       0x0400 /**< expected-config: at least one submodule is CM_SUB_STATE_IDENT_WRONG */
#define CM_SV_DEVICE_LED_INFO_NO          0x0800 /**< expected-config: at least one submodule is CM_SUB_STATE_IDENT_NO_SUBMODULE */

#define CM_SV_DEVICE_LED_INFO_DIAG        0x1000 /**< real-config: at least one submodule has diagnosis information */

#define CM_SV_DEVICE_LED_INFO_MCONS       0x2000 /**< m-consumer: at least one m-consumer has diagnosis information */

#define CM_SV_DEVICE_LED_INFO_RT_AR       0x4000 /**< one or more RT-ARs exist, and all are in-data */

typedef struct cm_sv_device_led_info_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	/***/

	LSA_UINT32 led_info;
	/**< ## REQ
	 * Status that the user knows
	 * ## CNF
	 * Current status \n\n

	 * Bit set: at least one submodule is in the state that corresponds to the bit. \n
	 * Bit field, refer to CM_SV_DEVICE_LED_INFO_OWNED, etc.
	 */

	LSA_UINT32 maint_info;
	/**< ## REQ
	 * Status that the user knows
	 * ## CNF
	 * Current status \n\n

	 * bit set = real-config: at least one submodule has maintenance or qualified information \n
	 * bit  0 = Maintenance-Required, see CM_MAINTENANCE_STATUS_REQUIRED \n
	 * bit  1 = Maintenance-Demanded, see CM_MAINTENANCE_STATUS_DEMANDED \n
	 * bit  2 = Reserved (since PNIO-Spec V2.3Ed2Rev22, V5.2i4.27, AP01545793 item 11) \n
	 * bit  3 = Qualifier_3 \n
	 * ... \n
	 * bit 31 = Qualifier_31 \n
	 *
	 * @note Bit field, refer to CM_MAINTENANCE_STATUS_REQUIRED, etc. and [/11/](@ref cm-ref-no-11) "Values for MaintenanceStatus".
	 */

} CM_SV_DEVICE_LED_INFO_TYPE;

typedef CM_SV_DEVICE_LED_INFO_TYPE * CM_UPPER_SV_DEVICE_LED_INFO_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_DEVICE_LED_INFO. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 109 CM_OPC_SV_DEVICE_READ                                                 */
/**************************************************************************//**
@defgroup CM_OPC_SV_DEVICE_READ 109 CM_OPC_SV_DEVICE_READ
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DEVICE_READ reads records from the device.
@details
The request is implemented by injecting an implicit read into the CM state machine. Therefore the buffer size must include CM_RECORD_OFFSET bytes for the headers (value is don't care).

The request is allowed too if the device is passive. It is the user's responsibility that this request is called only after the event resources are provided (see CM_OPC_SV_DEVICE_ADD).

## Reading indexes handled by the user

When reading an index that is handled by the user (e.g., I&M records) a CM_OPC_SV_AR_READ_IND indication is triggered (because the request is executed just like an implicit-read that came in over the Ethernet).
Whereas implicit-reads over the Ethernet are rejected internally if the device is passive, the request CM_OPC_SV_DEVICE_READ is executed always.
@attention Please be aware of this difference!

## Example

@code{.c}
//REQ:

UserDataMax = 10000;
data_length = CM_RECORD_OFFSET + UserDataMax;
data = Alloc(data_length);

//CNF:

If (Rqb.response == CM_OK) {
	If (Rqb.args.sv.dev_read.pnio_status == CM_PNIO_ERR_NONE) {
		UserDataPtr = data + CM_RECORD_OFFSET;
		UserDataLen = data_length - CM_RECORD_OFFSET;
		<Evaluate user data>
	}
}
@endcode

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Another request is still running.
CM_ERR_PARAM       | Parameter(s) out of range.
******************************************************************************/
typedef struct cm_sv_device_read_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT32 data_length;
	/**< ## REQ
	 * size of 'data' (including CM_RECORD_OFFSET)
	 * ## CNF
	 * CM_RECORD_OFFSET + number of bytes read
	 */

	CM_COMMON_MEM_U8_PTR_TYPE data;
	/**< ## REQ
	 * Buffer
	 */

	LSA_UINT32 api;
	/**< ## REQ
	 * Application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * Slot number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * Subslot number
	 */

	LSA_UINT32 record_index;
	/**< ## REQ
	 * Record index, refer to [/11/](@ref cm-ref-no-11)
	 */

	CLRPC_UUID_TYPE target_ar_uuid;
	/**< ## REQ
	 * TargetARUUID, refer to [/11/](@ref cm-ref-no-11)
	 */

	LSA_UINT32 pnio_status;
	/**< ## CNF
	 * PNIOStatus, refer to [/11/](@ref cm-ref-no-11)
	 */

} CM_SV_DEVICE_READ_TYPE;

typedef CM_SV_DEVICE_READ_TYPE * CM_UPPER_SV_DEVICE_READ_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_DEVICE_READ. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 111 CM_OPC_SV_SUBMODULE_ADD                                               */
/**************************************************************************//**
@defgroup CM_OPC_SV_SUBMODULE_ADD 111 CM_OPC_SV_SUBMODULE_ADD
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_SUBMODULE_ADD adds a set of submodules to a device.
@details
The request updates the "real configuration" and is confirmed immediately.

As a side-effect the "pull-plug state machine" is triggered (refer to [/10/](@ref cm-ref-no-10), state machines RSMSM, PULLSM, PLUGSM, and OWNSM).

## Interface and port submodules

Since V6.0i2.25 the interface and port submodules are plugged internally (see CM_OPC_SV_DEVICE_ADD, parameter "contains_pdev").

Ports are plugged via the opcode @ref CM_OPC_PD_SUBMODULE_CONTROL.

@note There is no "add API" or "add module"; APIs and modules are added automatically.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Submodule is already added.
CM_ERR_PARAM       | Must check elem[].response.
CM_ERR_ELEM        | Parameter(s) out-of-range. nr_of_elems is zero.
CM_ERR_OWNED       | 8ipp-submodules cannot be added to an active device.
******************************************************************************/
#define CM_SV_IM0_BITS_NOTHING      0x00 /**< this submodule does not contain I&M data */
#define CM_SV_IM0_BITS_SUBMODULE    0x01 /**< this submodule contains I&M0 data (see record index 0xF840 I&M0FilterData) */
#define CM_SV_IM0_BITS_MODULE       0x02 /**< modifier: this submodule stands for the module (see subblock I&M0FilterDataModule) */
#define CM_SV_IM0_BITS_DEVICE       0x04 /**< modifier: this submodule stands for the device (see subblock I&M0FilterDataDevice) */
#define CM_SV_IM0_BITS_IM5          0x08 /**< this submodule contains I&M5 data (see record index 0xAFF5 I&M5, values see CM_PD_CREATE_TYPE::oem_data) */
#define CM_SV_IM0_BITS_RESERVED     0x30 /**< reserved bits 0x10 and 0x20 */
#define CM_SV_IM0_BITS_R1_REMOTE    0x40 /**< is submodule of remote head (R1 only, note: not I&M but a solution for TIA 1915400) */
#define CM_SV_IM0_BITS_PROFIENERGY  0x80 /**< note: not I&M but a solution for TIA 1094128 */

typedef struct cm_sv_submodule_add_element_tag {

	LSA_UINT32 api;
	/**< ## REQ
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * slot number, range: see @ref CM_OPC_SV_DEVICE_ADD
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * subslot number, range: see @ref CM_OPC_SV_DEVICE_ADD plus 0x8000-0x8FFF \n
	 * Starting with V6.0i2.25 the pdev-submodules are added internally (see @ref CM_OPC_SV_DEVICE_ADD parameter "contains_pdev"; the information is fetched from CM-PD). \n
	 * Thus adding a submodule in the range of the pdev-submodules is no longer allowed (ERR_PARAM). \n
	 * For more details see the text below and chapter @ref cm-server-channels-device-model-of-pnio.
	 */

	LSA_UINT32 mod_ident;
	/**< ## REQ
	 * module ident number
	 */

	LSA_UINT32 sub_ident;
	/**< ## REQ
	 * submodule ident number
	 */

	LSA_UINT8 im0_bits;
	/**< ## REQ
	 * For values refer to CM_SV_IM0_BITS_NOTHING, ... \n\n
	 * Submodules with the bit CM_SV_IM0_BITS_SUBMODULE set are listed in the subblock I&M0FilterDataSubmodule of the record index 0xF840 I&M0FilterData. \n\n
	 * If the bit CM_SV_IM0_BITS_SUBMODULE is set, then the modifier-bits CM_SV_IM0_BITS_MODULE (module representative, subblock I&M0FilterDataModule) and CM_SV_IM0_BITS_DEVICE (device representative, subblock I&M0FilterDataDevice) can be set additionally. \n\n
	 * Submodules with the bit CM_SV_IM0_BITS_IM5 set enable reading the record index 0xAFF5 I&M5.
	 * Note that this bit is not a modifier of the bit CM_SV_IM0_BITS_SUBMODULE. All submodules with this bit set return the same content:
	 * the values that were specified in parameter "oem_data" of opcode @ref CM_OPC_PD_CREATE. Important: It is an error to specify this bit without having supplied "oem_data".
	 * Typically this bit is set on the submodule that acts as the device representative (a.k.a. DAP) and on all submodules that shall provide the same I&M5 data. Please refer to the GSDML property "IM5_Supported". \n\n
	 * Submodules with the bit CM_SV_IM0_BITS_PROFIENERGY set enable reading the index 0x80A0.
	 * Note that this bit does not belong to I&M - this may be confusing but introducing a new flag was not possible. \n\n
	 * The bit CM_SV_IM0_BITS_R1_REMOTE applies to R1 devices only and is related to the bit CM_SV_IM0_BITS_IM5. See chapter @ref cm-server-channels-r-device-submod-add for more information.
	 */

	LSA_UINT16 response;
	/**< ## CNF
	 * Element specific response. Needs to be checked only if rqb.response == CM_ERR_ELEM.
	 */

} CM_SV_SUBMODULE_ADD_ELEMENT_TYPE;

typedef CM_SV_SUBMODULE_ADD_ELEMENT_TYPE * CM_UPPER_SV_SUBMODULE_ADD_ELEMENT_PTR_TYPE; /**< The element pointer type used for CM_OPC_SV_SUBMODULE_ADD. */

typedef CM_SV_SUBMODULE_ADD_ELEMENT_TYPE const * CM_UPPER_SV_SUBMODULE_ADD_ELEMENT_CONST_PTR_TYPE; /**< The element const pointer type used for CM_OPC_SV_SUBMODULE_ADD. */

typedef struct cm_sv_submodule_add_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 nr_of_elems;
	/**< ## REQ
	 * Number of elements contained in the array "elem" (1..n).
	 */

	CM_SV_SUBMODULE_ADD_ELEMENT_TYPE elem[1];
	/**< ## REQ
	 * elements to add \n
	 * elem[2..n] follow here
	 */

} CM_SV_SUBMODULE_ADD_TYPE;

typedef CM_SV_SUBMODULE_ADD_TYPE * CM_UPPER_SV_SUBMODULE_ADD_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_SUBMODULE_ADD. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 112 CM_OPC_SV_SUBMODULE_REMOVE                                            */
/**************************************************************************//**
@defgroup CM_OPC_SV_SUBMODULE_REMOVE 112 CM_OPC_SV_SUBMODULE_REMOVE
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_SUBMODULE_REMOVE removes a set of submodules from a device.
@details
If a submodule is owned by an AR all pending alarms for this submodule are cancelled before the request is confirmed.

The request updates the "real configuration" and is confirmed immediately.

As a side-effect the "pull-plug state machine" is triggered (refer to /10/, state machines RSMSM, PULLSM, PLUGSM, and OWNSM).

@todo CM Documentation: More Text for IO data handling!
Before removing the submodule, the user must set IOPS = BAD in the owning AR.

@note There is no "remove API" or "remove module"; APIs and modules are removed automatically. Existing diagnoses are removed automatically, too.

## Interface and port submodules

Ports are pulled via the opcode @ref CM_OPC_PD_SUBMODULE_CONTROL.

## Async issues

The user must reckon with indications (and respond them) until he sees the confirmation of the request, because of the asynchronous nature of the RQB interface.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Submodule is not added.
CM_ERR_ELEM        | Must check elem[].response.
CM_ERR_PARAM       | Parameter(s) out-of-range. nr_of_elems is zero.
CM_ERR_OWNED       | 8ipp-submodules cannot be removed from an active device.
******************************************************************************/
typedef struct cm_sv_submodule_remove_element_tag {

	LSA_UINT32 api;
	/**< ## REQ
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * subslot number
	 */

	LSA_UINT16 response;
	/**< ## CNF
	 * Element specific response. Needs to be checked only if rqb.response == CM_ERR_ELEM.
	 */

} CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE;

typedef CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE * CM_UPPER_SV_SUBMODULE_REMOVE_ELEMENT_PTR_TYPE; /**< The element pointer type used for CM_OPC_SV_SUBMODULE_REMOVE. */

typedef CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE const * CM_UPPER_SV_SUBMODULE_REMOVE_ELEMENT_CONST_PTR_TYPE; /**< The element const pointer  type used for CM_OPC_SV_SUBMODULE_REMOVE. */

typedef struct cm_sv_submodule_remove_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 nr_of_elems;
	/**< ## REQ
	 * Number of elements contained in the array "elem" (1..n).
	 */

	CM_SV_SUBMODULE_REMOVE_ELEMENT_TYPE elem[1];
	/**< ## REQ
	 * elements to add \n
	 * elem[2..n] follow here
	 */

} CM_SV_SUBMODULE_REMOVE_TYPE;

typedef CM_SV_SUBMODULE_REMOVE_TYPE * CM_UPPER_SV_SUBMODULE_REMOVE_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_SUBMODULE_REMOVE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 114 CM_OPC_SV_DIAG_ADD                                                    */
/**************************************************************************//**
@defgroup CM_OPC_SV_DIAG_ADD 114 CM_OPC_SV_DIAG_ADD
@ingroup cm-sv-opcodes
@brief The request CM_OPC_SV_DIAG_ADD adds diagnoses to submodules.
@details
@deprecated Usage of CM_OPC_SV_DIAG_ADD is deprecated. Use @ref CM_OPC_SV_DIAG_ADD2 instead!

The request updates the "diagnosis database" and is confirmed immediately.

Note that CM-SV does not automatically generate a diagnosis alarm. This is the responsibility of the user.

Several diagnoses can be added to a submodule. A diagnosis is identified by the user-assigned "diag_tag". Using this "diag_tag" a diagnosis can be updated or removed later.

If the "diag_tag" does not fit your needs, check the opcode @ref CM_OPC_SV_DIAG_ADD2.

Diagnoses are managed in a list (per submodule). A diagnosis is always added to the end of the list. If a diagnosis with the same "diag_tag" already exists, it will be removed from the list beforehand and the element specific response is CM_OK_REPLACED instead of CM_OK.

In other words: When the diagnoses are queried, they are returned in the temporal sequence of their occurrence (per submodule).

Diagnoses added with @ref CM_OPC_SV_DIAG_ADD have to be removed with CM_OPC_SV_DIAG_REMOVE.

If diagnoses are added to a PDEV-Submodule, @ref CM_OPC_SV_DIAG_ADD returns CM_ERR_PARAM.

Diagnoses can be queried via RPC. Refer to [/11/](@ref cm-ref-no-11), "DiagnosisData".

Interconnection with the "station failure/ok" bit of the APDUStatus:

Refer to chapter @ref cm-appendix-e-alarm-specifier-data-status.

##Parallelism:

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_REPLACED     | (reported in elem[].response only!) Success. The existing diagnosis was replaced by the new one.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Submodule is not added.
CM_ERR_ELEM        | Must check elem[].response.
CM_ERR_PARAM       | Parameter(s) out-of-range. nr_of_elems is zero. Submodule is a PDEV submodule.
******************************************************************************/


/*****************************************************************************/
/* 116 CM_OPC_SV_DIAG_ADD2                                                   */
/**************************************************************************//**
@defgroup CM_OPC_SV_DIAG_ADD2 116 CM_OPC_SV_DIAG_ADD2
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DIAG_ADD2 adds diagnoses to submodules.
@details
CM_OPC_SV_DIAG_ADD2 was introduced in V5.2i4.16 (usecase AP01467031). The opcode is similar (but not equal) to @ref CM_OPC_SV_DIAG_ADD.

CM_OPC_SV_DIAG_ADD2 uses a set of parameters as the key of a diagnosis entry ("compound key"), whereas @ref CM_OPC_SV_DIAG_ADD uses the single parameter "diag_tag" ("surrogate key").

@attention The user has to decide whether to use @ref CM_OPC_SV_DIAG_ADD or CM_OPC_SV_DIAG_ADD2 in his application. Mixing the opcodes is not allowed! Note that CM does not check this!

CM_OPC_SV_DIAG_ADD2 uses the same type as @ref CM_OPC_SV_DIAG_ADD. The parameter "diag_tag" has no effect (but has to be set to 1). Instead, the "compound key" takes its role.

The following set of parameters composes the key of a diagnosis entry:

Location part of the key:
 - Api
 - Slot_nr
 - Subslot_nr
 - channel_number
 - channel_properties:: CM_CHPR_ACCU_XXX (Bit 8)
 - channel_properties::CM_CHPR_DIR_XXX (Bit 13 - 15)

Diagnosis part of the key:
 - If diag_type == CM_SV_DIAG_TYPE_CHANNEL:
   - u.ch.channel_error_type
 - If diag_type == CM_SV_DIAG_TYPE_EXT_CHANNEL:
   - u.ext.channel_error_type
   - u.ext.ext_channel_error_type
 - If diag_type == CM_SV_DIAG_TYPE_QUALIFIED:
   - u.qua.channel_error_type
   - u.qua.ext_channel_error_type
 - If diag_type == CM_SV_DIAG_TYPE_GENERIC:
   - u.gen.info_tag

Note that an EXT_CHANNEL diagnosis with ext_channel_error_type = 0 is considered the same as a CHANNEL diagnosis.

Note that a QUALIFIED diagnosis with qualified_channel_qualifier = 0 is considered the same as an EXT_CHANNEL diagnosis.

For more information on Profinet diagnosis, please refer to the PNIO Diagnosis Guideline [/20/](@ref cm-ref-no-20).

Diagnoses added with CM_OPC_SV_DIAG_ADD2 have to be removed with @ref CM_OPC_SV_DIAG_REMOVE2.

If diagnoses are added to a PDEV-Submodule, CM_OPC_SV_DIAG_ADD2 returns CM_ERR_PARAM.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_REPLACED     | (reported in elem[].response only!) Success. The existing diagnosis was replaced by the new one.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Submodule is not added.
CM_ERR_ELEM        | Must check elem[].response.
CM_ERR_PARAM       | Parameter(s) out-of-range. nr_of_elems is zero. Submodule is a PDEV submodule.
******************************************************************************/

#define CM_SV_DIAG_TYPE_CHANNEL      0 /**< ChannelDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_EXT_CHANNEL  1 /**< ExtChannelDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_QUALIFIED    2 /**< QualifiedChannelDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_GENERIC      3 /**< ManufacturerSpecificDiagnosis, see PNIO-Spec */
#define CM_SV_DIAG_TYPE_MAX          4 /**< internally used */

typedef struct cm_sv_diag_add_element_tag {

	LSA_UINT32 api;
	/**< ## REQ
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * subslot number
	 */

	LSA_UINT32 diag_tag;
	/**< ## REQ
	 * := 1 (because 0 means "all" for CM_OPC_SV_DIAG_REMOVE2)
	 */

	LSA_UINT16 diag_type;
	/**< ## REQ
	 * see CM_SV_DIAG_TYPE_CHANNEL, ...
	 */

	LSA_UINT16 channel_number;
	/**< ## REQ
	 * ChannelNumber, 0..0x8000
	 */

	LSA_UINT16 channel_properties;
	/**< ## REQ
	 * ChannelProperties, see CM_CHPR_MAINT_MASK, ..., see @ref cm-channel-properties.
	 */

	union {

		struct {
			LSA_UINT16 channel_error_type;
			/**< ## REQ
			 * ChannelErrorType: 1..0x7FFF. For values, refer to [/11/](@ref cm-ref-no-11).
			 */
		} ch; /**< if "diag_type" is CM_SV_DIAG_TYPE_CHANNEL */

		struct {
			LSA_UINT16 channel_error_type;
			/**< ## REQ
			 * ChannelErrorType: 1..0x7FFF. For values, refer to [/11/](@ref cm-ref-no-11).
			 */

			LSA_UINT16 ext_channel_error_type;
			/**< ## REQ
			 * ExtChannelErrorType: For values refer to [/11/](@ref cm-ref-no-11).
			 */

			LSA_UINT32 ext_channel_add_value;
			/**< ## REQ
			 * ExtChannelAddValue: "user-specific"
			 */
		} ext; /**< if "diag_type" is CM_SV_DIAG_TYPE_EXT_CHANNEL */

		struct {
			LSA_UINT16 channel_error_type;
			/**< ## REQ
			 * ChannelErrorType: 1..0x7FFF. For values, refer to [/11/](@ref cm-ref-no-11).
			 */

			LSA_UINT16 ext_channel_error_type;
			/**< ## REQ
			 * ExtChannelErrorType: For values refer to [/11/](@ref cm-ref-no-11).
			 */

			LSA_UINT32 ext_channel_add_value;
			/**< ## REQ
			 * ExtChannelAddValue: "user-specific"
			 */

			LSA_UINT32 qualified_channel_qualifier;
			/**< ## REQ
			 * QualifiedChannelQualifier: For values refer to [/11/](@ref cm-ref-no-11).
			 */
		} qua; /**< if "diag_type" is CM_SV_DIAG_TYPE_QUALIFIED */

		struct {
			LSA_UINT16               info_tag;
			/**< ## REQ
			 * UserStructureIdentifier for info_data: 0..0x7FFF. Refer to [/11/](@ref cm-ref-no-11), "ManufacturerSpecificDiagnosis".
			 */

			LSA_UINT16               info_length;
			/**< ## REQ
			 * length of info_data
			 */

			CM_COMMON_MEM_U8_PTR_TYPE info_data;
			/**< ## REQ
			 * Pointer to the diagnosis information. CM makes a copy of the diagnosis information!
			 */
		} gen; /**< if "diag_type" is CM_SV_DIAG_TYPE_GENERIC */
	} u; /**< specific for diag-type, see PNIO-Spec */

	LSA_UINT16 response;
	/**< ## CNF
	 * Element specific response. Needs to be checked only if rqb.response == CM_ERR_ELEM. @note CM_OK_REPLACED does not result in a CM_ERR_ELEM.
	 */

} CM_SV_DIAG_ADD_ELEMENT_TYPE;

typedef CM_SV_DIAG_ADD_ELEMENT_TYPE * CM_UPPER_SV_DIAG_ADD_ELEMENT_PTR_TYPE; /**< The element pointer type used for CM_OPC_SV_DIAG_ADD2 and CM_OPC_SV_DIAG_REMOVE2. */

typedef CM_SV_DIAG_ADD_ELEMENT_TYPE const * CM_UPPER_SV_DIAG_ADD_ELEMENT_CONST_PTR_TYPE; /**< The const element pointer type used for CM_OPC_SV_DIAG_ADD2 and CM_OPC_SV_DIAG_REMOVE2. */

typedef struct cm_sv_diag_add_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 nr_of_elems;
	/**< ## REQ
	 * Number of elements contained in the array "elem" (1..n).
	 */

	CM_SV_DIAG_ADD_ELEMENT_TYPE elem[1];
	/**< ## REQ
	 * elements to add \n
	 * elem[2..n] follow here
	 */

} CM_SV_DIAG_ADD_TYPE;

typedef CM_SV_DIAG_ADD_TYPE * CM_UPPER_SV_DIAG_ADD_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_DIAG_ADD2 and CM_OPC_SV_DIAG_REMOVE2. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 115 CM_OPC_SV_DIAG_REMOVE                                                 */
/**************************************************************************//**
@defgroup CM_OPC_SV_DIAG_REMOVE 115 CM_OPC_SV_DIAG_REMOVE
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_DIAG_REMOVE removes diagnoses from submodules.
@details
The request updates the "diagnosis database" and is confirmed immediately.

Interconnection with the "station failure/ok" bit of the APDUStatus:

@note  CM-SV does not automatically generate a diagnosis alarm. This is the responsibility of the user.

Refer to chapter @ref cm-appendix-e-alarm-specifier-control-problem-indicator-ar.

@deprecated @ref CM_OPC_SV_DIAG_ADD and CM_OPC_SV_DIAG_REMOVE have been deprecated. Use @ref CM_OPC_SV_DIAG_ADD2 and @ref CM_OPC_SV_DIAG_REMOVE2 instead!

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Submodule is not added. Diagnosis with "diag_tag" is not added.
CM_ERR_ELEM        | Must check elem[].response.
CM_ERR_PARAM       | Parameter(s) out-of-range. nr_of_elems is zero. Submodule is a PDEV submodule.
******************************************************************************/
typedef struct cm_sv_diag_remove_element_tag {

	LSA_UINT32 api;
	/**< ## REQ
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/**< ## REQ
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/**< ## REQ
	 * subslot number
	 */

	LSA_UINT32 diag_tag;
	/**< ## REQ
	 * See "diag_tag" in @ref CM_OPC_SV_DIAG_ADD, (0 = all)
	 */

	LSA_UINT16 response;
	/**< ## CNF
	 * Element specific response. Needs to be checked only if rqb.response == CM_ERR_ELEM.
	 */

} CM_SV_DIAG_REMOVE_ELEMENT_TYPE;

typedef CM_SV_DIAG_REMOVE_ELEMENT_TYPE * CM_UPPER_SV_DIAG_REMOVE_ELEMENT_PTR_TYPE;

typedef CM_SV_DIAG_REMOVE_ELEMENT_TYPE const * CM_UPPER_SV_DIAG_REMOVE_ELEMENT_CONST_PTR_TYPE;

typedef struct cm_sv_diag_remove_tag {

	LSA_UINT16 device_nr; /* device number */

	LSA_UINT16 nr_of_elems; /* number of array elements (1..n) */

	CM_SV_DIAG_REMOVE_ELEMENT_TYPE elem[1];

	/* elem[2..n] follow here */

} CM_SV_DIAG_REMOVE_TYPE;

typedef CM_SV_DIAG_REMOVE_TYPE * CM_UPPER_SV_DIAG_REMOVE_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_DIAG_REMOVE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 117 CM_OPC_SV_DIAG_REMOVE2                                                */
/**************************************************************************//**
@defgroup CM_OPC_SV_DIAG_REMOVE2 117 CM_OPC_SV_DIAG_REMOVE2
@ingroup cm-sv-opcodes

@brief The request CM_OPC_SV_DIAG_REMOVE2 removes diagnoses from submodules.
@details
CM_OPC_SV_DIAG_REMOVE2 was introduced together with @ref CM_OPC_SV_DIAG_ADD2.

CM_OPC_SV_DIAG_REMOVE2 uses the same type as CM_OPC_SV_DIAG_ADD2 (sic!).

## Parameters

.diag_tag REQ == 0: Remove all diagnoses of this submodule (all other parameters are don't care; but set diag_type = CM_SV_DIAG_TYPE_MAX for clarity).

.diag_tag REQ != 0: Remove the diagnosis with this "key".

@see @ref CM_OPC_SV_DIAG_ADD2

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Submodule is not added. Diagnosis with "diag_tag" is not added.
CM_ERR_ELEM        | Must check elem[].response.
CM_ERR_PARAM       | Parameter(s) out-of-range. nr_of_elems is zero. Submodule is a PDEV submodule.
******************************************************************************/


/*****************************************************************************/
/* 170 CM_OPC_SV_R1BRIDGE_SEND_PROVIDE                                       */
/**************************************************************************//**
@defgroup CM_OPC_SV_R1BRIDGE_SEND_PROVIDE 170 CM_OPC_SV_R1BRIDGE_SEND_PROVIDE
@ingroup cm-sv-opcodes-r1
@brief The request CM_OPC_SV_R1BRIDGE_SEND_PROVIDE provides an R1Bridge indication-resource to the device.
@details
The user has to provide 1 resource if the device is set-up as an R1-device.

The request is cancelled when the device is removed. The request is returned to the user with CM_OK_CANCELLED before the device-remove request is confirmed.

This opcode uses the same type as @ref CM_OPC_SV_R1BRIDGE_SEND_IND_RSP. Only the parameter CM_SV_R1BRIDGE_SEND_TYPE::device_nr is used!

@note There are no negative responses such as CM_ERR_SEQUENCE or CM_ERR_PARAM because the application cannot handle them gracefully. A negative response means that the application is faulty. Thus it was decided that CM shall call FATAL (TIA 1495347).

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK_CANCELLED    | Out-parameters are invalid.
******************************************************************************/


/*****************************************************************************/
/* 171/172 CM_OPC_SV_R1BRIDGE_SEND_IND/_RSP                                  */
/**************************************************************************//**
@defgroup CM_OPC_SV_R1BRIDGE_SEND_IND_RSP 171/172 CM_OPC_SV_R1BRIDGE_SEND_IND/_RSP
@ingroup cm-sv-opcodes-r1
@{
@brief The indication CM_OPC_SV_R1BRIDGE_SEND_IND signals that the application has to send the message to the other side. The indication has to be responded.
@details
Semantics of CM_OPC_SV_R1BRIDGE_SEND_RSP: CM_OPC_SV_R1BRIDGE_SEND_RSP means that the message is acknowledged by the transport layer.
This means that the message is stored in a buffer on the other side. In other words: If this side fails after CM_OPC_SV_R1BRIDGE_SEND_RSP, the message is not lost.

Semantics of "message cannot be transferred": If a message cannot be transferred, the bridge is considered broken and the application must react accordingly.

@note The time span between CM_OPC_SV_R1BRIDGE_SEND_IND and CM_OPC_SV_R1BRIDGE_SEND_RSP may be arbitrarily long.

## Bridge failure and send indication

@attention After signaling [CM_OPC_SV_R1BRIDGE_CONTROL](@ref CM_OPC_SV_R1BRIDGE_CONTROL) "broken" to CM the user must reckon with indications (and respond them) until he sees the confirmation of the bridge-control, because of the asynchronous nature of the RQB interface.

@note Because of this, there is no sequence to obey in the case "send is underway and while waiting for the transport acknowledge the bridge breaks".
The user can respond the send RQB first and then signaling "broken" or vice versa. CM cannot distinguish the cases.

## "selector_id" of PPPN_OPC_PRIO_x_SEND

CM does not specify the value of the PPPN-Parameter "selector_id".


## Buffer-size for PPPN-Receive

All data one side can send must be received on the other side to form a working system.
Thus the buffer-size for PPPN-Receive is equal to that value (or greater).

The data depends on the size of the RPC-buffer which in turn depends on the "Connect" (number of submodules) and the GSD-Property "MaxSupportedRecordSize". Plus some protocol overhead.

The maximum value is specified via the parameter "R1_bridge_max" of @ref CM_OPC_SV_DEVICE_ADD.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/
typedef struct cm_sv_r1bridge_send_tag {

	LSA_UINT16 device_nr;
	/**< # IND
	 * device number
	 */

	LSA_UINT16 session_key;
	/**< # IND
	 * Do not evaluate, do not change! Internally used to find the way back.
	 */

	LSA_UINT32 message_size;
	/**< IND
	 * size of message_data
	 * The value is always less or equal to "R1_bridge_max" (see @ref CM_OPC_SV_DEVICE_ADD).
	 */

	LSA_VOID_PTR_TYPE message_data;
	/**< IND
	 * the message (connect, connect_done, ...- array of octets)
	 */

	LSA_VOID_PTR_TYPE message_raw;
	/**< IND
	 * Internally used at indication-side in response-handling. Do not evaluate, do not change!
	 * The following picture is for information only.
	 * @verbatim
	 [ MessagePrefix | MessageHeader | MessageBody ]
	 ^               ^
	 Raw             Data
					 <-- Size --------------------->

	 MessagePrefix ::= ListElem, etc.
					   ^-- solves the "queueable" problem on the "IND side"

	 MessageHeader ::= Type, etc.

	 MessageBody ::= Octets specific to Type
	@endverbatim
	 */

} CM_SV_R1BRIDGE_SEND_TYPE;

typedef CM_SV_R1BRIDGE_SEND_TYPE * CM_UPPER_SV_R1BRIDGE_SEND_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_R1BRIDGE_SEND_IND/_RSP. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 173 CM_OPC_SV_R1BRIDGE_RECEIVED                                           */
/**************************************************************************//**
@defgroup CM_OPC_SV_R1BRIDGE_RECEIVED 173 CM_OPC_SV_R1BRIDGE_RECEIVED
@ingroup cm-sv-opcodes-r1
@{
@brief The request CM_OPC_SV_R1BRIDGE_RECEIVED forwards a received bridge-message to CM.
@details
CM can handle more than one request at a time. However, it is recommended not to utilize this feature without sound reasons.

## Usage

CM->APPL                    | APPL->BRIDGE   | BRIDGE->APPL | APPL->CM
--------------------------- |----------------|--------------|----------------
CM_OPC_SV_R1BRIDGE_SEND_IND | PPPN-Send      |              |               |
CM_OPC_SV_R1BRIDGE_SEND_RSP | PPPN-Send-Ack  |              |               |
|                           |                | PPPN-Receive | CM_OPC_SV_R1BRIDGE_RECEIVED

@note There are no negative responses such as CM_ERR_SEQUENCE or CM_ERR_PARAM because the application cannot handle them gracefully. A negative response means that the application is faulty. Thus it was decided that CM shall call FATAL (TIA 1495347).

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/
typedef struct cm_sv_r1bridge_received_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT32 message_size;
	/**< ## REQ
	 * size of message_data
	 */

	LSA_VOID_PTR_TYPE message_data;
	/**< ## REQ
	 * the message (connect, connect_done, ...)
	 */

} CM_SV_R1BRIDGE_RECEIVED_TYPE;

typedef CM_SV_R1BRIDGE_RECEIVED_TYPE * CM_UPPER_SV_R1BRIDGE_RECEIVED_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_R1BRIDGE_RECEIVED. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 174 CM_OPC_SV_R1BRIDGE_CONTROL                                            */
/**************************************************************************//**
@defgroup CM_OPC_SV_R1BRIDGE_CONTROL 174 CM_OPC_SV_R1BRIDGE_CONTROL
@ingroup cm-sv-opcodes-r1
@{
@brief The request CM_OPC_SV_R1BRIDGE_CONTROL signals the state of the bridge to CM.
@details
The request is allowed only after device-activation (background: with device-activation the PDEV-ownership is transferred from CM-PD to CM-SV; otherwise plug-alarms cannot parameterize the PDEV).

The initial bridge-state is "unknown" and RPC-Connects are rejected. The first bridge-control changes the bridge-state to "known" (either broken or established).

@note There are no negative responses such as CM_ERR_SEQUENCE or CM_ERR_PARAM because the application cannot handle them gracefully. A negative response means that the application is faulty. Thus it was decided that CM shall call FATAL (TIA 1495347).

## CM_SV_R1BRIDGE_CONTROL_CMD_BROKEN
Signals that the bridge is broken.
- Startup without partner (Hochlauf ohne Partner) - RPC-Connects are now accepted.
- Partner lost while running (Partnerverlust im Betrieb).
The request is confirmed only after all internal state-machines are idle. In other words: If "broken" is confirmed then another "established active/passive" may be issued immediately.

## CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_ACTIVE
Signals that the bridge is established (versions are exchanged).
The received Bridge-Version has to be passed in bridge_version[].
- This side is "active" (a.k.a. "master"); the other side will initiate the "update" (AuA).
- RPC-Connects are accepted after "update" is finished.
The request is confirmed immediately. In other words: If "established" is confirmed then "broken" may be issued immediately.

## CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_PASSIVE
Signals that the bridge is established (versions are exchanged).
The received Bridge-Version has to be passed in bridge_version[].
- This side is "passive" (a.k.a. "standby") and starts "updating" (AuA).
- RPC-Connects are accepted after "update" is finished.
The request is confirmed immediately. In other words: If "established" is confirmed then "broken" may be issued immediately.

@attention Known issues as of 10/2015: Invalid sequences such as "broken / broken" lead to FATAL (because the application cannot handle it other than calling FATAL either).

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/
#define CM_SV_R1BRIDGE_VERSION_OCT0  1 /**< V1 */
#define CM_SV_R1BRIDGE_VERSION_OCT1  0
#define CM_SV_R1BRIDGE_VERSION_OCT2  0
#define CM_SV_R1BRIDGE_VERSION_OCT3  0

#define CM_SV_R1BRIDGE_VERSION { CM_SV_R1BRIDGE_VERSION_OCT0, CM_SV_R1BRIDGE_VERSION_OCT1, CM_SV_R1BRIDGE_VERSION_OCT2, CM_SV_R1BRIDGE_VERSION_OCT3 } /**< version of CM bridge protocol */

#define CM_SV_R1BRIDGE_CONTROL_CMD_BROKEN               0 /**< APPL to CM: bridge is broken */
#define CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_ACTIVE   1 /**< APPL to CM: bridge is established; this side is "active" */
#define CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_PASSIVE  2 /**< APPL to CM: bridge is established; this side is "passive" and starts "update" */

typedef struct cm_sv_r1bridge_control_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 cmd;
	/**< ## REQ
	 * command, see CM_SV_R1BRIDGE_CONTROL_CMD_BROKEN, CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_ACTIVE, CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_PASSIVE
	 */

	LSA_UINT8 bridge_version[4];
	/**< ## REQ
	 * evaluated if cmd == CM_SV_R1BRIDGE_CONTROL_CMD_ESTABLISHED_ACTIVE/PASSIVE, not evaluated otherwise
	 */

} CM_SV_R1BRIDGE_CONTROL_TYPE;

typedef CM_SV_R1BRIDGE_CONTROL_TYPE * CM_UPPER_SV_R1BRIDGE_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_R1BRIDGE_CONTROL. */

typedef CM_SV_R1BRIDGE_CONTROL_TYPE const * CM_UPPER_SV_R1BRIDGE_CONTROL_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_SV_R1BRIDGE_CONTROL. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 120 CM_OPC_SV_AR_APPL_READY                                               */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_APPL_READY 120 CM_OPC_SV_AR_APPL_READY
@ingroup cm-sv-opcodes
@{
@brief With the request CM_OPC_SV_AR_APPL_READY the user signals the readiness (or non-readiness) of a set of submodules.
@details
CM_OPC_SV_AR_APPL_READY can be called only after PrmEnd.RSP.

@note: "application-ready-pending" can be changed later to "application-ready" by means of a return-of-submodule alarm. See @ref CM_OPC_SV_AR_ALARM_SEND.

@todo CM documentation: More text for "interruption of the prm-phase".

## RTClass1-AR and RTClass2-AR

The inputs must be updated before calling ApplReady.REQ.

## RTClass3-AR (CM_AR_TYPE_SINGLE_RTC3)

The inputs must not be updated until RIR.IND. See chapter [CM_OPC_SV_AR_RIR_IND](@ref CM_OPC_SV_AR_RIR_IND_RSP) for more information on RTClass3 startup.

## System Redundancy ARs

NAP-Type S2: See additional information in chapter @ref cm-server-channels-sysred-appl-ready. \n
NAP-Type R1: The things said for S2 apply to R1 too.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | (reported in elem[].response only!) Not an error. The parameterization was interrupted: the AR was aborted, or the AR is already disconnecting, or the submodule was pulled, or an IO-supervisor AR took over control, etc.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. AR is not connected (before connect.RSP or after disconnect.RSP).
CM_ERR_ELEM        | Must check elem[].response.
CM_ERR_PARAM       | Parameter(s) out-of-range. submodule is not in AR. nr_of_elems is zero.
******************************************************************************/

typedef struct cm_sv_ar_appl_ready_element_tag {

	LSA_UINT16 session_key;
	/** ## REQ
	 * session key, refer to @ref CM_OPC_SV_AR_CONNECT_IND
	 */

	LSA_UINT32 api;
	/** ## REQ
	 * application process identifier
	 */

	LSA_UINT16 slot_nr;
	/** ## REQ
	 * slot number
	 */

	LSA_UINT16 subslot_nr;
	/** ## REQ
	 * subslot number
	 */

	LSA_UINT8 appl_ready_pending;
	/** ## REQ
	 * CM_SV_APPL_READY_PENDING_YES .. the application is not-ready. \n
	 * CM_SV_APPL_READY_PENDING_NO ... the application is ready.
	 */

	LSA_UINT16 response;
	/** ## CNF
	 * Element specific response. Needs to be checked only if rqb.response == CM_ERR_ELEM.
	 * @note CM_OK_CANCELLED does not result in a CM_ERR_ELEM.
	 */

} CM_SV_AR_APPL_READY_ELEMENT_TYPE;

typedef CM_SV_AR_APPL_READY_ELEMENT_TYPE * CM_UPPER_SV_AR_APPL_READY_ELEMENT_PTR_TYPE; /**< The elem pointer type used for CM_OPC_SV_AR_APPL_READY. */

typedef CM_SV_AR_APPL_READY_ELEMENT_TYPE const * CM_UPPER_SV_AR_APPL_READY_ELEMENT_CONST_PTR_TYPE; /**< The elem onst pointer type used for CM_OPC_SV_AR_APPL_READY. */

typedef struct cm_sv_ar_appl_ready_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 nr_of_elems;
	/**< ## REQ
	 * Number of elements contained in the array "elem" (1..n).
	 */

	CM_SV_AR_APPL_READY_ELEMENT_TYPE elem[1];
	/**< ## REQ
	 * elements to add \n
	 * elem[2..n] follow here
	 */

} CM_SV_AR_APPL_READY_TYPE;

typedef CM_SV_AR_APPL_READY_TYPE * CM_UPPER_SV_AR_APPL_READY_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_APPL_READY. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* CM_OPC_SV_AR_PDEV_WRITE                                                   */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_PDEV_WRITE XXX CM_OPC_SV_AR_PDEV_WRITE
@ingroup cm-sv-opcodes
@brief Obsolete!
@details The functionality of CM_OPC_SV_AR_PDEV_WRITE was replaced by [CM_OPC_SV_AR_PDEVPRM_READ_WRITE](@ref CM_OPC_SV_AR_PDEVPRM_WRITE).
******************************************************************************/


/*****************************************************************************/
/* 122 CM_OPC_SV_AR_ABORT                                                   */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_ABORT 122 CM_OPC_SV_AR_ABORT
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_AR_ABORT aborts (disconnects) the AR with reason CM_AR_REASON_ABORT.
@details
The request is confirmed immediately. It only triggers the abortion of the AR. A disconnect-indication will follow.

@note The abortion of an AR may take some seconds due to the cancellation of running RPCs (ApplReady or ReadyForClass3 callback).

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | Not an error. The AR is already disconnecting.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. AR is not connected (before connect.RSP or after disconnect.RSP).
******************************************************************************/
typedef struct cm_sv_ar_abort_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 session_key;
	/**< ## REQ
	 * session key, refer to @ref CM_OPC_SV_AR_CONNECT_IND
	 */

} CM_SV_AR_ABORT_TYPE;

typedef CM_SV_AR_ABORT_TYPE * CM_UPPER_SV_AR_ABORT_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_ABORT. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 125 CM_OPC_SV_AR_RIR_ACK                                                 */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_RIR_ACK 125 CM_OPC_SV_AR_RIR_ACK
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_AR_RIR_ACK acknowledges the @ref CM_OPC_SV_AR_RIR_IND.
@details
Usage see chapter @ref CM_OPC_SV_AR_RIR_IND.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | Not an error. The AR is already disconnecting.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. AR is not connected (before connect.RSP or after disconnect.RSP).
******************************************************************************/
typedef struct cm_sv_ar_rir_ack_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 session_key;
	/**< ## REQ
	 * session key, refer to @ref CM_OPC_SV_AR_CONNECT_IND
	 */

} CM_SV_AR_RIR_ACK_TYPE;

typedef CM_SV_AR_RIR_ACK_TYPE * CM_UPPER_SV_AR_RIR_ACK_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_RIR_ACK. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 121 CM_OPC_SV_AR_CONTROL                                                  */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_CONTROL 121 CM_OPC_SV_AR_CONTROL
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_AR_CONTROL sets the (AR-specific) data-status of the AR's input-CRs (providers). The initial value after connect is XXX. @todo CM documentation: XXX?!
@details
The bit "station failure/ok" cannot be set by the user; it is controlled per AR by CM-SV.

The bit "primary/backup/redundancy" can be set only via synchronous EDDx functions. Please refer to the EDDx documentation.

The bit "run/stop" can be set only via CM_OPC_SV_CONTROL.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | Not an error. The AR is already disconnecting. The "cmd" is not executed because the providers are passive and executing it would have no effect on the Ethernet anyway.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. AR is not connected (before connect.RSP or after disconnect.RSP). Another request is still running.
CM_ERR_PARAM       | Parameter(s) out of range.
******************************************************************************/
#define CM_SV_AR_CONTROL_CMD_xxx   0 /**< currently no commands supported */
#define CM_SV_AR_CONTROL_CMD_MASK  0

typedef struct cm_sv_ar_control_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 session_key;
	/**< ## REQ
	 * session key, refer to @ref CM_OPC_SV_AR_CONNECT_IND
	 */

	LSA_UINT16 cmd;
	/**< ## REQ
	 * Currently no commands supported!
	 */

} CM_SV_AR_CONTROL_TYPE;

typedef CM_SV_AR_CONTROL_TYPE * CM_UPPER_SV_AR_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_CONTROL. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 165/166 CM_OPC_SV_AR_PDEVPRM_IND/_RSP                                     */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_PDEVPRM_IND_RSP 165/166 CM_OPC_SV_AR_PDEVPRM_IND/_RSP
@ingroup cm-sv-opcodes
@brief The indication CM_OPC_SV_AR_PDEVPRM_IND signals that the PDEV is about to be parameterized. The indication has to be responded.
@details
@see @ref CM_EVENT_TYPE

The indication has to be enabled by "PDevPrmInd_enabled" in CM_OPC_SV_DEVICE_ADD.

The indication is signaled even if the PDEV is superordinated locked (SO-locked). This is by design (TIA 1475690).
Note that manipulating the PDEV parameterization does not make sense in this case because it will be discarded later on.

The feature is intended to be used in a managed switch.

@note No parameters!

## Usecase:

In the window between CM_OPC_SV_AR_PDEVPRM_IND and CM_OPC_SV_AR_PDEVPRM_RSP the user has the possibility to issue the opcodes @ref CM_OPC_SV_AR_PDEVPRM_READ and @ref CM_OPC_SV_AR_PDEVPRM_WRITE to manipulate the PDEV parameterization.
Only 1 read/write is allowed at a time.

With CM_OPC_SV_AR_PDEVPRM_RSP the manipulated parameterization takes effect.

@note The result of the PDEV parameterization (success or failure) is signaled in the @ref CM_OPC_SV_AR_PRM_END_IND, see subchapter about PDEV submodules.

The following sequence diagram shows the embedding in the parameterization sequence.

![embedding in the parameterization sequence](cm-sv-parameterization-sequence.svg)

## Async issues

Due to the asynchronous nature of the RQB interface there is the possibility, that CM fires the indication and the user (in another thread context) issues e.g. an AR-abort.
The user has to handle the "late" indication gracefully.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
******************************************************************************/


/*****************************************************************************/
/* 167 CM_OPC_SV_AR_PDEVPRM_READ                                             */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_PDEVPRM_READ 167 CM_OPC_SV_AR_PDEVPRM_READ
@ingroup cm-sv-opcodes
@{
@brief With the request CM_OPC_SV_AR_PDEVPRM_READ the user can read records from the B-buffer in the window between CM_OPC_SV_AR_PDEVPRM_IND and CM_OPC_SV_AR_PDEVPRM_RSP.
@details
Reading "non-PRM" records is also possible. See the sequence diagram in chapter @ref CM_OPC_SV_AR_PDEVPRM_IND.

@attention Only 1 read/write is allowed at a time.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | Not an error. The AR is already disconnecting.
CM_ERR_SEQUENCE    | CM-SV is not created. evice is not added. AR is not connected (before connect.RSP or after disconnect.RSP). Another request is still running. Not within the PDEVPrm window.
CM_ERR_PARAM       | Parameter(s) out of range.
CM_ERR_PRM_xxx     | Reading the index failed.
******************************************************************************/
typedef struct cm_sv_ar_pdevprm_rdwr_tag {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 session_key;
	/**< ## REQ
	 * Session key, refer to @ref CM_OPC_SV_AR_CONNECT_IND
	 */

	CM_PD_PRM_RDWR_TYPE rec;
	/**
	 * see definition, implicit: api == 0
	 *
	 * ## write
	 * don't forget to specify the "rec.mk_remanent" flag accordingly (AP00738255)
	 * write: don't forget to specify the "rec.is_local" flag accordingly (AP00585108)
	 *
	 * "rec.cm_pnio_err", "rec.ret_val_1", and "rec.ret_val_2" always return 0 (do not evaluate them)
	 */

} CM_SV_AR_PDEVPRM_RDWR_TYPE;

typedef CM_SV_AR_PDEVPRM_RDWR_TYPE * CM_UPPER_SV_AR_PDEVPRM_RDWR_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_AR_PDEVPRM_READ and CM_OPC_SV_AR_PDEVPRM_WRITE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 168 CM_OPC_SV_AR_PDEVPRM_WRITE                                            */
/**************************************************************************//**
@defgroup CM_OPC_SV_AR_PDEVPRM_WRITE 168 CM_OPC_SV_AR_PDEVPRM_WRITE
@ingroup cm-sv-opcodes
@brief With the request CM_OPC_SV_AR_PDEVPRM_WRITE the user can write records to the B-buffer in the window between CM_OPC_SV_AR_PDEVPRM_IND and CM_OPC_SV_AR_PDEVPRM_RSP.
@details
Writing 0 bytes has a special meaning: Delete the record from the B-buffer. Writing "non-PRM" records is also possible.
See the sequence diagram in chapter @ref CM_OPC_SV_AR_PDEVPRM_IND.

@attention Only 1 read/write is allowed at a time!

@note If MRP records are manipulated (written or deleted) the flag "set_mrp_off" if set in @ref CM_OPC_SV_AR_OWNERSHIP_IND is ignored and no "MRP Off" record is generated by CM-PD.
The user must care for a potential need of PDInterfaceMrpDataAdjust with MRP_Role = OFF!

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | Not an error. The AR is already disconnecting.
CM_ERR_SEQUENCE    | CM-SV is not created. evice is not added. AR is not connected (before connect.RSP or after disconnect.RSP). Another request is still running. Not within the PDEVPrm window.
CM_ERR_PARAM       | Parameter(s) out of range.
CM_ERR_PRM_xxx     | Writing the index failed.
******************************************************************************/


/*****************************************************************************/
/* 118 CM_OPC_SV_ARSET_TRIGGER                                               */
/**************************************************************************//**
@defgroup CM_OPC_SV_ARSET_TRIGGER 118 CM_OPC_SV_ARSET_TRIGGER
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_ARSET_TRIGGER signals that an AR of the AR-Set changed its redundancy state (primary/backup). The request triggers the state-machines in CM-SV.
@details
Usage see chapter @ref cm-server-channels-sysred-data-status.

@note There is no explicit parameter "arset_nr", because arset_nr = CM_SV_SESSION_KEY-_TO_ARSET_NR(session_key).

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Another request is still running. CM-SV is not created.
******************************************************************************/

#define CM_SV_IS_PRIMARY_NO  0
#define CM_SV_IS_PRIMARY_YES 1

typedef struct cm_sv_arset_trigger {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 session_key;
	/**< ## REQ
	 * session key (arset_nr := CM_SV_SESSION_KEY_TO_ARSET_NR(session_key)), see @ref CM_OPC_SV_AR_CONNECT_IND
	 */

	LSA_UINT8 is_primary;
	/**< ## REQ
	 * CM_SV_IS_PRIMARY_YES .. "session_key" is the new primary AR. The request returns immediately.
	 * CM_SV_IS_PRIMARY_NO ... "session_key" no longer is the primary AR. The request returns only after pending alarms (prio high and low) have been sent.
	 */

} CM_SV_ARSET_TRIGGER_TYPE;

typedef CM_SV_ARSET_TRIGGER_TYPE * CM_UPPER_SV_ARSET_TRIGGER_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_ARSET_TRIGGER. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 119 CM_OPC_SV_ARSET_ABORT                                                 */
/**************************************************************************//**
@defgroup CM_OPC_SV_ARSET_ABORT 119 CM_OPC_SV_ARSET_ABORT
@ingroup cm-sv-opcodes
@{
@brief The request CM_OPC_SV_ARSET_ABORT aborts all ARs of an AR-Set with reason CM_AR_REASON_RDHT.
@details
Usage see chapter @ref cm-server-channels-sysred-arset-abort.

## Parallelism

@see @ref cm-sv-parallelism

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_OK_CANCELLED    | The AR-set is empty.
CM_ERR_SEQUENCE    | CM-SV is not created. Device is not added. Another request is still running.
CM_ERR_PARAM       | Parameter(s) out-of-range. AR is not an IOCARSR.
******************************************************************************/
typedef struct cm_sv_arset_abort {

	LSA_UINT16 device_nr;
	/**< ## REQ
	 * device number
	 */

	LSA_UINT16 arset_nr;  /* AR-set number (see CM_SV_SESSION_KEY_TO_ARSET_NR() too) */

} CM_SV_ARSET_ABORT_TYPE;

typedef CM_SV_ARSET_ABORT_TYPE * CM_UPPER_SV_ARSET_ABORT_PTR_TYPE; /**< The pointer type used for CM_OPC_SV_ARSET_ABORT. */
/** @} ***********************************************************************/


/******************************************************************************
end of server opcodes
******************************************************************************/
#endif


/******************************************************************************
begin of multicast opcodes
******************************************************************************/
#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE

/*****************************************************************************/
/* 060 CM_OPC_MC_CREATE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_MC_CREATE 060 CM_OPC_MC_CREATE
@ingroup cm-mc-opcodes
@{
@brief The "multicast channel" (refer to Channel Details, cm_get_path_info()) is configured as such with the CreateMulticast function.
@details
Resources for "max_devices" devices will be allocated.

CreateMulticast sets the channel-global APDU status on the default value (primary, run and station Ok).

## Connection with CM-PD:
CM_OPC_PD_CREATE must have been executed before CM_OPC_MC_CREATE.

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Instance was created.
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_PARAM       | Parameter out-of-range.
******************************************************************************/
#define CM_MC_MAX_DEVICE_NR 2047 /**< max StationNumber from ES */

typedef struct cm_mc_create_tag {

	LSA_UINT16		max_devices;
	/**< ## REQ
	 * max number of managing devices: 1 .. CM_MC_MAX_DEVICE_NR
	 */

	LSA_UINT32		hardware_type;
	/**< ## CNF
	 * see EDD_HW_TYPE_USED_MSK etc.
	 */

	LSA_UINT16		interface_id;
	/**< ## CNF
	 * 1..16
	 * see EDD_SRV_GET_PARAMS::InterfaceID
	 */

} CM_MC_CREATE_TYPE;

typedef CM_MC_CREATE_TYPE * CM_UPPER_MC_CREATE_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_CREATE. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 061 CM_OPC_MC_DELETE                                                      */
/**************************************************************************//**
@defgroup CM_OPC_MC_DELETE 061 CM_OPC_MC_DELETE
@ingroup cm-mc-opcodes
@{
@brief The DeleteMulticast function "deletes" a multicast instance.
@details
All devices must first be removed, before a multicast instance can be deleted.

@note No parameters!

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Instance was deleted.
CM_ERR_SEQUENCE    | Call sequence not as specified..
******************************************************************************/
typedef LSA_VOID * CM_UPPER_MC_DELETE_PTR_TYPE;
/** @} ***********************************************************************/


/*****************************************************************************/
/* 062 CM_OPC_MC_CONTROL                                                     */
/**************************************************************************//**
@defgroup CM_OPC_MC_CONTROL 062 CM_OPC_MC_CONTROL
@ingroup cm-mc-opcodes
@{
@brief Activate, Passivate, Set APDU Status
@details
Alias for @ref CM_OPC_CL_CONTROL

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Instance started running or was stopped.
CM_PARAM           | Invalid control_tag passed.
CM_ERR_SEQUENCE	   | Another CM_OPC_MC_CONTROL is pending.
******************************************************************************/
typedef CM_CL_CONTROL_TYPE CM_MC_CONTROL_TYPE;

typedef CM_MC_CONTROL_TYPE * CM_UPPER_MC_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_CONTROL. @see CM_CL_CONTROL_PTR_TYPE */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 063 CM_OPC_MC_DEVICE_ADD                                                  */
/**************************************************************************//**
@defgroup CM_OPC_MC_DEVICE_ADD 063 CM_OPC_MC_DEVICE_ADD
@ingroup cm-mc-opcodes
@{
@brief A new multicast endpoint is setup with the AddMulticastDevice function (M-provider or M-consumer). The interface resembles the CM client AddDevice.
@details
The PDEV must be parameterized before AddMulticastDevice (otherwise: ERR_SEQUENCE). If the PDEV SendClock is not equal to the IOCR SendClock (CM_ARCB_IOCR_TYPE::send_clock), then the "local adaption" must be used (CM_ARCB_ADD_TYPE::iocr_local_list).

If the PDEV is parameterized as MasterProject then this service is rejected with CM_ERR_SEQUENCE.

The "device_nr" is assigned by the user. The device is addressed through this "device_nr".
The valid range of the device_nr lies between 1 and CM_MC_MAX_DEVICE_NR. The count of the available resources is given by CM_MC_CREATE::max_devices.

@note The multicast state machine is documented in [/9d/](@ref cm-ref-no-9d).

## Properties

@note see @ref CM_OPC_CL_DEVICE_ADD
@param CM_MC_DEVICE_ADD_TYPE::device_nr               **REQ** device number
@param CM_MC_DEVICE_ADD_TYPE::pnio_status             **REQ** don't care \n
													  **CNF** In case of CM_ERR_PARAM error, a PNIO - conforming description of the error location is contained here, provided the error was found inside an AR.
@param CM_MC_DEVICE_ADD_TYPE::nr_of_event_resources   **REQ** don't care \n
													  **CNF** Number of event-resources the CM-user must provide with @ref CM_OPC_MC_DEVICE_PROVIDE_EVENT.
@param CM_MC_DEVICE_ADD_TYPE::nr_of_alarm_resources   **REQ** don't care \n
													  **CNF** 0 (multicast devices don't have alarms).
@param CM_MC_DEVICE_ADD_TYPE::arcb                    **REQ** Pointer to AR control block @ref CM_ARCB_ADD_TYPE.
@param CM_MC_DEVICE_ADD_TYPE::arcb.ar_nr              **REQ** AR Number (1..255) \n
															  This number is passed to the CM_OPC_MC_EVENT_* opcodes.\n
															  Set to 1 if not used in the event handling.
@param CM_MC_DEVICE_ADD_TYPE::arcb.iocr_list          **REQ** \n
															  M-provider:\n
															  List with 1..n multicast-provider CRs.\n
															  M-consumer:\n
															  List with exactly one multicast-consumer CR.\n
															  A corresponding entry ("iocr_ref") must be available in "mcr_list".
@param CM_MC_DEVICE_ADD_TYPE::arcb.iocr_local_list    **REQ** Local adaption for "iocr_list". See CM client AddDevice @ref CM_OPC_CL_DEVICE_ADD.
@param CM_MC_DEVICE_ADD_TYPE::arcb.mcr_list           **REQ** \n
															  M-provider: empty list\n
															  M-consumer: List with one entry containing the parameters of the MCRBlockReq.
@param rest                                           don't care

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Device was added.
CM_ERR_PARAM       | Parameter is wrong (e.g., out-of-range)
CM_ERR_SEQUENCE    | Call sequence not as specified.
CM_ERR_ALLOC_LOCAL | Not enough local memory.
CM_ERR_LOWER_LAYER | Provider or consumer could not be added.
******************************************************************************/
typedef CM_CL_DEVICE_ADD_TYPE CM_MC_DEVICE_ADD_TYPE;

typedef CM_MC_DEVICE_ADD_TYPE * CM_UPPER_MC_DEVICE_ADD_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_DEVICE_ADD. */

typedef CM_MC_DEVICE_ADD_TYPE const * CM_UPPER_MC_DEVICE_ADD_CONST_PTR_TYPE; /**< The const pointer type used for CM_OPC_MC_DEVICE_ADD. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 064 CM_OPC_MC_DEVICE_REMOVE                                               */
/**************************************************************************//**
@defgroup CM_OPC_MC_DEVICE_REMOVE 064 CM_OPC_MC_DEVICE_REMOVE
@ingroup cm-mc-opcodes
@{
@brief A device can be removed again with the help of the RemoveMulticastDevice function.
@details
The device does not need to be passivated before.

"Offline" is reported before Remove.CNF if "In Data" was reported.

@note see @ref CM_OPC_CL_DEVICE_REMOVE

## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Device was removed.
CM_ERR_PARAM       | Parameter wrong (e.g., out-of-range)
CM_ERR_SEQUENCE    | Call sequence not as specified.
******************************************************************************/
typedef CM_CL_DEVICE_REMOVE_TYPE CM_MC_DEVICE_REMOVE_TYPE;

typedef CM_MC_DEVICE_REMOVE_TYPE * CM_UPPER_MC_DEVICE_REMOVE_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_DEVICE_ADD. */
/** @} ***********************************************************************/


/*****************************************************************************/
/* 065 CM_OPC_MC_DEVICE_CONTROL                                              */
/**************************************************************************//**
@defgroup CM_OPC_MC_DEVICE_CONTROL 065 CM_OPC_MC_DEVICE_CONTROL
@ingroup cm-mc-opcodes
@{
@brief The ControlDevice function can be used to activate, passivate or abort the devices.
@details
Alias for @ref CM_OPC_CL_DEVICE_CONTROL.

@note Use CM_CL_DEVICE_CONTROL_PASSIVATE and CM_CL_DEVICE_CONTROL_ACTIVATE

@see CM_CL_DEVICE_CONTROL_TYPE

@param CM_MC_DEVICE_CONTROL_TYPE::device_nr   **REQ** device number
@param CM_MC_DEVICE_CONTROL_TYPE::control_tag **REQ** Refer to cm_cl_device_control_enum.
													  Allowed values:
													  - CM_CL_DEVICE_CONTROL_PASSIVATE
													  - CM_CL_DEVICE_CONTROL_ACTIVATE
													  - CM_CL_DEVICE_CONTROL_APDU_STATUS
@param CM_MC_DEVICE_CONTROL_TYPE::apdu_status **REQ** Valid if control_tag = CM_CL_DEVICE_CONTROL_APDU_STATUS. Refer to cm_cl_ar_control_enum.
													  Allowed values:
													  - CM_CL_AR_CONTROL_STATION_OK
													  - CM_CL_AR_CONTROL_STATION_PROBLEM
## Result of the request

Return Code        | Meaning
-------------------|--------------------------------
CM_OK              | Success.
CM_ERR_PARAM       | Parameter wrong (e.g., out-of-range).
CM_ERR_SEQUENCE    | Call sequence not as specified.
******************************************************************************/
typedef CM_CL_DEVICE_CONTROL_TYPE CM_MC_DEVICE_CONTROL_TYPE;

typedef CM_MC_DEVICE_CONTROL_TYPE * CM_UPPER_MC_DEVICE_CONTROL_PTR_TYPE; /**< The pointer type used for CM_OPC_MC_DEVICE_CONTROL. */
/** @} ***********************************************************************/


/******************************************************************************
end of multicast opcodes
******************************************************************************/
#endif


/**************************************************************************//**
@defgroup cm-rqbs-group CM RQBs
@ingroup cm-group
@brief The configuration options and the input functions are listed in the following pages according to [/1/](@ref cm-ref-no-1).
******************************************************************************/


/*****************************************************************************/
/* CM_RQB_TYPE                                                               */
/**************************************************************************//**
@defgroup CM_RQB_TYPE CM_RQB_TYPE
@ingroup cm-rqbs-group
@{
@brief CM ReQuestBlock.
@details A CM request block consists of a header (CM_RQB_TYPE) and opcode-specific parameters that must be allocated separately and attached in the 'args' field (union of pointers)."
@see @ref cm-rqbs-rqb-header and CM_RQB_TYPE::CM_RQB_HEADER and @ref CM_RQB_TYPE::CM_RQB_TRAILER respectively
******************************************************************************/
typedef union cm_any_args_tag { /* may be useful: one for all... */

	union {
		CM_OPEN_CHANNEL_TYPE  open;
		/*CM_CLOSE_CHANNEL_TYPE	close; no parameters, no type! */
	} channel;

	union {
		CM_PD_CREATE_TYPE  create;
		/*CM_PD_DELETE_TYPE  d_lete; no parameters, no type! */
		CM_PD_REMAP_TYPE     remap;
		/***/
		CM_PD_LINK_STATUS_INFO_TYPE  link_status;
		CM_PD_SYNC_LED_INFO_TYPE     sync_led_info;
		/***/
		CM_EVENT_TYPE  event;
		CM_ALARM_TYPE  alarm;
		/***/
		CM_PD_SUBMODULE_CONTROL_TYPE submodule_control;
		CM_PD_DIAG_ADD_TYPE          diag_add;
		CM_PD_DIAG_REMOVE_TYPE       diag_remove;
		/***/
		CM_PD_PRM_BEGIN_TYPE         prm_begin;
		CM_PD_PRM_RDWR_TYPE          prm_rdwr;
		CM_PD_PRM_END_TYPE           prm_end;
		CM_PD_PRM_RELINQUISH_TYPE    prm_relinquish; /* internal */
		/***/
		CM_PD_PTCP_MASTER_CONTROL_TYPE  ptcp_master_control;
	} pd;

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
	union {
		CM_CL_CREATE_TYPE  create;
		/*CM_CL_DELETE_TYPE  d_lete; no parameters, no type! */
		CM_CL_CONTROL_TYPE  control;

		CM_CL_DEVICE_ADD_TYPE      dev_add;
		CM_CL_DEVICE_REMOVE_TYPE   dev_remove;
		CM_CL_DEVICE_CONTROL_TYPE  dev_control;

		CM_EVENT_TYPE  event_ind;

		CM_ALARM_TYPE  alarm_send;
		CM_ALARM_TYPE  alarm;

		CM_CL_RECORD_READ_TYPE            read;
		CM_CL_RECORD_WRITE_TYPE           write;

		CM_CL_LOGBOOK_READ_TYPE           logbook_read;

		CM_CL_PRM_UPDATE_TYPE  prm_update;
	} cl;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE
	union {
		CM_MC_CREATE_TYPE  create;
		/*CM_MC_DELETE_TYPE  d_lete; no parameters, no type! */
		CM_MC_CONTROL_TYPE  control;
		/***/
		CM_MC_DEVICE_ADD_TYPE      dev_add;
		CM_MC_DEVICE_REMOVE_TYPE   dev_remove;
		CM_MC_DEVICE_CONTROL_TYPE  dev_control;
		/***/
		CM_EVENT_TYPE  event_ind;
	} mc;
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
	union {
		CM_SV_CREATE_TYPE   create;
		/*CM_SV_DELETE_TYPE d_lete; no parameters, no type! */
		CM_SV_CONTROL_TYPE  control;
		/***/
		CM_SV_DEVICE_ADD_TYPE      dev_add;
		CM_SV_DEVICE_REMOVE_TYPE   dev_remove;
		CM_SV_DEVICE_CONTROL_TYPE  dev_control;
		CM_SV_DEVICE_LED_INFO_TYPE dev_led_info;
		CM_EVENT_TYPE              dev_provide_event;
		CM_ALARM_TYPE              dev_provide_alarm; /* ALMR */
		CM_SV_DEVICE_READ_TYPE     dev_read;
		/***/
		CM_SV_R1BRIDGE_SEND_TYPE     r1bridge_send;
		CM_SV_R1BRIDGE_RECEIVED_TYPE r1bridge_received;
		CM_SV_R1BRIDGE_CONTROL_TYPE  r1bridge_control;
		/***/
		CM_SV_SUBMODULE_ADD_TYPE    submod_add;
		CM_SV_SUBMODULE_REMOVE_TYPE submod_remove;
		CM_SV_DIAG_ADD_TYPE         diag_add;
		CM_SV_DIAG_REMOVE_TYPE      diag_remove;
		/***/
		CM_SV_ARSET_TRIGGER_TYPE arset_trigger;
		CM_SV_ARSET_ABORT_TYPE   arset_abort;
		/***/
		CM_EVENT_TYPE            ar_event;
		CM_SV_AR_ABORT_TYPE      ar_abort;
		CM_SV_AR_RIR_ACK_TYPE    ar_rir_ack;
		CM_SV_AR_APPL_READY_TYPE ar_appl_ready;
		CM_SV_AR_CONTROL_TYPE    ar_control;
		/***/
		CM_ALARM_TYPE            ar_alarm_send; /* ALMI */
		/***/
		CM_ALARM_TYPE            ar_alarm_ind; /* ALMR */
		CM_ALARM_TYPE            ar_alarm_ack; /* ALMR */
		/***/
		CM_SV_AR_PDEVPRM_RDWR_TYPE ar_pdevprm_rdwr;
	} sv;
#endif

} CM_ANY_ARGS_TYPE;

typedef CM_ANY_ARGS_TYPE * CM_UPPER_ANY_ARGS_PTR_TYPE;


#define CM_RQB_SET_ARGS(rqb_, v_)  {(rqb_)->args.void_ptr = (v_);}
#define CM_RQB_GET_ARGS(rqb_)      ((rqb_)->args.void_ptr)


typedef struct cm_rqb_tag {

	CM_RQB_HEADER

	union {

		union {
			CM_UPPER_OPEN_CHANNEL_PTR_TYPE   open;
			CM_UPPER_CLOSE_CHANNEL_PTR_TYPE  close;
			/***/
		} channel;

		union {
			CM_UPPER_PD_CREATE_PTR_TYPE  create;
			CM_UPPER_PD_DELETE_PTR_TYPE  d_lete; /**< @note sic! delete is a C++ keyword */
			CM_UPPER_PD_REMAP_PTR_TYPE   remap;
			/***/
			CM_UPPER_PD_LINK_STATUS_INFO_PTR_TYPE  link_status;
			CM_UPPER_PD_SYNC_LED_INFO_PTR_TYPE     sync_led_info;
			/***/
			CM_UPPER_ALARM_PTR_TYPE  alarm;
			CM_UPPER_EVENT_PTR_TYPE  event;
			/***/
			CM_UPPER_PD_SUBMODULE_CONTROL_PTR_TYPE submodule_control;
			CM_UPPER_PD_DIAG_ADD_PTR_TYPE          diag_add;
			CM_UPPER_PD_DIAG_REMOVE_PTR_TYPE       diag_remove;
			/***/
			CM_UPPER_PD_PRM_BEGIN_PTR_TYPE       prm_begin;
			CM_UPPER_PD_PRM_RDWR_PTR_TYPE        prm_rdwr;
			CM_UPPER_PD_PRM_END_PTR_TYPE         prm_end;
			CM_UPPER_PD_PRM_RELINQUISH_PTR_TYPE  prm_relinquish; /**< @note internal */
			/***/
			CM_UPPER_PD_PTCP_MASTER_CONTROL_PTR_TYPE  ptcp_master_control;
		} pd;

#if CM_CFG_MAX_CLIENTS || CM_CFG_EVERYTHING_VISIBLE
		union {
			CM_UPPER_CL_CREATE_PTR_TYPE   create;
			CM_UPPER_CL_DELETE_PTR_TYPE   d_lete; /**< @note sic! delete is a C++ keyword */
			CM_UPPER_CL_CONTROL_PTR_TYPE  control;
			/***/
			CM_UPPER_CL_DEVICE_ADD_PTR_TYPE      dev_add;
			CM_UPPER_CL_DEVICE_REMOVE_PTR_TYPE   dev_remove;
			CM_UPPER_CL_DEVICE_CONTROL_PTR_TYPE  dev_control;
			/***/
			CM_UPPER_EVENT_PTR_TYPE  event_ind;
			/***/
			CM_UPPER_ALARM_PTR_TYPE  alarm_send;
			CM_UPPER_ALARM_PTR_TYPE  alarm;
			/***/
			CM_UPPER_CL_RECORD_READ_PTR_TYPE            read;
			CM_UPPER_CL_RECORD_WRITE_PTR_TYPE           write;
			/***/
			CM_UPPER_CL_LOGBOOK_READ_PTR_TYPE           logbook_read;
			/***/
			CM_UPPER_CL_PRM_UPDATE_PTR_TYPE  prm_update;
		} cl;
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS || CM_CFG_EVERYTHING_VISIBLE
		union {
			CM_UPPER_MC_CREATE_PTR_TYPE   create;
			CM_UPPER_MC_DELETE_PTR_TYPE   d_lete; /**< @note sic! delete is a C++ keyword */
			CM_UPPER_MC_CONTROL_PTR_TYPE  control;
			/***/
			CM_UPPER_MC_DEVICE_ADD_PTR_TYPE      dev_add;
			CM_UPPER_MC_DEVICE_REMOVE_PTR_TYPE   dev_remove;
			CM_UPPER_MC_DEVICE_CONTROL_PTR_TYPE  dev_control;
			/***/
			CM_UPPER_EVENT_PTR_TYPE  event_ind;
		} mc;
#endif

#if CM_CFG_MAX_SERVERS || CM_CFG_EVERYTHING_VISIBLE
		union {
			CM_UPPER_SV_CREATE_PTR_TYPE  create;
			CM_UPPER_SV_DELETE_PTR_TYPE  d_lete; /**< @note sic! delete is a C++ keyword */
			CM_UPPER_SV_CONTROL_PTR_TYPE control;
			/***/
			CM_UPPER_SV_DEVICE_ADD_PTR_TYPE      dev_add;
			CM_UPPER_SV_DEVICE_REMOVE_PTR_TYPE   dev_remove;
			CM_UPPER_SV_DEVICE_CONTROL_PTR_TYPE  dev_control;
			CM_UPPER_SV_DEVICE_LED_INFO_PTR_TYPE dev_led_info;
			CM_UPPER_EVENT_PTR_TYPE              dev_provide_event;
			CM_UPPER_ALARM_PTR_TYPE              dev_provide_alarm; /**< ALMR */
			CM_UPPER_SV_DEVICE_READ_PTR_TYPE     dev_read;
			/***/
			CM_UPPER_SV_R1BRIDGE_SEND_PTR_TYPE     r1bridge_send;
			CM_UPPER_SV_R1BRIDGE_RECEIVED_PTR_TYPE r1bridge_received;
			CM_UPPER_SV_R1BRIDGE_CONTROL_PTR_TYPE  r1bridge_control;
			/***/
			CM_UPPER_SV_SUBMODULE_ADD_PTR_TYPE    submod_add;
			CM_UPPER_SV_SUBMODULE_REMOVE_PTR_TYPE submod_remove;
			CM_UPPER_SV_DIAG_ADD_PTR_TYPE         diag_add;
			CM_UPPER_SV_DIAG_REMOVE_PTR_TYPE      diag_remove;
			/***/
			CM_UPPER_SV_ARSET_TRIGGER_PTR_TYPE arset_trigger;
			CM_UPPER_SV_ARSET_ABORT_PTR_TYPE   arset_abort;
			/***/
			CM_UPPER_EVENT_PTR_TYPE            ar_event;
			CM_UPPER_SV_AR_ABORT_PTR_TYPE      ar_abort;
			CM_UPPER_SV_AR_RIR_ACK_PTR_TYPE    ar_rir_ack;
			CM_UPPER_SV_AR_APPL_READY_PTR_TYPE ar_appl_ready;
			CM_UPPER_SV_AR_CONTROL_PTR_TYPE    ar_control;
			/***/
			CM_UPPER_ALARM_PTR_TYPE            ar_alarm_send; /**< ALMI */
			/***/
			CM_UPPER_ALARM_PTR_TYPE            ar_alarm_ind; /**< ALMR */
			CM_UPPER_ALARM_PTR_TYPE            ar_alarm_ack; /**< ALMR */
			/***/
			CM_UPPER_SV_AR_PDEVPRM_RDWR_PTR_TYPE ar_pdevprm_rdwr;
		} sv;
#endif

		LSA_VOID * void_ptr;  /**< see CM_RQB_SET_ARGS/CM_RQB_GET_ARGS */

	} args;
	/**< Arguments corresponding to the opcode.
	 * The user must enter a pointer to a memory range that was populated with parameters according to the 'opcode' in the "args" data entry field
	 */

	CM_RQB_TRAILER /**< CM_RQB_TRAILER - last in struct, see cm_cfg.h */

} CM_RQB_TYPE;
/** @} ***********************************************************************/

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Opening a new communication channel for CM components.
@details
Asynchronous, according to [/1/](@ref cm-ref-no-1).

For description of the RQB, please refer to [chapter 6.2](@ref CM_OPC_OPEN_CHANNEL).

@include cm_open_channel_example.c

@param [in] upper_rqb_ptr Pointer on request block
@return None
******************************************************************************/
LSA_VOID
cm_open_channel(
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Closing an opened communication channel with the help of cm_open_channel.
@details
Asynchronous, according to [/1/](@ref cm-ref-no-1).

For description of the RQB, please refer to [chapter 6.2](@ref CM_OPC_CLOSE_CHANNEL).

@include cm_close_channel_example.c

@param [in] upper_rqb_ptr Pointer on request block
@return None
******************************************************************************/
LSA_VOID
cm_close_channel(
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Processing requests for CM.
@details
Asynchronous, according to [/1/](@ref cm-ref-no-1).

The opcodes of the request function are described in [chapter 5](@ref cm-rqbs).

The function accepts a request block from the upper level for processing.

The response is transferred asynchronously to the caller with the help of the callback function disclosed in cm_open_channel.

The CM_FATAL function is called if the RQB cannot be evaluated (e.g.: NULL pointer).

The CM_FATAL_ERROR output function is called if an error situation cannot be reported to the upper layer.

@include cm_request_example.c

@param [in] upper_rqb_ptr Pointer on request block
@return None
******************************************************************************/
LSA_VOID
cm_request(
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


#if 0 /* not used */
/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Asynchronous allocation of an upper-RQB.
@details Not used.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_rqb_ptr pointer to upper RQB
******************************************************************************/
LSA_VOID cm_upper_rqb(
	LSA_USER_ID_TYPE          user_id,
	LSA_UINT16               length,
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


#if 0 /* not used */
/**************************************************************************//**
@ingroup cm-product-components-input-functions
@brief Asynchronous allocation of an upper-memory.
@details Not used.

@param [in]  user_id       user id
@param [in]  length        length
@param [out] upper_mem_ptr pointer to upper memory
******************************************************************************/
LSA_VOID cm_upper_mem(
	LSA_USER_ID_TYPE          user_id,
	LSA_UINT16               length,
	CM_UPPER_MEM_PTR_TYPE  upper_mem_ptr
);
#endif


/*====  out functions  =====*/

/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Allocate an upper-RQB.
@details
Requesting a request block with the memory attributes of the upper layer.

Alignment according to request of processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface.

CM also allocates upper RQBs for Internal purposes. However, these are never forwarded to the user!

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *upper_rqb_ptr_ptr upper RQB pointer
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
#ifndef CM_ALLOC_UPPER_RQB
LSA_VOID
CM_ALLOC_UPPER_RQB(
	CM_UPPER_RQB_PTR_TYPE * upper_rqb_ptr_ptr,
	LSA_USER_ID_TYPE        user_id,
	LSA_UINT16             length,
	LSA_SYS_PTR_TYPE        sys_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a requested request block with CM_ALLOC_UPPER_RQB.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr upper return value
@param [in]  upper_rqb_ptr upper RQB pointer
@param [in]  sys_ptr       system pointer
******************************************************************************/
#ifndef CM_FREE_UPPER_RQB
LSA_VOID
CM_FREE_UPPER_RQB(
	LSA_UINT16           * ret_val_ptr,
	CM_UPPER_RQB_PTR_TYPE   upper_rqb_ptr,
	LSA_SYS_PTR_TYPE        sys_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Requesting memory with the memory attributes of the upper layer.
@details
Alignment according to request of processor, e.g., 32-bit for TriCore.

The memory must be accessible from the upper interface.

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *upper_mem_ptr_ptr upper memory pointer
@param [in]  user_id            user id
@param [in]  length             alloc length
@param [in]  sys_ptr            system pointer
******************************************************************************/
#ifndef CM_ALLOC_UPPER_MEM
LSA_VOID
CM_ALLOC_UPPER_MEM(
	CM_UPPER_MEM_PTR_TYPE    *  upper_mem_ptr_ptr,
	LSA_USER_ID_TYPE         user_id,
	LSA_UINT32               length,
	LSA_SYS_PTR_TYPE         sys_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Release of a requested memory with CM_ALLOC_UPPER_MEM.
@details
An LSA_RET_xxx must be returned in ret_val_ptr, refer to [/1/](@ref cm-ref-no-1).

Synchronous, according to [/1/](@ref cm-ref-no-1).

@param [out] *ret_val_ptr upper return value
@param [in]  upper_mem_ptr      user id
@param [in]  sys_ptr            system pointer
******************************************************************************/
#ifndef CM_FREE_UPPER_MEM
LSA_VOID
CM_FREE_UPPER_MEM(
	LSA_UINT16               *  ret_val_ptr,
	CM_UPPER_MEM_PTR_TYPE     upper_mem_ptr,
	LSA_SYS_PTR_TYPE          sys_ptr
);
#endif


/**************************************************************************//**
@ingroup cm-product-components-output-functions
@brief Send a request to "self".
@details
Use during the synchronization of the LSA timer callbacks with the CM task context.
Refer to @ref cm_timeout().

@param [in] upper_rqb_ptr upper RQB pointer
******************************************************************************/
#ifndef CM_REQUEST_LOCAL
LSA_VOID
CM_REQUEST_LOCAL(
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_USR_H */
