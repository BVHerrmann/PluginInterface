#ifndef PCIOX_PSI_CFG_PLF_WINSOCK_H     /* ----- reinclude-protection ----- */
#define PCIOX_PSI_CFG_PLF_WINSOCK_H

#ifdef __cplusplus                      /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: pcIOX (PNIO Controler, Device and more)   :C&  */
/*                                                                           */
/*  F i l e               &F: pciox_psi_cfg_plf_winsock.h               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI EPS Platform defintions for EDDS with WPCAP and WINSOCK.             */
/*                                                                           */
/*****************************************************************************/

/* included by "pnboards_psi_cfg_plf_eps.h" */
#define PCIOX_PLATFORM_NAME "WINSOCK"

#define PSI_CFG_MAX_IF_CNT          4 // Maximum number of PNIO interfaces (HDs) of all EDDs integrated in a system: 1x PNIO interface = 1x HD = 1x EDD
#define PSI_CFG_MAX_PORT_CNT        4 // Maximum number of ports supported by EDD API services

#define PSI_CFG_MAX_SOCK_APP_CHANNELS  2 /* Maximum number of user application sock channels, Values (1..8) */
#define PSI_CFG_MAX_DIAG_TOOLS         8 /* Maximum number of diagnosis tools used in the system */

// Compile Keys for pcIOX (CM/SNMPX role specific) features
#define PSI_CFG_USE_IOC                 1
#define PSI_CFG_USE_IOM                 1
#define PSI_CFG_USE_IOD                 1
#define PSI_CFG_MAX_SNMPX_MGR_SESSIONS  32

// Compile Keys pcIOX (PNIO component role and specific)
// Note: 0 is off, 1 used

/* include optional components for HD */
#define PSI_CFG_USE_EDDI       0
#define PSI_CFG_USE_EDDP       0
#define PSI_CFG_USE_EDDS       1
#define PSI_CFG_USE_EDDT       0
#define PSI_CFG_USE_POF        0

/* include optional components for LD */
#define PSI_CFG_USE_DNS        0
#define PSI_CFG_USE_TCIP       0 

#define PSI_CFG_TCIP_STACK_OPEN_BSD      0
#define PSI_CFG_TCIP_STACK_INTERNICHE    0
#define PSI_CFG_TCIP_STACK_WINSOCK       1

#if PSI_CFG_USE_IOC
    #define PSI_CFG_MAX_CL_DEVICES       128         // Max devices for create client
    #define PSI_CFG_MAX_CL_OSU_DEVICES   16          // range 1..32 (see EDD_DCP_MAX_DCP_HELLO_FILTER(=32))
#endif

#if PSI_CFG_USE_IOM
    #define PSI_CFG_MAX_MC_DEVICES       32             // Max devices for create MC client
#endif

#if PSI_CFG_USE_IOD
    #define PSI_CFG_MAX_SV_DEVICES        64            // max devices for create SV server
    #define PSI_CFG_MAX_SV_IO_ARS         16            // maximum number of ARs for cyclic communication (RTC123 IO CRs)
#endif

// Select the EDDS configuration
#if (PSI_CFG_USE_EDDS == 1)
#define PSI_EDDS_CFG_HW_PACKET32
#define PSI_CFG_MAX_EDDS_DEVICES    4 // Maximum number of devices that the EDDS supports
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PCIOX_PSI_CFG_PLF_WINSOCK_H */
