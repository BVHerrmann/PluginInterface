#ifndef TCIP_OBSD_CFG_H                       /* ----- reinclude-protection ----- */
#define TCIP_OBSD_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_obsd_cfg.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration module                                                     */
/*                                                                           */
/*****************************************************************************/
#include "psi_cfg.h"

/*===========================================================================*/
/*=== Global defines from PNIO to OBSD           ============================*/
/*===========================================================================*/

#if defined (LSA_HOST_ENDIANESS_LITTLE)
#define OBSD_PNIO_CFG_BIG_ENDIAN 0
#else
#define OBSD_PNIO_CFG_BIG_ENDIAN 1
#endif

/*  #define OBSD_PNIO_TOOL_CHAIN_MICROSOFT   MS-C/C++ Compiler                        */
/*  #define OBSD_PNIO_TOOL_CHAIN_TASKING_TRICORE                                       */
/*  #define OBSD_PNIO_TOOL_CHAIN_GNU                                                  */
/*  #define OBSD_PNIO_TOOL_CHAIN_GNU_PPC                                               */
/*  #define OBSD_PNIO_TOOL_CHAIN_CC386                                                 */
/*  #define OBSD_PNIO_TOOL_CHAIN_GREENHILLS_ARM                                        */
/*  #define OBSD_PNIO_TOOL_CHAIN_NRK  ... ARM/Thumb C/C++ Compiler                     */

#if defined TOOL_CHAIN_MICROSOFT
#define OBSD_PNIO_TOOL_CHAIN_MICROSOFT
#elif defined TOOL_CHAIN_TASKING_TRICORE
#define OBSD_PNIO_TOOL_CHAIN_TASKING_TRICORE
#elif defined TOOL_CHAIN_GNU
#define OBSD_PNIO_TOOL_CHAIN_GNU
#elif defined TOOL_CHAIN_GNU_PPC
#define OBSD_PNIO_TOOL_CHAIN_GNU_PPC
#elif defined TOOL_CHAIN_CC386
#define OBSD_PNIO_TOOL_CHAIN_CC386
#elif defined TOOL_CHAIN_GREENHILLS_ARM
#define OBSD_PNIO_TOOL_CHAIN_GREENHILLS_ARM
#elif defined TOOL_CHAIN_NRK
#define OBSD_PNIO_TOOL_CHAIN_NRK
#else
#error "no valid toolchain defined"
#endif

// Note: Not possible to use EDD_CFG_MAX_INTERFACE_CNT and EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE here since #include "edd_cfg.h" is not possible in this file. Use the PSI defines.
#define OBSD_PNIO_CFG_MAX_IF_COUNT ((2 * PSI_CFG_EDD_CFG_MAX_INTERFACE_CNT) + PSI_CFG_EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE) /* add Carp Interfaces */
#define OBSD_PNIO_CFG_MAX_SOCKETS  (PSI_CFG_MAX_SOCKETS)
#define OBSD_PNIO_CFG_SYS_SERVICES (PSI_CFG_TCIP_CFG_SYS_SERVICES)
#define OBSD_PNIO_CFG_BUFFERS_LIMIT 1000
#define OBSD_PNIO_CFG_BUFFERS_IDLE  250
#define OBSD_PNIO_CFG_SYN_CACHE_LIMIT (PSI_CFG_MAX_SOCKETS * 2)

/* RQ 2223846 */
/* Default GW + Routing Entries for Interfaces + ARP Cache + ARP Requests */
#define OBSD_PNIO_CFG_RT_ENTRY_LIMIT 2000 /* 1 entry ~ 150 Bytes --> 300 kBytes */

#define OBSD_PNIO_CFG_TRACE_MODE	(PSI_CFG_TRACE_MODE)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_CFG_H (reinclude protection) */
