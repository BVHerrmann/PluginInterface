#ifndef CM_CFG_H
#define CM_CFG_H
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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: cm_cfg.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CM using PSI                            */
/*                                                                           */
/*****************************************************************************/

/*
 * included by "cm_inc.h"
 */

void cm_psi_startstop (int start);

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#define CM_DEBUG  PSI_DEBUG

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/


/*===========================================================================*/
/* CM-HEADER and CM-TRAILER                                                  */
/*===========================================================================*/

#if ! defined(LSA_RQB_HEADER) || ! defined(LSA_RQB_TRAILER)
# error "LSA_RQB_HEADER or LSA_RQB_TRAILER not defined in lsa_cfg.h"
#endif


#define CM_RQB_HEADER \
	LSA_RQB_HEADER(CM_UPPER_RQB_PTR_TYPE) /* standard LSA-header comes first */ \
	LSA_UINT32 hif_helper; \
	/* end of CM_RQB_HEADER */


#define CM_RQB_TRAILER \
	LSA_RQB_TRAILER /* standard LSA-trailer comes last */ \
	/* end of CM_RQB_TRAILER */


#define CM_RQB_SET_NEXT_RQB_PTR(rb_, val_)		LSA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define CM_RQB_GET_NEXT_RQB_PTR(rb_)			LSA_RQB_GET_NEXT_RQB_PTR(rb_)

#define CM_RQB_SET_PREV_RQB_PTR(rb_, val_)		LSA_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define CM_RQB_GET_PREV_RQB_PTR(rb_)			LSA_RQB_GET_PREV_RQB_PTR(rb_)

#define CM_RQB_SET_OPCODE(rb_, val_)			LSA_RQB_SET_OPCODE(rb_, val_)
#define CM_RQB_GET_OPCODE(rb_)					LSA_RQB_GET_OPCODE(rb_)

#define CM_RQB_SET_HANDLE(rb_, val_)			LSA_RQB_SET_HANDLE(rb_, val_)
#define CM_RQB_GET_HANDLE(rb_)					LSA_RQB_GET_HANDLE(rb_)

#define CM_RQB_SET_USER_ID_PTR(rb_, val_)		LSA_RQB_SET_USER_ID_PTR(rb_, val_)
#define CM_RQB_GET_USER_ID_PTR(rb_)				LSA_RQB_GET_USER_ID_PTR(rb_)

#define CM_RQB_SET_RESPONSE(rqb_, v_)			LSA_RQB_SET_RESPONSE(rqb_, v_)
#define CM_RQB_GET_RESPONSE(rqb_)				LSA_RQB_GET_RESPONSE(rqb_)


/*===========================================================================*/
/* LOWER LAYER MACROS                                                        */
/*===========================================================================*/

/*
 * ACP
 */

#define CM_ACP_LOWER_RQB_TYPE				ACP_RQB_TYPE

#define CM_ACP_SET_NEXT_RQB_PTR(rb_, val_)	ACP_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define CM_ACP_SET_PREV_RQB_PTR(rb_, val_)	ACP_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define CM_ACP_SET_OPCODE(rb_, val_)		ACP_RQB_SET_OPCODE(rb_, val_)
#define CM_ACP_SET_HANDLE(rb_, val_)		ACP_RQB_SET_HANDLE(rb_, val_)
#define CM_ACP_SET_USER_ID_PTR(rb_, val_)	ACP_RQB_SET_USER_ID_PTR(rb_, val_)
#define CM_ACP_SET_RESPONSE(rb_, val_)		ACP_RQB_SET_RESPONSE(rb_, val_)

#define CM_ACP_GET_NEXT_RQB_PTR(rb_)		ACP_RQB_GET_NEXT_RQB_PTR(rb_)
#define CM_ACP_GET_PREV_RQB_PTR(rb_)		ACP_RQB_GET_PREV_RQB_PTR(rb_)
#define CM_ACP_GET_OPCODE(rb_)				ACP_RQB_GET_OPCODE(rb_)
#define CM_ACP_GET_HANDLE(rb_)				ACP_RQB_GET_HANDLE(rb_)
#define CM_ACP_GET_USER_ID_PTR(rb_)			ACP_RQB_GET_USER_ID_PTR(rb_)
#define CM_ACP_GET_RESPONSE(rb_)			ACP_RQB_GET_RESPONSE(rb_)

#define CM_ACP_SET_ARGS(rb_, val_)			ACP_RQB_SET_ARGS(rb_, val_)
#define CM_ACP_GET_ARGS(rb_)				ACP_RQB_GET_ARGS(rb_)

/*
 * EDD
 */

#define CM_EDD_LOWER_RQB_TYPE	EDD_RQB_TYPE

# define CM_EDD_SET_NEXT_RQB_PTR(rb_, val_)	EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
# define CM_EDD_SET_PREV_RQB_PTR(rb_, val_)	EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
# define CM_EDD_SET_OPCODE(rb_, opc_, svc_)	{ EDD_RQB_SET_OPCODE(rb_, opc_); EDD_RQB_SET_SERVICE(rb_, svc_); }
# define CM_EDD_SET_HANDLE(rb_, val_)		EDD_RQB_SET_HANDLE(rb_, val_)
# define CM_EDD_SET_USER_ID_PTR(rb_, val_)	EDD_RQB_SET_USERID_PTR(rb_, val_)
# define CM_EDD_SET_RESPONSE(rb_, val_)		EDD_RQB_SET_RESPONSE(rb_, val_)

# define CM_EDD_GET_NEXT_RQB_PTR(rb_)		EDD_RQB_GET_NEXT_RQB_PTR(rb_)
# define CM_EDD_GET_PREV_RQB_PTR(rb_)		EDD_RQB_GET_PREV_RQB_PTR(rb_)
# define CM_EDD_GET_OPCODE(rb_)				EDD_RQB_GET_OPCODE(rb_)
# define CM_EDD_GET_SERVICE(rb_)			EDD_RQB_GET_SERVICE(rb_)
# define CM_EDD_GET_HANDLE(rb_)				EDD_RQB_GET_HANDLE(rb_)
# define CM_EDD_GET_USER_ID_PTR(rb_)		EDD_RQB_GET_USERID_PTR(rb_)
# define CM_EDD_GET_RESPONSE(rb_)			EDD_RQB_GET_RESPONSE(rb_)

#define CM_EDD_SET_USER_ID_HANDLE_DEV_AR(rb_, h_, dev_, ar_) { /* store channel_handle, device_nr and ar_nr */ \
	LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, dev_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_2(rb_, h_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_3(rb_, (LSA_UINT8)(ar_)); /*see CM_CFG_MAX_CL/SV_ARS*/ \
	}

#define CM_EDD_GET_USER_ID_DEV(rb_) ( /* get device_nr */ \
	LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_) \
	)

#define CM_EDD_GET_USER_ID_HANDLE(rb_) ( /* get channel_handle */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_2(rb_) \
	)

#define CM_EDD_GET_USER_ID_AR(rb_) ( /* get ar_nr */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_3(rb_) \
	)


/*
 * GSY
 */

#if (PSI_CFG_USE_GSY == 1) // included
    #define CM_CFG_USE_GSY   1
#else
    #define CM_CFG_USE_GSY   0
#endif


#if CM_CFG_USE_GSY

#define CM_GSY_INVALID_HANDLE	0xFF /* GSY doesn't export one */

#define CM_GSY_LOWER_RQB_TYPE	GSY_RQB_TYPE

# define CM_GSY_SET_NEXT_RQB_PTR(rb_, val_)	GSY_UPPER_RQB_NEXT_SET(rb_, val_)
# define CM_GSY_SET_PREV_RQB_PTR(rb_, val_)	GSY_UPPER_RQB_PREV_SET(rb_, val_)
# define CM_GSY_SET_OPCODE(rb_, val_)		GSY_UPPER_RQB_OPCODE_SET(rb_, val_)
# define CM_GSY_SET_HANDLE(rb_, val_)		GSY_UPPER_RQB_HANDLE_SET(rb_, val_)
# define CM_GSY_SET_USER_ID_PTR(rb_, val_)	LSA_RQB_SET_USER_ID_PTR(rb_, val_)
# define CM_GSY_SET_RESPONSE(rb_, val_)		GSY_UPPER_RQB_RESPONSE_SET(rb_, val_)

# define CM_GSY_GET_NEXT_RQB_PTR(rb_)		GSY_UPPER_RQB_NEXT_GET(rb_)
# define CM_GSY_GET_PREV_RQB_PTR(rb_)		GSY_UPPER_RQB_PREV_GET(rb_)
# define CM_GSY_GET_OPCODE(rb_)				GSY_UPPER_RQB_OPCODE_GET(rb_)
# define CM_GSY_GET_HANDLE(rb_)				GSY_UPPER_RQB_HANDLE_GET(rb_)
# define CM_GSY_GET_USER_ID_PTR(rb_)		LSA_RQB_GET_USER_ID_PTR(rb_)
# define CM_GSY_GET_RESPONSE(rb_)			GSY_UPPER_RQB_RESPONSE_GET(rb_)

#define CM_GSY_SET_USER_ID_HANDLE_DEV_AR(rb_, h_, dev_, ar_) { /* store channel_handle, device_nr and ar_nr */ \
	LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, dev_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_2(rb_, h_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_3(rb_, (LSA_UINT8)(ar_)); /*see CM_CFG_MAX_CL/SV_ARS*/ \
	}

#define CM_GSY_GET_USER_ID_DEV(rb_) ( /* get device_nr */ \
	LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_) \
	)

#define CM_GSY_GET_USER_ID_HANDLE(rb_) ( /* get channel_handle */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_2(rb_) \
	)

#define CM_GSY_GET_USER_ID_AR(rb_) ( /* get ar_nr */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_3(rb_) \
	)

#endif


/*
 * NARE
 */

#define CM_NARE_INVALID_HANDLE  0xFF /* with very high propability, see nare_int.h */

#define CM_NARE_LOWER_RQB_TYPE	NARE_RQB_TYPE

# define CM_NARE_SET_NEXT_RQB_PTR(rb_, val_)	NARE_RQB_SET_NEXT_RQB_PTR(rb_, val_)
# define CM_NARE_SET_PREV_RQB_PTR(rb_, val_)	NARE_RQB_SET_PREV_RQB_PTR(rb_, val_)
# define CM_NARE_SET_OPCODE(rb_, val_)			NARE_RQB_SET_OPCODE(rb_, val_)
# define CM_NARE_SET_HANDLE(rb_, val_)			NARE_RQB_SET_HANDLE(rb_, val_)
# define CM_NARE_SET_USER_ID_PTR(rb_, val_)		NARE_RQB_SET_USER_ID_PTR(rb_, val_)
# define CM_NARE_SET_RESPONSE(rb_, val_)		NARE_RQB_SET_RESPONSE(rb_, val_)

# define CM_NARE_GET_NEXT_RQB_PTR(rb_)			NARE_RQB_GET_NEXT_RQB_PTR(rb_)
# define CM_NARE_GET_PREV_RQB_PTR(rb_)			NARE_RQB_GET_PREV_RQB_PTR(rb_)
# define CM_NARE_GET_OPCODE(rb_)				NARE_RQB_GET_OPCODE(rb_)
# define CM_NARE_GET_HANDLE(rb_)				NARE_RQB_GET_HANDLE(rb_)
# define CM_NARE_GET_USER_ID_PTR(rb_)			NARE_RQB_GET_USER_ID_PTR(rb_)
# define CM_NARE_GET_RESPONSE(rb_)				NARE_RQB_GET_RESPONSE(rb_)

#define CM_NARE_SET_USER_ID_HANDLE_DEV_AR(rb_, h_, dev_, ar_) { /* store channel_handle, device_nr and ar_nr */ \
	LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, dev_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_2(rb_, h_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_3(rb_, (LSA_UINT8)(ar_)); /*see CM_CFG_MAX_CL/SV_ARS*/ \
	}

#define CM_NARE_GET_USER_ID_DEV(rb_) ( /* get device_nr */ \
	LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_) \
	)

#define CM_NARE_GET_USER_ID_HANDLE(rb_) ( /* get channel_handle */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_2(rb_) \
	)

#define CM_NARE_GET_USER_ID_AR(rb_) ( /* get ar_nr */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_3(rb_) \
	)



/*
 * OHA
 */

#define CM_OHA_INVALID_HANDLE	OHA_INVALID_HANDLE

#define CM_OHA_LOWER_RQB_TYPE	OHA_RQB_TYPE

# define CM_OHA_SET_NEXT_RQB_PTR(rb_, val_)	OHA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
# define CM_OHA_SET_PREV_RQB_PTR(rb_, val_)	OHA_RQB_SET_PREV_RQB_PTR(rb_, val_)
# define CM_OHA_SET_OPCODE(rb_, val_)		OHA_RQB_SET_OPCODE(rb_, val_)
# define CM_OHA_SET_HANDLE(rb_, val_)		OHA_RQB_SET_HANDLE(rb_, val_)
# define CM_OHA_SET_USER_ID_PTR(rb_, val_)	OHA_RQB_SET_USER_ID_PTR(rb_, val_)
# define CM_OHA_SET_RESPONSE(rb_, val_)		OHA_RQB_SET_RESPONSE(rb_, val_)

# define CM_OHA_GET_NEXT_RQB_PTR(rb_)		OHA_RQB_GET_NEXT_RQB_PTR(rb_)
# define CM_OHA_GET_PREV_RQB_PTR(rb_)		OHA_RQB_GET_PREV_RQB_PTR(rb_)
# define CM_OHA_GET_OPCODE(rb_)				OHA_RQB_GET_OPCODE(rb_)
# define CM_OHA_GET_HANDLE(rb_)				OHA_RQB_GET_HANDLE(rb_)
# define CM_OHA_GET_USER_ID_PTR(rb_)		OHA_RQB_GET_USER_ID_PTR(rb_)
# define CM_OHA_GET_RESPONSE(rb_)			OHA_RQB_GET_RESPONSE(rb_)

#define CM_OHA_SET_USER_ID_HANDLE_DEV_AR(rb_, h_, dev_, ar_) { /* store channel_handle, device_nr and ar_nr */ \
	LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, dev_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_2(rb_, h_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_3(rb_, (LSA_UINT8)(ar_)); /*see CM_CFG_MAX_CL/SV_ARS*/ \
	}

#define CM_OHA_GET_USER_ID_DEV(rb_) ( /* get device_nr */ \
	LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_) \
	)

#define CM_OHA_GET_USER_ID_HANDLE(rb_) ( /* get channel_handle */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_2(rb_) \
	)

#define CM_OHA_GET_USER_ID_AR(rb_) ( /* get ar_nr */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_3(rb_) \
	)


/*
 * RPC
 */

#define CM_RPC_INVALID_HANDLE		CLRPC_INVALID_HANDLE
#define CM_RPC_INVALID_CLIENT_ID	CLRPC_INVALID_CLIENT_ID

#define CM_RPC_LOWER_RQB_TYPE		CLRPC_RQB_TYPE

#define CM_RPC_SET_NEXT_RQB_PTR(rb_, val_)	CLRPC_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define CM_RPC_SET_PREV_RQB_PTR(rb_, val_)	CLRPC_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define CM_RPC_SET_OPCODE(rb_, val_)		CLRPC_RQB_SET_OPCODE(rb_, val_)
#define CM_RPC_SET_HANDLE(rb_, val_)		CLRPC_RQB_SET_HANDLE(rb_, val_)

#define CM_RPC_GET_NEXT_RQB_PTR(rb_)		CLRPC_RQB_GET_NEXT_RQB_PTR(rb_)
#define CM_RPC_GET_PREV_RQB_PTR(rb_)		CLRPC_RQB_GET_PREV_RQB_PTR(rb_)
#define CM_RPC_GET_OPCODE(rb_)				CLRPC_RQB_GET_OPCODE(rb_)
#define CM_RPC_GET_HANDLE(rb_)				CLRPC_RQB_GET_HANDLE(rb_)

#define CM_RPC_SET_USER_ID_PTR(rb_, val_)	CLRPC_RQB_SET_USER_ID_PTR(rb_, val_)
#define CM_RPC_GET_USER_ID_PTR(rb_)			CLRPC_RQB_GET_USER_ID_PTR(rb_)

#define CM_RPC_SET_RESPONSE(rb_, val_)		CLRPC_RQB_SET_RESPONSE(rb_, val_)
#define CM_RPC_GET_RESPONSE(rb_)			CLRPC_RQB_GET_RESPONSE(rb_)

#define CM_RPC_SET_ARGS(rb_, val_)			CLRPC_RQB_SET_ARGS(rb_, val_)
#define CM_RPC_GET_ARGS(rb_)				CLRPC_RQB_GET_ARGS(rb_)

#define CM_RPC_SET_USER_ID_HANDLE_DEV_AR(rb_, h_, dev_, ar_) { /* store channel_handle, device_nr and ar_nr */ \
	LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, dev_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_2(rb_, h_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_3(rb_, (LSA_UINT8)(ar_)); /*see CM_CFG_MAX_CL/SV_ARS*/ \
	}

#define CM_RPC_GET_USER_ID_DEV(rb_) ( /* get device_nr */ \
	LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_) \
	)

#define CM_RPC_GET_USER_ID_HANDLE(rb_) ( /* get channel_handle */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_2(rb_) \
	)

#define CM_RPC_GET_USER_ID_AR(rb_) ( /* get ar_nr */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_3(rb_) \
	)


/*
 * MRP
 */

#if (PSI_CFG_USE_MRP == 1) // included
    #define CM_CFG_USE_MRP   1
#else
    #define CM_CFG_USE_MRP   0
#endif


#if CM_CFG_USE_MRP

#define CM_MRP_INVALID_HANDLE	0xFF /* with very high propability, see mrp_int.h */

#define CM_MRP_LOWER_RQB_TYPE	MRP_RQB_TYPE

# define CM_MRP_SET_NEXT_RQB_PTR(rb_, val_)		MRP_RQB_SET_NEXT_RQB_PTR(rb_, val_)
# define CM_MRP_SET_PREV_RQB_PTR(rb_, val_)		MRP_RQB_SET_PREV_RQB_PTR(rb_, val_)
# define CM_MRP_SET_OPCODE(rb_, val_)			MRP_RQB_SET_OPCODE(rb_, val_)
# define CM_MRP_SET_HANDLE(rb_, val_)			MRP_RQB_SET_HANDLE(rb_, val_)
# define CM_MRP_SET_USER_ID_PTR(rb_, val_)		MRP_RQB_SET_USER_ID_PTR(rb_, val_)
# define CM_MRP_SET_RESPONSE(rb_, val_)			MRP_RQB_SET_RESPONSE(rb_, val_)

# define CM_MRP_GET_NEXT_RQB_PTR(rb_)			MRP_RQB_GET_NEXT_RQB_PTR(rb_)
# define CM_MRP_GET_PREV_RQB_PTR(rb_)			MRP_RQB_GET_PREV_RQB_PTR(rb_)
# define CM_MRP_GET_OPCODE(rb_)					MRP_RQB_GET_OPCODE(rb_)
# define CM_MRP_GET_HANDLE(rb_)					MRP_RQB_GET_HANDLE(rb_)
# define CM_MRP_GET_USER_ID_PTR(rb_)			MRP_RQB_GET_USER_ID_PTR(rb_)
# define CM_MRP_GET_RESPONSE(rb_)				MRP_RQB_GET_RESPONSE(rb_)

#define CM_MRP_SET_USER_ID_HANDLE_DEV_AR(rb_, h_, dev_, ar_) { /* store channel_handle, device_nr and ar_nr */ \
	LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, dev_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_2(rb_, h_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_3(rb_, (LSA_UINT8)(ar_)); /*see CM_CFG_MAX_CL/SV_ARS*/ \
	}

#define CM_MRP_GET_USER_ID_DEV(rb_) ( /* get device_nr */ \
	LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_) \
	)

#define CM_MRP_GET_USER_ID_HANDLE(rb_) ( /* get channel_handle */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_2(rb_) \
	)

#define CM_MRP_GET_USER_ID_AR(rb_) ( /* get ar_nr */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_3(rb_) \
	)

#endif


/*
 * POF
 */

#if (PSI_CFG_USE_POF == 1) // included
    #define CM_CFG_USE_POF   1
#else
    #define CM_CFG_USE_POF   0
#endif


#if CM_CFG_USE_POF

#define CM_POF_INVALID_HANDLE	0xFF /* with very high propability, see POF_GetUserHDBFromHandle() */

#define CM_POF_LOWER_RQB_TYPE	POF_RQB_TYPE

# define CM_POF_SET_NEXT_RQB_PTR(rb_, val_)		POF_RQB_SET_NEXT_RQB_PTR(rb_, val_)
# define CM_POF_SET_PREV_RQB_PTR(rb_, val_)		POF_RQB_SET_PREV_RQB_PTR(rb_, val_)
# define CM_POF_SET_OPCODE(rb_, val_)			POF_RQB_SET_OPCODE(rb_, val_)
# define CM_POF_SET_HANDLE(rb_, val_)			POF_RQB_SET_HANDLE(rb_, val_)
# define CM_POF_SET_USER_ID_PTR(rb_, val_)		POF_RQB_SET_USER_ID_PTR(rb_, val_)
# define CM_POF_SET_RESPONSE(rb_, val_)			POF_RQB_SET_RESPONSE(rb_, val_)

# define CM_POF_GET_NEXT_RQB_PTR(rb_)			POF_RQB_GET_NEXT_RQB_PTR(rb_)
# define CM_POF_GET_PREV_RQB_PTR(rb_)			POF_RQB_GET_PREV_RQB_PTR(rb_)
# define CM_POF_GET_OPCODE(rb_)					POF_RQB_GET_OPCODE(rb_)
# define CM_POF_GET_HANDLE(rb_)					POF_RQB_GET_HANDLE(rb_)
# define CM_POF_GET_USER_ID_PTR(rb_)			POF_RQB_GET_USER_ID_PTR(rb_)
# define CM_POF_GET_RESPONSE(rb_)				POF_RQB_GET_RESPONSE(rb_)

#define CM_POF_SET_USER_ID_HANDLE_DEV_AR(rb_, h_, dev_, ar_) { /* store channel_handle, device_nr and ar_nr */ \
	LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(rb_, dev_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_2(rb_, h_); \
	LSA_RQB_SET_USER_ID_UVAR8_ARRAY_3(rb_, (LSA_UINT8)(ar_)); /*see CM_CFG_MAX_CL/SV_ARS*/ \
	}

#define CM_POF_GET_USER_ID_DEV(rb_) ( /* get device_nr */ \
	LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(rb_) \
	)

#define CM_POF_GET_USER_ID_HANDLE(rb_) ( /* get channel_handle */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_2(rb_) \
	)

#define CM_POF_GET_USER_ID_AR(rb_) ( /* get ar_nr */ \
	LSA_RQB_GET_USER_ID_UVAR8_ARRAY_3(rb_) \
	)

#endif


/*=============================================================================
 *       set local_mem static or dynamic:
 * static (no use of CM_ALLOC_LOCAL_MEM):
 * #define CM_LOCAL_MEM_STATIC
 * or dynamic (use of CM_ALLOC_LOCAL_MEM):
 * #define CM_LOCAL_MEM_DYNAMIC
 *===========================================================================*/
#define CM_LOCAL_MEM_DYNAMIC /* don't change. there is no choice! */


/*=============================================================================
 *       clear global data:
 * clear (CM_DATA_TYPE cm_data = {0};):
 * #define CM_CLEAR_GLOBAL_DATA
 * or no clear:
 * #define CM_NO_CLEAR_GLOBAL_DATA
 *===========================================================================*/
#define CM_NO_CLEAR_GLOBAL_DATA /* don't change. there is no choice! */


/*===========================================================================*/
/* configurable things                                                       */
/*===========================================================================*/

#define CM_CFG_USE_CASTS    1   /* 1 = use casts to avoid some warnings */

/*---------------------------------------------------------------------------*/

#define CM_CFG_MAX_CLIENTS              (PSI_CFG_USE_IOC ? PSI_CFG_MAX_IF_CNT : 0)

#if CM_CFG_MAX_CLIENTS

#define CM_CFG_MAX_CL_LOG_ENTRIES       12 // current session + IN_DATA/OFFLINE for old session 
                                           // (see cm_cl_logbook_lbentry_type_enum (per client-device)

#define CM_CFG_MAX_CL_FSU_DEVICES (PSI_CFG_CM_CFG_MAX_CL_FSU_DEVICES)
#ifndef CM_CFG_MAX_CL_FSU_DEVICES
# error "CM_CFG_MAX_CL_FSU_DEVICES not defined"  /* 0 = no Fast-StartUp-devices, 1..EDD_DCP_MAX_DCP_HELLO_FILTER = max. number of supported Fast-StartUp-devices */
#endif

#endif

/*---------------------------------------------------------------------------*/

#define CM_CFG_MAX_SERVERS          (PSI_CFG_USE_IOD ? 1 : 0)

#if CM_CFG_MAX_SERVERS

#define CM_CFG_MAX_SV_DEVICES   (PSI_CFG_MAX_SV_DEVICES)   /* 1 .. 2048 per server */

#define CM_CFG_MAX_SV_LOG_ENTRIES   16 // per device (min=16, see IEC61158-6) 

#define CM_CFG_MAX_SV_ARS       PSI_CFG_CM_CFG_MAX_SV_ARS       /* 2 .. 16 ARs per device */

#define CM_CFG_SV_AR_HAS_MCONS      0

#endif

/*---------------------------------------------------------------------------*/

#define CM_CFG_MAX_MULTICAST_CHANNELS   (PSI_CFG_USE_IOM ? PSI_CFG_MAX_IF_CNT : 0)


/*------------------------------------------------------------------------------
//	Assert
//
//  ==> MUST BE EMPTY in Release-Version!
//
//  check if condition holds or call fatal error otherwise (programming error)
//
//----------------------------------------------------------------------------*/

#if CM_DEBUG

#ifdef _lint
#define CM_TEST_POINTER(ptr_)  (ptr_) /* empty for Lint */
#else
#define CM_TEST_POINTER(ptr_)  (PSI_TEST_POINTER(ptr_)) /* test for strange values */
#endif
// CM Fatal is defined in cm_sys.c/h. cm_archb_helper.c requires this definition if no cm is available.
#if (PSI_CFG_USE_HD_COMP == 0)
#define CM_FATAL()  
#define CM_FATAL1(ec_0) 
#endif

	/* check if condition holds or call fatal (programming error) */
#define CM_ASSERT(cond_) { if (!(cond_)) { CM_FATAL(); } }

#else

#define CM_TEST_POINTER(ptr_)  (ptr_) /* don't test */

#define CM_ASSERT(cond_) { /* empty-block in release version */ }

#endif

/*------------------------------------------------------------------------------
//	System Layer (prototypes in cm_sys.h)
//----------------------------------------------------------------------------*/

/*
 * all "macros" are implemented as functions in cm_cfg.c as an example
 */

/*------------------------------------------------------------------------------
//	Upper Layer (prototypes in cm_usr.h)
//----------------------------------------------------------------------------*/

/*
 * all "macros" are implemented as functions in cm_cfg.c as an example
 */

/*------------------------------------------------------------------------------
//	Lower Layer (prototypes in cm_low.h)
//----------------------------------------------------------------------------*/

/*
 * all "macros" are implemented as functions in cm_cfg.c as an example
 */

/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 = no LSA trace
//	1 = LSA trace
//	2 = LSA index trace
//----------------------------------------------------------------------------*/

#define CM_CFG_TRACE_MODE       (PSI_CFG_TRACE_MODE)


/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef CM_FILE_SYSTEM_EXTENSION
# define CM_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif

/*- alloc mem macros----------------------------------------------------------*/

#define CM_ALLOC_UPPER_RQB(upper_rqb_ptr_ptr, user_id, length, sys_ptr)     PSI_ALLOC_LOCAL_MEM((upper_rqb_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_CM, PSI_MTYPE_UPPER_RQB)
#define CM_ALLOC_UPPER_MEM(upper_mem_ptr_ptr, user_id, length, sys_ptr)     PSI_ALLOC_LOCAL_MEM((upper_mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_CM, PSI_MTYPE_UPPER_MEM)
#define CM_ACP_ALLOC_ALARM_MEM(mem_ptr_ptr, length, sys_ptr)                PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), (sys_ptr), LSA_COMP_ID_CM, PSI_MTYPE_LOWER_MEM_ACP)

#define CM_FREE_UPPER_RQB(ret_val_ptr, upper_rqb_ptr, sys_ptr)              PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), (sys_ptr), LSA_COMP_ID_CM, PSI_MTYPE_UPPER_RQB)
#define CM_FREE_UPPER_MEM(ret_val_ptr, upper_mem_ptr, sys_ptr)              PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_CM, PSI_MTYPE_UPPER_MEM)
#define CM_ACP_FREE_ALARM_MEM(ret_val_ptr, mem_ptr, sys_ptr)                PSI_FREE_LOCAL_MEM((ret_val_ptr), (mem_ptr), 0, LSA_COMP_ID_CM, PSI_MTYPE_LOWER_MEM_ACP)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_CFG_H */
