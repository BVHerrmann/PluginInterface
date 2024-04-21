#ifndef EPS_INTERNAL_CFG_H_
#define EPS_INTERNAL_CFG_H_

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
/*  F i l e               &F: eps_internal_cfg.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS internal configurations                                              */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define EPS_NO      0
#define EPS_YES     1

#define EPS_TGROUP_STATISTICS   EPS_NO
/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
    
/*---------------------------------------------------------------------------
     Configure EPS Interrupt Mode for Basic Variants
---------------------------------------------------------------------------*/        
#include "eps_enums.h"
#if (EPS_PLF == EPS_PLF_LINUX_SOC1)
#define EPS_ISR_MODE_HIF_PNCORE     EPS_ISR_MODE_POLL            // eps_hif_pncorestd_drv.c
#else
#define EPS_ISR_MODE_HIF_PNCORE     EPS_ISR_MODE_IR_USERMODE     // eps_hif_pncorestd_drv.c
#endif
#define EPS_ISR_MODE_EB200P         EPS_ISR_MODE_IR_KERNELMODE   // eps_pn_eb200p_drv.c
#define EPS_ISR_MODE_EB200          EPS_ISR_MODE_POLL            // eps_pn_ertec200_drv.c - not used 2016-04-19
#define EPS_ISR_MODE_EB400          EPS_ISR_MODE_POLL            // eps_pn_ertec400_drv.c
#define EPS_ISR_MODE_WPCAP          EPS_ISR_MODE_POLL            // eps_wpcapdrv.c
#define EPS_ISR_MODE_HERA           EPS_ISR_MODE_IR_KERNELMODE   // eps_pn_hera_drv.c //todo hera: Only polling is allowed yet!
#define EPS_ISR_MODE_AM5728         EPS_ISR_MODE_POLL            // eps_pn_am5728_drv.c

#if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
#define EPS_IPC_ACK_MODE            EPS_IPC_ACK_MODE_NONE        // eps_ipc_pndevdrv_drv.c, eps_ipc_linux_soc_drv.c
#else
#define EPS_IPC_ACK_MODE            EPS_IPC_ACK_MODE_NO_HERA     // eps_ipc_generic_drv.c, eps_ipc_pndevdrv_drv.c
#endif

#if (EPS_PLF == EPS_PLF_LINUX_SOC1)
#define EPS_ISR_MODE_SOC1           EPS_ISR_MODE_IR_USERMODE     // eps_pn_soc1_drv.c -> for Linux on SoC1
#else
#define EPS_ISR_MODE_SOC1           EPS_ISR_MODE_IR_KERNELMODE   // eps_pn_soc1_drv.c
#endif
/*-------------------------------------------------------------------------*/
    
#if ((EPS_PLF == EPS_PLF_SOC_MIPS) && (EPS_ISR_MODE_SOC1   == EPS_ISR_MODE_IR_KERNELMODE))
    #ifndef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK
    #error "Error in configuration. Parts of PSI now run in kernel mode!"
    #endif 
#endif

#if ((EPS_PLF == EPS_PLF_PNIP_ARM9) && (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_KERNELMODE))
    #ifndef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK
    #error "Error in configuration. Parts of PSI now run in kernel mode!"
    #endif 
#endif

#if ((EPS_PLF == EPS_PLF_LINUX_SOC1) && (EPS_ISR_MODE_SOC1   == EPS_ISR_MODE_IR_USERMODE))
    #ifdef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK
    #error "Error in configuration. No part of PSI is running in kernel mode!"
    #endif 
#endif
    
#if ((EPS_PLF == EPS_PLF_PNIP_A53) && (EPS_ISR_MODE_HERA == EPS_ISR_MODE_IR_KERNELMODE))
    #ifndef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK
    #error "Error in configuration. Parts of PSI now run in kernel mode!"
    #endif 
#endif

#if ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) && (EPS_ISR_MODE_EB200 == EPS_ISR_MODE_IR_KERNELMODE))
    #ifndef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK
    #error "Error in configuration. Parts of PSI now run in kernel mode!"
    #endif 
#endif

#if ((EPS_PLF == EPS_PLF_ERTEC400_ARM9) && (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_KERNELMODE))
    #ifndef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK
    #error "Error in configuration. Parts of PSI now run in kernel mode!"
    #endif 
#endif
/*---------------------------------------------------------------------------
Configure EPS Memory statistics
---------------------------------------------------------------------------*/
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
// cpu heavy statistics and tracing of memory allocations should only be done in debug case or in systems with good performance
#define EPS_USE_MEM_STATISTIC                      /* Define if EPS Mem Statistic should be used */

#endif

/*-------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_INTERNAL_CFG_H_ */
