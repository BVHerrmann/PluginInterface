#ifndef PNBOARDS_PSI_CFG_PLF_BASIC_SOC1_INTERNICHE_H    /* ----- reinclude-protection ----- */
#define PNBOARDS_PSI_CFG_PLF_BASIC_SOC1_INTERNICHE_H

#ifdef __cplusplus                                      /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: pnboards_psi_cfg_plf_basic_soc1_interniche.h :F& */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI EPS settings for SOC1 board and TCIP-Stack is interniche.            */
/*                                                                           */
/*****************************************************************************/

/* included by <pnboards_psi_cfg_plf_eps.h> */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

// check the including of this header only for SOC1 board
#if (EPS_PLF != EPS_PLF_SOC_MIPS)
#error "(EPS_PLF != EPS_PLF_SOC_MIPS) in <pnboards_psi_cfg_plf_basic_soc1_interniche.h>"
#endif

#define EPS_PLATFORM_NAME "SOC"
	
/* Maximum number of PNIO interfaces (HDs) of all EDDs integrated in a system: 1x PNIO interface = 1x HD = 1x EDD */
#define PSI_CFG_MAX_IF_CNT      4

// Compile Keys for EPS (CM/SNMPX role specific) features
#define PSI_CFG_USE_IOC         1
#define PSI_CFG_USE_IOM         1
#define PSI_CFG_USE_IOD         1

#define PSI_CFG_MAX_SNMPX_MGR_SESSIONS  32

/* include eddx */
#define PSI_CFG_USE_EDDI        1
#define PSI_CFG_USE_EDDP        0
#define PSI_CFG_USE_EDDS        0
#define PSI_CFG_USE_EDDT        0

/* include optional components for LD */      
#define PSI_CFG_USE_DNS         0

/* Select IP stack  */
#define PSI_CFG_TCIP_STACK_OPEN_BSD   0
#define PSI_CFG_TCIP_STACK_INTERNICHE 1
    
/* Maximum number of devices that the EDDI supports */
#define PSI_CFG_MAX_EDDI_DEVICES    1

#define PSI_CFG_USE_NRT_CACHE_SYNC  1

// This EPS variant calls psi_hd_interrupt and psi_hd_eddi_exttimer_interrupt in kernel mode context. The switch must be set.
// Also see eps_internal_cfg -> EPS_ISR_MODE_SOC1 has to be set to EPS_ISR_MODE_IR_KERNELMODE, otherwise undef this.
#define PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK

#if PSI_CFG_USE_IOC
    #define PSI_CFG_MAX_CL_DEVICES      512         // Max devices for create client 
    #define PSI_CFG_MAX_CL_OSU_DEVICES  16          // range 1..32 (see EDD_DCP_MAX_DCP_HELLO_FILTER(=32))
#endif

#if PSI_CFG_USE_IOM
    #define PSI_CFG_MAX_MC_DEVICES      32  // Max devices for create MC client
#endif

#if PSI_CFG_USE_IOD
    #define PSI_CFG_MAX_SV_DEVICES      32  // maximum numbers of IOD instances, e.g. 64 for IEPB link, 1 for other UseCases
    #define PSI_CFG_MAX_SV_IO_ARS       16  // maximum number of ARs for cyclic communication (RTC123 IO CRs)
#endif

// Select the EDDI configuration
#define PSI_CFG_EDDI_CFG_SOC

// EDDI configuration for Linux basic variant with iso interrupt in Linux x86 host
//#define PSI_CFG_EDDI_CFG_APPLSYNC_SEPARATE

// used in both variants HD ONLY and LD+DH (like CPU 1518 and 1516)
#define PSI_CFG_EDDI_CFG_SII_USE_SPECIAL_EOI    4999    //100 µs on IRTE Rev 7
    
// system config - EPS_HD_ONLY is set by -DEPS_HD_ONLY in makefile / ant.
#ifdef EPS_HD_ONLY      /* HD only firmware. */
    #define PSI_CFG_USE_LD_COMP      0
    #define PSI_CFG_USE_HD_COMP      1
    
    /* HIF HD is required       */
    #define PSI_CFG_USE_HIF_LD       0
    #define PSI_CFG_USE_HIF_HD       1
#else                   /* LD and HD firmware.*/
    #define PSI_CFG_USE_LD_COMP      1
    #define PSI_CFG_USE_HD_COMP      1

    /* Only HIF LD is required. HIF HD is disabled to enhance performance  */
    #define PSI_CFG_USE_HIF_LD       1
    #define PSI_CFG_USE_HIF_HD       0
#endif

#define PSI_CFG_HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PNBOARDS_PSI_CFG_PLF_BASIC_SOC1_INTERNICHE_H */
