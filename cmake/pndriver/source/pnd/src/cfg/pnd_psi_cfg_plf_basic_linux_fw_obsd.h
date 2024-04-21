#ifndef PND_PSI_CFG_PLF_BASIC_LINUX_FW_OBSD_H      /* ----- reinclude-protection ----- */
#define PND_PSI_CFG_PLF_BASIC_LINUX_FW_OBSD_H		

#ifdef __cplusplus                                  /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*  File is derived from pnboards_psi_cfg_plf_basic_soc1_obsd.h              */
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
/*  F i l e               &F: pnd_psi_cfg_plf_basic_fw_obsd.h           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI PnDriver settings for SOC1 board and TCIP-Stack is OpenBSD.          */
/*                                                                           */
/*****************************************************************************/

#include "eps_plf_types.h"  /* for defines of EPS_PLATFORM and EPS_PLF */

/* EPS_PLATFORM_PNDRIVER and EPS_PLF_LINUX_SOC1 are defined in <eps_plf_types.h> */
#define EPS_PLATFORM    EPS_PLATFORM_PNDRIVER
#define EPS_PLF         EPS_PLF_LINUX_SOC1

#define EPS_PLATFORM_NAME "SOC_OBSD"

/*------------------------------------------------------------------------------
This define activates the out macros
- for EDDS:
-- EPS_APP_LL_LED_BACKUP_MODE
-- EPS_APP_LL_LED_RESTORE_MODE
-- EPS_APP_LL_LED_SET_MODE
- for EDDI:
-- EPS_APP_LL_LED_BLINK_START
-- EPS_APP_LL_LED_BLINK_END
-- EPS_APP_LL_LED_BLINK_SET_MODE
and allows the user to use external implementations to drive the LEDs
------------------------------------------------------------------------------*/
#undef EPS_CFG_USE_EXTERNAL_LED  // #undef for linux application 

/*------------------------------------------------------------------------------
This define sets the IO memory to board accessible host mem. Currently SOC1 board only.
------------------------------------------------------------------------------*/
#define EPS_CFG_USE_IO_HOSTMEM


#define EPS_CFG_USE_LD  1   /* LD is used */

#define _DEBUG_

#if defined(_DEBUG_)
  #define PSI_DEBUG     1
#else
  #define PSI_DEBUG     0
#endif


#define PSI_CFG_EDDI_CFG_APPLSYNC_SEPARATE
/*---------------------------------------------------------------------------*/
/* EPS Framework Feature defines                                             */
/*---------------------------------------------------------------------------*/

#define EPS_TASKS_USE_EVENT_TASKS

/*---------------------------------------------------------------------------*/
/* Common defines valid for all targets                                      */
/*---------------------------------------------------------------------------*/

/* Maximum number of PNIO interfaces (HDs) of all EDDs integrated in a system: 1x PNIO interface = 1x HD = 1x EDD */
#define PSI_CFG_MAX_PNIO_IF_CNT      4			 // added for linux application 

// Compile Keys for EPS (CM/SNMPX role specific) features
#define PSI_CFG_USE_IOC         1 
#define PSI_CFG_USE_IOM         0 // added for linux application 
#define PSI_CFG_USE_IOD         0 // added for linux application 

#define PSI_CFG_MAX_SNMPX_MGR_SESSIONS  32

#define PSI_CFG_MAX_PORT_CNT                    4 /* Maximum number of ports supported by EDD API services        */

#define PSI_CFG_MAX_SOCK_APP_CHANNELS           2 /* Maximum number of user application sock channels, Values (1..8) */
#define PSI_CFG_MAX_DIAG_TOOLS                  8 /* Maximum number of diagnosis tools used (TIA, ...)               */
    
#define PSI_CFG_CM_CFG_SIMATIC_B000             0 // added for linux application 
#define PSI_CFG_CM_CFG_CL_ALLOW_DIAG_AND_PDEV   0 // added for linux application 


/**
 * This define enables the TCP communication channel from SOCK to EDDx
 * By disabling this define a TCP communication is not available (e.g. webserver)
 * ARP, ICMP and UDP are still working
 */
#define PSI_CFG_TCIP_CFG_SUPPORT_PATH_EDD_TCP

/**
 * Using COPY IF for TCIP
 */
#define PSI_CFG_TCIP_CFG_COPY_ON_SEND   1

/**
 * Enable the automatic filling up to 3 bytes with 0 in NRT-Send-Buffer by EDDI     
 * Switching on is only allowed, if PSI_CFG_TCIP_CFG_COPY_ON_SEND is defined!          
 */
#define PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT
 
/** Finish configuration of IP Stack. 
 * PNBoard variants set either:
 * PSI_CFG_TCIP_STACK_OPEN_BSD = 1 and PSI_CFG_TCIP_STACK_INTERNICHE = 0 or 
 * PSI_CFG_TCIP_STACK_OPEN_BSD = 0 and PSI_CFG_TCIP_STACK_INTERNICHE = 1 
 * PSI_CFG_TCIP_CFG_WINSOCK and PSI_CFG_TCIP_STACK_CUSTOM are not supported for PNBoards variants, set them to 0 here.
 */
#define PSI_CFG_TCIP_STACK_WINSOCK    0
#define PSI_CFG_TCIP_STACK_CUSTOM     0

/*---------------------------------------------------------------------------*/
/* PNIO component usage for task creation                                    */
/*---------------------------------------------------------------------------*/
    
/* include mandatory components for HD */
#define PSI_CFG_USE_HD_COMP     1

/* include optional components for HD */
#define PSI_CFG_USE_EDDI        1
#define PSI_CFG_USE_EDDP        0
#define PSI_CFG_USE_EDDS        0
#define PSI_CFG_USE_EDDT        0
#define PSI_CFG_USE_GSY         1
#define PSI_CFG_USE_MRP         0
#define PSI_CFG_USE_POF         0

/* include logical device if LD is used. These switches are mandatory if the LD part is used*/
#define PSI_CFG_USE_TCIP        1
#define PSI_CFG_USE_LD_COMP     1
    
/* include optional components for LD */
#define PSI_CFG_USE_HSA         0
#define PSI_CFG_USE_SOCKAPP     0
#define PSI_CFG_USE_DNS         0
#define PSI_CFG_USE_IOH         0

/* Set configuration for system adaptation modules */
#define PSI_CFG_USE_PNTRC       1
#define PSI_CFG_USE_EPS_RQBS    1

/* Select IP stack  */
#define PSI_CFG_TCIP_STACK_OPEN_BSD     1
#define PSI_CFG_TCIP_STACK_INTERNICHE   0

/* Maximum number of devices that the EDDI supports */
#define PSI_CFG_MAX_EDDI_DEVICES        1

#define PSI_CFG_USE_NRT_CACHE_SYNC      1

// This EPS variant calls psi_hd_interrupt and psi_hd_eddi_exttimer_interrupt in kernel mode context. The switch must be set.
// Also see eps_internal_cfg -> EPS_ISR_MODE_SOC1 has to be set to EPS_ISR_MODE_IR_KERNELMODE, otherwise undef this.

#undef PSI_CFG_LOCK_MSGBOX_WITH_IR_LOCK 	// undef for linux application 
#undef PSI_CFG_USE_NRT_CACHE_SYNC      		// undef for linux application 

#if PSI_CFG_USE_IOC
    #define PSI_CFG_MAX_CL_DEVICES      512 // Max devices for create client 
    #define PSI_CFG_MAX_CL_OSU_DEVICES  16  // range 1..32 (see EDD_DCP_MAX_DCP_HELLO_FILTER(=32))
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

// used in both variants HD ONLY and LD+DH (like CPU 1518 and 1516)
#define PSI_CFG_EDDI_CFG_SII_USE_SPECIAL_EOI    4999    //100 �s on IRTE Rev 7

/*---------------------------------------------------------------------------*/
/* Configure HIF                                                             */
/*---------------------------------------------------------------------------*/
#define PSI_CFG_USE_HIF         1
#define PSI_CFG_USE_HIF_LD      1
#define PSI_CFG_USE_HIF_HD      0

#define PSI_CFG_HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC
 
#define PSI_CFG_USE_LD_COMP     1
#define PSI_CFG_USE_HD_COMP     1
    
#define PSI_CFG_HIF_CFG_COMPILE_SERIALIZATION       1   // Use Serialization
#define PSI_CFG_HIF_CFG_MAX_LD_INSTANCES            1   // HD only Variant => No LD
#define PSI_CFG_HIF_CFG_MAX_HD_INSTANCES            PSI_CFG_MAX_IF_CNT * 2   // Number of Hw Devices in HD only firmware
  
/*---------------------------------------------------------------------------*/
/* Configure IOH                                                             */
/*---------------------------------------------------------------------------*/
#if defined EPS_CFG_USE_IO_HOSTMEM
/* This define sets the IO memory to board accessible host mem.Currently SOC1 board only. */
#define PSI_CFG_USE_IO_HOSTMEM
#endif
  
/*---------------------------------------------------------------------------*/
/* Map PSI switches to EPS switches. Used for task generation                */
/*---------------------------------------------------------------------------*/
#define EPS_CFG_USE_EDDI        (PSI_CFG_USE_EDDI && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDP        (PSI_CFG_USE_EDDP && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDS        (PSI_CFG_USE_EDDS && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDT        (PSI_CFG_USE_EDDT && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_GSY         (PSI_CFG_USE_GSY && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_LLDP        PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_MRP         (PSI_CFG_USE_MRP && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_DNS         (PSI_CFG_USE_DNS && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_SNMPX       PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_SOCK        PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_TCIP        (PSI_CFG_USE_TCIP && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_CLRPC       PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_OHA         PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_ACP         PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_CM          PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_DCP         PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_NARE        PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_HIF         (PSI_CFG_USE_HIF) 
#define EPS_CFG_USE_HIF_LD      (PSI_CFG_USE_HIF_LD)
#define EPS_CFG_USE_HIF_HD      (PSI_CFG_USE_HIF_HD)
#define EPS_CFG_USE_PSI         1                       //EPS uses PSI (Starts PSI task in eps_task.c)
#define EPS_CFG_USE_POF         (PSI_CFG_USE_POF && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_HSA         PSI_CFG_USE_HSA
#define EPS_CFG_USE_IOH         (PSI_CFG_USE_IOH && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_EPS         1                       //EPS uses EPS (Starts EPS task in eps_task.c)
#define EPS_CFG_USE_SOCKAPP     (PSI_CFG_USE_SOCKAPP && PSI_CFG_USE_LD_COMP) 
#define EPS_CFG_USE_STATISTICS  0                       //EPS does not use statistics

/*===========================================================================*/
/*        LSA compiler-switches                                              */
/*===========================================================================*/

/*===========================================================================*/
/* Select the TOOL Chain for adaption LSA Types to Toolchain (see lsa_cfg.h) */
/*  #define TOOL_CHAIN_MICROSOFT   MS-C/C++ Compiler                         */
/*  #define TOOL_CHAIN_TASKING_TRICORE                                       */
/*  #define TOOL_CHAIN_GNU_PPC                                               */
/*  #define TOOL_CHAIN_CC386                                                 */
/*  #define TOOL_CHAIN_GREENHILLS_ARM                                        */
/*  #define TOOL_CHAIN_NRK  ... ARM/Thumb C/C++ Compiler                     */
/*===========================================================================*/

#ifdef EPS_USE_RTOS_WINDOWS
#define TOOL_CHAIN_MICROSOFT
#undef  TOOL_CHAIN_GREENHILLS_ARM
#elif defined TOOL_CHAIN_GREENHILLS_ARM
#undef TOOL_CHAIN_MICROSOFT
#undef TOOL_CHAIN_GNU
#else
#ifndef TOOL_CHAIN_GNU // prevent redefinition error
#define TOOL_CHAIN_GNU 
#endif // !TOOL_CHAIN_GNU
#undef  TOOL_CHAIN_GREENHILLS_ARM
#endif

#undef  TOOL_CHAIN_TASKING_TRICORE
#undef  TOOL_CHAIN_GNU_PPC
#undef  TOOL_CHAIN_CC386
#undef  TOOL_CHAIN_NRK

// Select the EDDI configuration
#define EDDI_CFG_SET_ERTEC_VERSION_EXTERN // added for linux application 
#define EDDI_CFG_SOC					  // added for linux application 

/*============================================================================
 *       Endianess:
 * If you have a computer system whitch stors most significant byte at the
 * lowest address of the word/doubleword: Define this
 * #define LSA_HOST_ENDIANESS_BIG
 * else define this
 * #define LSA_HOST_ENDIANESS_LITTLE
 *===========================================================================*/

#undef  LSA_HOST_ENDIANESS_BIG
#define LSA_HOST_ENDIANESS_LITTLE

/**
* This enables PNTRC to write the checksum of the tracemap into the header sections of the trace buffers.
* Do not enable this config switch if your toolchain is unable to run ltrcscanner to generate the pntrc_checksum.c
*/
#define PNTRC_CFG_CHECK_SCANNERINFO
/*===========================================================================*/

// Added for for linux application 
/*------------------------------------------------------------------------------
// interface to LSA trace
//  0 .. no LSA trace
//  1 .. LSA trace [default]
//  2 .. LSA index trace
//----------------------------------------------------------------------------*/
#define PSI_CFG_TRACE_MODE  2

#define PSI_CFG_MAX_IF_CNT	PSI_CFG_MAX_PNIO_IF_CNT // added for linux application 

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_PSI_CFG_PLF_BASIC_FW_OBSD_H */
