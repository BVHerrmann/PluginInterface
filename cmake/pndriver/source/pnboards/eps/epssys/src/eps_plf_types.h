#ifndef EPS_PLF_TYPES_H
#define EPS_PLF_TYPES_H

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_plf_types.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Types for platform settings                                              */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*        Platform settings                                                  */
/*===========================================================================*/
/* for EPS_PLATFORM */
#define EPS_PLATFORM_NONE               1
#define EPS_PLATFORM_SOC                4   // EDDI only
#define EPS_PLATFORM_PNIP               5   // EDDP only
#define EPS_PLATFORM_ERTEC200           6   // EDDI only
#define EPS_PLATFORM_ERTEC400           7   // EDDI only
#define EPS_PLATFORM_ADONIS_X86         8   // EDDI, EDDP, EDDS
#define EPS_PLATFORM_PCIOX              9   // PCIOX APP
#define EPS_PLATFORM_SOC_OBSD           11  // EDDI only /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
#define EPS_PLATFORM_PNIP_OBSD          12  // EDDP only /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
#define EPS_PLATFORM_ERTEC200_OBSD      13  // EDDI only /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
#define EPS_PLATFORM_ERTEC400_OBSD      14  // EDDI only /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
#define EPS_PLATFORM_ADONIS_X86_OBSD    15  // EDDI, EDDP, EDDS /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
#define EPS_PLATFORM_PNDRIVER           16
#define EPS_PLATFORM_AM5728             17  // EDDS only and TCIP-Stack is interniche
#define EPS_PLATFORM_AM5728_OBSD        18  // EDDS only and TCIP-Stack is OpenBSD

/* for EPS_PLF */
#define EPS_PLF_SOC_MIPS                1   // Adonis on MIPS of SOC1 board
#define EPS_PLF_PNIP_ARM9               2   // Adonis on ARM9 of EB200P board
#define EPS_PLF_ERTEC200_ARM9           3   // Adonis on ARM9 of EB200 or EB200 PCIe board
#define EPS_PLF_ERTEC400_ARM9           4   // Adonis on ARM9 of CP1616 or EB400 PCIe board
#define EPS_PLF_ADONIS_X86              5   // Adonis on IntelX86
#define EPS_PLF_WINDOWS_X86             6   // Windows on IntelX86
#define EPS_PLF_LINUX_X86               7   // Linux on IntelX86 (PNDriver)
#define EPS_PLF_PNIP_A53                8   // Adonis on A53 of Hera board
#define EPS_PLF_ARM_CORTEX_A15          9   // Adonis on ARM-Cortex-A15 of AM5728 board
#define EPS_PLF_LINUX_SOC1              10  // Linux on SOC1 (PNDriver)
#define EPS_PLF_LINUX_IOT2000           11  // Linux on IOT2000 (PNDriver)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PLF_TYPES_H */
