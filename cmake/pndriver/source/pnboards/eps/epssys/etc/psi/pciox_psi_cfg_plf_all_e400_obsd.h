#ifndef PCIOX_PSI_CFG_PLF_ALL_E400_OBSD_H   /* ----- reinclude-protection ----- */
#define PCIOX_PSI_CFG_PLF_ALL_E400_OBSD_H

#ifdef __cplusplus                          /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: pciox_psi_cfg_plf_all_e400_obsd.h         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */ 
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI EPS Platform defintions for EDDI ERTEC400, EDDP, EDDS and EDDT       */
/*  using OBSD.                                                              */
/*                                                                           */
/*****************************************************************************/

//Use PnDevDrv
#define EPS_CFG_USE_PNDEVDRV

//Use VDD
#define EPS_CFG_USE_VDD

/* included by "pnboards_psi_cfg_plf_eps.h" */
#define PCIOX_PLATFORM_NAME "ERTEC400-ALL-OBSD"
	
#define PSI_CFG_MAX_IF_CNT          4 // Maximum number of PNIO interfaces (HDs) of all EDDs integrated in a system: 1x PNIO interface = 1x HD = 1x EDD
#define PSI_CFG_MAX_PORT_CNT        4 // Maximum number of ports supported by EDD API services

#define PSI_CFG_MAX_SOCK_APP_CHANNELS  2 /* Maximum number of user application sock channels, Values (1..8) */
#define PSI_CFG_MAX_DIAG_TOOLS         8 /* Maximum number of diagnosis tools used in the system */

// Compile Keys pcIOX (PNIO component role and specific)
#define PSI_CFG_USE_IOC                 1
#define PSI_CFG_USE_IOM                 1
#define PSI_CFG_USE_IOD                 1
#define PSI_CFG_MAX_SNMPX_MGR_SESSIONS  32

// Compile Keys pcIOX (PNIO component role and specific)
// Note: 0 is off, 1 used
/* include optional components for HD */
#define PSI_CFG_USE_EDDI       1
#define PSI_CFG_USE_EDDP       1
#define PSI_CFG_USE_EDDS       1
#define PSI_CFG_USE_EDDT       0
#define PSI_CFG_USE_POF        1

/* include optional components for LD */
#define PSI_CFG_USE_DNS        1
#define PSI_CFG_USE_TCIP       1

#define PSI_CFG_TCIP_STACK_OPEN_BSD       1
#define PSI_CFG_TCIP_STACK_INTERNICHE     0
#define PSI_CFG_TCIP_STACK_WINSOCK        0

#if PSI_CFG_USE_IOC
    #define PSI_CFG_MAX_CL_DEVICES       256         // Max devices for create client
    #define PSI_CFG_MAX_CL_OSU_DEVICES   16          // range 1..32 (see EDD_DCP_MAX_DCP_HELLO_FILTER(=32))
#endif

#if PSI_CFG_USE_IOM
    #define PSI_CFG_MAX_MC_DEVICES       32        // Max devices for create MC client
#endif

#if PSI_CFG_USE_IOD
    #define PSI_CFG_MAX_SV_DEVICES       64          // max devices for create SV server
    #define PSI_CFG_MAX_SV_IO_ARS        16          // Maximum number of ARs for cyclic communication (RTC123 IO CRs)
#endif

// Select the EDDI configuration
#if (PSI_CFG_USE_EDDI == 1)
    #define PSI_CFG_EDDI_CFG_ERTEC_400
    #define PSI_CFG_MAX_EDDI_DEVICES    4 // Maximum number of devices that the EDDI supports
#endif

// Select the EDDP configuration
#if (PSI_CFG_USE_EDDP == 1)
    #define PSI_CFG_MAX_EDDP_DEVICES        4 // Maximum number of devices that the EDDP supports
    // Light variant supports both ASICs
    #define PSI_CFG_EDDP_CFG_HW_ERTEC200P_SUPPORT
    #define PSI_CFG_EDDP_CFG_HW_HERA_SUPPORT
	#define PSI_CFG_EDDP_CFG_ISR_POLLING_MODE      // interrupt context in UserMode   
#endif

// Select the EDDS configuration
#if (PSI_CFG_USE_EDDS == 1)
    #define PSI_EDDS_CFG_HW_INTEL
    #define PSI_EDDS_CFG_HW_KSZ88XX
    #define PSI_EDDS_CFG_HW_TI
    #define PSI_CFG_MAX_EDDS_DEVICES    4 // Maximum number of devices that the EDDS supports
#endif

// Select the EDDT configuration
#if (PSI_CFG_USE_EDDT == 1)
    #define PSI_CFG_MAX_EDDT_DEVICES    4 // Maximum number of devices that the EDDT supports
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PCIOX_PSI_CFG_PLF_ALL_E400_OBSD_H */
