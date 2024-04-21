#ifndef ACP_PLS_H
#define ACP_PLS_H

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
/*  C o m p o n e n t     &C: ACP (Alarm Consumer Provider)             :C&  */
/*                                                                           */
/*  F i l e               &F: acp_pls.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  check consistency with EDD                                               */
/*                                                                           */
/*  check the configuration                                                  */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#ifndef ACP_DEBUG
#error "ACP_DEBUG not defined in acp_cfg.h"
#endif

#ifndef ACP_CFG_MAX_CHANNELS
#error "ACP_CFG_MAX_CHANNELS not defined in acp_cfg.h"
#endif

#ifndef ACP_CFG_CAST_WARN
#error "ACP_CFG_CAST_WARN not defined in acp_cfg.h"
#endif

#ifndef ACP_ASSERT
#error "ACP_ASSERT not defined in acp_cfg.h"
#endif

#ifndef ACP_FILE_SYSTEM_EXTENSION
#error "ACP_FILE_SYSTEM_EXTENSION not defined in acp_cfg.h"
#endif

#ifndef ACP_CFG_TRACE_MODE
# error "ACP_CFG_TRACE_MODE not defined in file acp_cfg.h"
#endif

#ifndef ACP_FRAG_MODE /* Note: experimental */
#error "ACP_FRAG_MODE not defined in acp_usr.h"
#endif

/*----------------------------------------------------------------------------*/

#ifdef ACP_CFG_RT_CLASS_3_SUPPORT_ON
#error "ACP_CFG_RT_CLASS_3_SUPPORT_ON is obsolete (AP01498199). please remove the define from acp_cfg.h"
#endif

#ifdef ACP_ASSERT_OR_FATAL
#error "ACP_ASSERT_OR_FATAL is no longer necessary, remove if from acp_cfg.h"
#endif

/*----------------------------------------------------------------------------*/

#if 1 <= ACP_INVALID_HANDLE  && ACP_INVALID_HANDLE <= ACP_CFG_MAX_CHANNELS
#error "ACP_CFG_MAX_CHANNELS wrong"
#endif

/*----------------------------------------------------------------------------*/
/* LSA */

#if defined ACP_UPPER_IN_FCT_ATTR
#error "ACP_UPPER_IN_FCT_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_UPPER_OUT_FCT_ATTR
#error "ACP_UPPER_OUT_FCT_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_LOWER_IN_FCT_ATTR
#error "ACP_LOWER_IN_FCT_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_LOWER_OUT_FCT_ATTR
#error "ACP_LOWER_OUT_FCT_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_SYSTEM_IN_FCT_ATTR
#error "ACP_SYSTEM_IN_FCT_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_SYSTEM_OUT_FCT_ATTR
#error "ACP_SYSTEM_OUT_FCT_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_LOCAL_FCT_ATTR
#error "ACP_LOCAL_FCT_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_UPPER_RQB_ATTR
#error "ACP_UPPER_RQB_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_UPPER_MEM_ATTR
#error "ACP_UPPER_MEM_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_LOWER_RQB_ATTR
#error "ACP_LOWER_RQB_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_LOWER_MEM_ATTR
#error "ACP_LOWER_MEM_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_SYSTEM_MEM_ATTR
#error "ACP_SYSTEM_MEM_ATTR defined in file acp_cfg.h"
#endif

#if defined ACP_LOCAL_MEM_ATTR
#error "ACP_LOCAL_MEM_ATTR defined in file acp_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif


/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of ACP_PLS_H */
