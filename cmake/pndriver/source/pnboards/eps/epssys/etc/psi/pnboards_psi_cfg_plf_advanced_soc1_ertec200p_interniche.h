#ifndef PNBOARDS_PSI_CFG_PLF_ADVANCED_SOC1_ERTEC200P_INTERNICHE_H   /* ----- reinclude-protection ----- */
#define PNBOARDS_PSI_CFG_PLF_ADVANCED_SOC1_ERTEC200P_INTERNICHE_H

#ifdef __cplusplus                                                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: pnboards_psi_cfg_plf_advanced_soc1_ertec200p_interniche.h:F& */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI EPS settings for Adonis on IntelX86 and TCIP-Stack is interniche.    */
/*                                                                           */
/*****************************************************************************/

/* included by <pnboards_psi_cfg_plf_eps.h> */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

//Use PnDevDrv
#define EPS_CFG_USE_PNDEVDRV

// check the including of this header only for Adonis on IntelX86
#if (EPS_PLF != EPS_PLF_ADONIS_X86)
#error "(EPS_PLF != EPS_PLF_ADONIS_X86) in <pnboards_psi_cfg_plf_advanced_soc1_ertec200p_interniche.h>"
#endif

#define EPS_PLATFORM_NAME "ADONIS_X86"
	
/* Maximum number of PNIO interfaces (HDs) of all EDDs integrated in a system: 1x PNIO interface = 1x HD = 1x EDD */
#define PSI_CFG_MAX_IF_CNT      4
	
// Compile Keys for EPS (CM/SNMPX role specific) features
#define PSI_CFG_USE_IOC         1
#define PSI_CFG_USE_IOM         1
#define PSI_CFG_USE_IOD         1

#define PSI_CFG_MAX_SNMPX_MGR_SESSIONS  32

/* include eddx */
#define PSI_CFG_USE_EDDI        1
#define PSI_CFG_USE_EDDP        1
#define PSI_CFG_USE_EDDS        1
#define PSI_CFG_USE_EDDT        0

#define PSI_CFG_MAX_EDDI_DEVICES    4 // Maximum number of devices that the EDDI supports
#define PSI_CFG_USE_NRT_CACHE_SYNC  0
    
#define PSI_CFG_MAX_EDDP_DEVICES    4 // Maximum number of devices that the EDDP supports
#define PSI_CFG_EDDP_HW_HERA_SUPPORT  // Support HERA ASIC

#define PSI_CFG_MAX_EDDS_DEVICES    4 // Maximum number of devices that the EDDS supports

/* include optional components for LD */
#define PSI_CFG_USE_DNS         0

/* Select IP stack  */
#define PSI_CFG_TCIP_STACK_OPEN_BSD   0
#define PSI_CFG_TCIP_STACK_INTERNICHE 1
    
#if PSI_CFG_USE_IOC
    #define PSI_CFG_MAX_CL_DEVICES  512         // Max devices for create client 
    #define PSI_CFG_MAX_CL_OSU_DEVICES  16          // range 1..32 (see EDD_DCP_MAX_DCP_HELLO_FILTER(=32))
#endif

#if PSI_CFG_USE_IOM
    #define PSI_CFG_MAX_MC_DEVICES      32  // Max devices for create MC client
#endif

#if PSI_CFG_USE_IOD
    #define PSI_CFG_MAX_SV_DEVICES      32  // maximum numbers of IOD instances, e.g. 64 for IEPB link, 1 for other UseCases
    #define PSI_CFG_MAX_SV_IO_ARS       16  // maximum numbers of RTC123 IO CRs 
#endif

// Select the EDDI configuration
#define PSI_CFG_EDDI_CFG_SOC

// used because its like CPU 1517
#define PSI_CFG_EDDI_CFG_SII_USE_SPECIAL_EOI    4999    //100 µs on IRTE Rev 7

// Advanced variant supports both ASICs
#define PSI_CFG_EDDP_CFG_HW_ERTEC200P_SUPPORT
#define PSI_CFG_EDDP_CFG_HW_HERA_SUPPORT

// Select the EDDS configuration
#define PSI_EDDS_CFG_HW_INTEL   // support for Intel  Lower Layer -> Springville, Hartwell using PnDevDrv
#define PSI_EDDS_CFG_HW_KSZ88XX // support for Micrel Lower Layer -> Micrel Evaluation Boards using PnDevDrv
#define PSI_EDDS_CFG_HW_TI      // support for TI     Lower Layer -> TI Evaluation Boards using PnDevDrv

#if (PSI_CFG_USE_EDDP == 1)
    #define PSI_CFG_EDDP_CFG_ISR_POLLING_MODE      // interrupt context in UserMode    
#endif
    
// system config
#define PSI_CFG_USE_LD_COMP      1
#define PSI_CFG_USE_HD_COMP      1
    
//HIF config -> Advanced FW must support HIF LD and HIF HD. HIF HD is required for Advanced (2,0).
#define PSI_CFG_USE_HIF_LD      1
#define PSI_CFG_USE_HIF_HD      1

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PNBOARDS_PSI_CFG_PLF_ADVANCED_SOC1_ERTEC200P_INTERNICHE_H */
