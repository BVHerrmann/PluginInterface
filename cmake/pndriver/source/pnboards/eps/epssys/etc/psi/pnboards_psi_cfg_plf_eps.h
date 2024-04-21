#ifndef PNBOARDS_PSI_CFG_PLF_EPS_H      /* ----- reinclude-protection ----- */
#define PNBOARDS_PSI_CFG_PLF_EPS_H

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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: pnboards_psi_cfg_plf_eps.h                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI EPS Platform defintions.                                             */
/*                                                                           */
/*****************************************************************************/

/* included by AntGen Toolchain in <props.xml> */

#include "eps_plf_types.h"  /* for defines of EPS_PLATFORM and EPS_PLF */
    
/*===========================================================================*/
/*        Platform settings                                                  */
/*===========================================================================*/
/* PLF_SOC, PLF_PNIP... is set by Toolchain */
#if defined (PLF_SOC)
    #define EPS_PLATFORM        EPS_PLATFORM_SOC
    #define EPS_PLF             EPS_PLF_SOC_MIPS
#elif defined (PLF_PNIP)
    #define EPS_PLATFORM        EPS_PLATFORM_PNIP
    #define EPS_PLF             EPS_PLF_PNIP_ARM9
#elif defined (PLF_HERA)
    #define EPS_PLATFORM        EPS_PLATFORM_PNIP
    #define EPS_PLF             EPS_PLF_PNIP_A53
#elif defined (PLF_AM5728)
    #define EPS_PLATFORM        EPS_PLATFORM_AM5728
    #define EPS_PLF             EPS_PLF_ARM_CORTEX_A15
#elif defined (PLF_ERTEC200)
    #define EPS_PLATFORM        EPS_PLATFORM_ERTEC200
    #define EPS_PLF             EPS_PLF_ERTEC200_ARM9
#elif defined (PLF_ERTEC400)
    #define EPS_PLATFORM        EPS_PLATFORM_ERTEC400
    #define EPS_PLF             EPS_PLF_ERTEC400_ARM9
#elif defined (PLF_ADONIS_X86)
    #define EPS_PLATFORM        EPS_PLATFORM_ADONIS_X86
    #define EPS_PLF             EPS_PLF_ADONIS_X86
#elif defined (PLF_PCIOX)
    #define EPS_PLATFORM        EPS_PLATFORM_PCIOX
    #define EPS_PLF             EPS_PLF_WINDOWS_X86
#elif defined (PLF_SOC_OBSD)
    #define EPS_PLATFORM        EPS_PLATFORM_SOC_OBSD /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
    #define EPS_PLF             EPS_PLF_SOC_MIPS
#elif defined (PLF_PNIP_OBSD)
    #define EPS_PLATFORM        EPS_PLATFORM_PNIP_OBSD /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
    #define EPS_PLF             EPS_PLF_PNIP_ARM9
#elif defined (PLF_HERA_OBSD)
    #define EPS_PLATFORM        EPS_PLATFORM_PNIP_OBSD /* OBSD_ITGR -- new platform for OpenBSD */
    #define EPS_PLF             EPS_PLF_PNIP_A53
#elif defined (PLF_AM5728_OBSD)
    #define EPS_PLATFORM        EPS_PLATFORM_AM5728_OBSD
    #define EPS_PLF             EPS_PLF_ARM_CORTEX_A15
#elif defined (PLF_ERTEC200_OBSD)
    #define EPS_PLATFORM        EPS_PLATFORM_ERTEC200_OBSD /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
    #define EPS_PLF             EPS_PLF_ERTEC200_ARM9
#elif defined (PLF_ERTEC400_OBSD)
    #define EPS_PLATFORM        EPS_PLATFORM_ERTEC400_OBSD /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
    #define EPS_PLF             EPS_PLF_ERTEC400_ARM9
#elif defined (PLF_ADONIS_X86_OBSD)
    #define EPS_PLATFORM        EPS_PLATFORM_ADONIS_X86_OBSD /* OBSD_ITGR -- new platform for OpenBSD (AP01559741) */
    #define EPS_PLF             EPS_PLF_ADONIS_X86
#elif defined (PLF_PNDRIVER_WINDOWS)
    #define EPS_PLATFORM        EPS_PLATFORM_PNDRIVER
    #define EPS_PLF             EPS_PLF_WINDOWS_X86
#elif defined (PLF_PNDRIVER_LINUX)
    #define EPS_PLATFORM        EPS_PLATFORM_PNDRIVER
    #define EPS_PLF             EPS_PLF_LINUX_X86
#elif defined (PLF_PNDRIVER_LINUX_IOT2000)
    #define EPS_PLATFORM        EPS_PLATFORM_PNDRIVER
    #define EPS_PLF             EPS_PLF_LINUX_IOT2000
#else
    #error "PLF is not defined!"
#endif

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

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
#define EPS_CFG_USE_EXTERNAL_LED

/*------------------------------------------------------------------------------
If this define is set, EPS opens the LD_UPPER interface and calls PSI_LD_OPEN
If you want to implement the PNUser (e.g. CM-PD User, IOC User, IOD User...)
in the same firmware/program as the EPS, set this define
Further information can be found in EPS_USER.docx
For the PNBoards variants (Basic, Advanced) the user is never included in the FW
------------------------------------------------------------------------------*/
//#define EPS_LOCAL_PN_USER

/*---------------------------------------------------------------------------*/
#if (EPS_PLATFORM == EPS_PLATFORM_NONE)
    /* compile test */
#elif (EPS_PLATFORM == EPS_PLATFORM_SOC)
    /* using EDDI and a SOC board */
    #include "pnboards_psi_cfg_plf_basic_soc1_interniche.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_PNIP)
    /* using EDDP and a PN-IP board */
    #include "pnboards_psi_cfg_plf_basic_ertec200p_interniche.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_AM5728)
    /* using EDDS and a AM5728 board */
    #include "pnboards_psi_cfg_plf_basic_am5728_interniche.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_ERTEC200)
    /* using EDDI and an EB200 board */
    #include "pnboards_psi_cfg_plf_basic_ertec200_interniche.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_ERTEC400)
    /* using EDDI and a CP1616 board */
    #include "pnboards_psi_cfg_plf_basic_ertec400_interniche.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_ADONIS_X86)
    /* using EDDI, EDDP, EDDS */
    #include "pnboards_psi_cfg_plf_advanced_soc1_ertec200p_interniche.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_PCIOX)
    /* using EDDI, EDDP, EDDS */
    /* pcIOX has its own includes */
#elif (EPS_PLATFORM == EPS_PLATFORM_SOC_OBSD)
    /* using EDDI and a SOC board */
    #include "pnboards_psi_cfg_plf_basic_soc1_obsd.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_PNIP_OBSD)
    /* using EDDP and a PN-IP board */
    #include "pnboards_psi_cfg_plf_basic_ertec200p_obsd.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_ERTEC200_OBSD)
    /* using EDDI and an EB200 board */
    #include "pnboards_psi_cfg_plf_basic_ertec200_obsd.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_AM5728_OBSD)
    /* using EDDS and a AM5728 board */
    #include "pnboards_psi_cfg_plf_basic_am5728_obsd.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_ERTEC400_OBSD)
    /* using EDDI and a CP1616 board */
    #include "pnboards_psi_cfg_plf_basic_ertec400_obsd.h"
#elif (EPS_PLATFORM == EPS_PLATFORM_ADONIS_X86_OBSD)
    /* using EDDI, EDDP, EDDS */
    #include "pnboards_psi_cfg_plf_advanced_soc1_ertec200p_obsd.h"
#elif ((EPS_PLATFORM == EPS_PLATFORM_PNDRIVER) && (EPS_PLF == EPS_PLF_WINDOWS_X86))
    /* (EPS_PLATFORM = EPS_PLF_WINDOWS_X86) is set by PNDriver */
    #include "pnd_psi_cfg_plf_windows_wpcap_interniche.h"
#elif ((EPS_PLATFORM == EPS_PLATFORM_PNDRIVER) && (EPS_PLF == EPS_PLF_LINUX_X86))
    /* (EPS_PLATFORM = EPS_PLF_LINUX_X86) is set by PNDriver */
    #include "pnd_psi_cfg_plf_linux_intel_interniche.h"
#elif ((EPS_PLATFORM == EPS_PLATFORM_PNDRIVER) && (EPS_PLF == EPS_PLF_LINUX_IOT2000))
    /* EPS_PLATFORM and EPS_PLF are set by PNDriver */
    #include "pnd_psi_cfg_plf_linux_iot2000_obsd.h"
#else
    #error "EPS_PLATFORM unknown"
#endif

#if defined(_MSC_VER)

/* extra warnings for compile level 4 MsDev */
#pragma warning(4:4242) /* 'identifier' : conversion from 'type1' to 'type2', possible loss of data */
#pragma warning(4:4254) /* 'operator' : conversion from 'type1' to 'type2', possible loss of data */
#pragma warning(4:4289) /* nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope */

#pragma warning(error:4002) /* Zu viele uebergebene Parameter für das Makro 'XXX' */
#pragma warning(error:4013) /* 'function' undefined; assuming extern returning int */
#pragma warning(error:4020) /* zu viele parameter */
#pragma warning(error:4024) /* different types for formal and actual parameter */
#pragma warning(error:4028) /* formal parameter 'number' different from declaration */
#pragma warning(error:4029) /* declared formal parameter list different from definition */
#pragma warning(error:4033) /* 'XXX' muss einen Wert zurueckgeben */
#pragma warning(error:4047) /* differs in levels of indirection  */
#pragma warning(error:4133) /* incompatible types */
#pragma warning(error:4553) /* '==' : Operator hat keine Auswirkungen; ist '=' beabsichtigt? */
#pragma warning(error:4700) /* warning C4700: uninitialized local variable 'XXX' used */
#pragma warning(error:4706) /* assignment w/i conditional expression */
#pragma warning(error:4709) /* command operator w/o index expression */

#pragma warning(disable/*once*/:4514) /* optimizer removed an inline function that is not called */

#endif /* defined(_MSC_VER) */
#define _DEBUG_

#if defined(_DEBUG_)
  #define PSI_DEBUG     1
#else
  #define PSI_DEBUG     0
#endif

/*---------------------------------------------------------------------------*/
/* EPS Framework Feature defines                                             */
/*---------------------------------------------------------------------------*/

#define EPS_TASKS_USE_EVENT_TASKS

/*---------------------------------------------------------------------------*/
/* Common defines valid for all targets                                      */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_MAX_PORT_CNT                    4 /* Maximum number of ports supported by EDD API services        */

#define PSI_CFG_MAX_SOCK_APP_CHANNELS           2 /* Maximum number of user application sock channels, Values (1..8) */
#define PSI_CFG_MAX_DIAG_TOOLS                  8 /* Maximum number of diagnosis tools used (TIA, ...)               */
    
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

/* include optional components for HD */
/* #define PSI_CFG_USE_EDDI       0/1 < Set by PNBoards variant */
/* #define PSI_CFG_USE_EDDP       0/1 < Set by PNBoards variant */
/* #define PSI_CFG_USE_EDDS       0/1 < Set by PNBoards variant */
/* #define PSI_CFG_USE_EDDT       0/1 < Set by PNBoards variant */
#define PSI_CFG_USE_GSY        1
#define PSI_CFG_USE_MRP        1
#define PSI_CFG_USE_POF        1

/* include logical device if LD is used. These switches are mandatory if the LD part is used*/
#define PSI_CFG_USE_TCIP        1
    
/* include optional components for LD */
#define PSI_CFG_USE_HSA         1
#define PSI_CFG_USE_SOCKAPP     1

/* #define PSI_CFG_USE_DNS        0/1 < Set by PNBoards variant, active for OBSD Variants. */

/* Set configuration for system adaptation modules */
#define PSI_CFG_USE_PNTRC       1
#define PSI_CFG_USE_EPS_RQBS    1

/*---------------------------------------------------------------------------*/
/* Configure HIF                                                             */
/*---------------------------------------------------------------------------*/
#define PSI_CFG_USE_HIF         1

#if (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)  /* HD only firmware. HIF LD not required */
    #define PSI_CFG_HIF_CFG_MAX_LD_INSTANCES            0   // HD only Variant => No LD
    #define PSI_CFG_HIF_CFG_MAX_HD_INSTANCES            1   // Number of Hw Devices in HD only firmware
#else                                                       /* LD only or LD and HD firmware. */
    #define PSI_CFG_HIF_CFG_MAX_LD_INSTANCES            1   // anzahl der USER LDs
    /* Use the value from the pnboards_psi_cfg_*** headers if available*/
    #ifndef PSI_CFG_HIF_CFG_MAX_HD_INSTANCES
        /* otherwise set the default value */
        #define PSI_CFG_HIF_CFG_MAX_HD_INSTANCES        (PSI_CFG_MAX_IF_CNT * 2) // Number of Hw Devices*2 in LD firmware
    #endif
#endif

#define PSI_CFG_USE_IOH                             1
#define PSI_CFG_HIF_CFG_COMPILE_SERIALIZATION       1   // Use Serialization

/*---------------------------------------------------------------------------*/
/* Map PSI switches to EPS switches. Used for task generation                */
/*---------------------------------------------------------------------------*/
#define EPS_CFG_USE_EDDI        (PSI_CFG_USE_EDDI && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDP        (PSI_CFG_USE_EDDP && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDS        (PSI_CFG_USE_EDDS && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDT        (PSI_CFG_USE_EDDT && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_GSY         (PSI_CFG_USE_GSY && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_LLDP        (PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_MRP         (PSI_CFG_USE_MRP && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_DNS         (PSI_CFG_USE_DNS && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_SNMPX       (PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_SOCK        (PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_TCIP        (PSI_CFG_USE_TCIP && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_CLRPC       (PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_OHA         (PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_ACP         (PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_CM          (PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_DCP         (PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_NARE        (PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_HIF         (PSI_CFG_USE_HIF) 
#define EPS_CFG_USE_HIF_LD      (PSI_CFG_USE_HIF_LD)
#define EPS_CFG_USE_HIF_HD      (PSI_CFG_USE_HIF_HD)
#define EPS_CFG_USE_PSI         1            //EPS uses PSI (Starts PSI task in eps_task.c)
#define EPS_CFG_USE_POF         (PSI_CFG_USE_POF && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_IOH         (PSI_CFG_USE_IOH && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_EPS         1            //EPS uses EPS (Starts EPS task in eps_task.c)
#define EPS_CFG_USE_HSA         (PSI_CFG_USE_HSA && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_SOCKAPP     (PSI_CFG_USE_SOCKAPP && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_STATISTICS  0            //EPS uses no statistics

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
#define TOOL_CHAIN_GNU
#undef  TOOL_CHAIN_GREENHILLS_ARM
#endif

#undef  TOOL_CHAIN_TASKING_TRICORE
#undef  TOOL_CHAIN_GNU_PPC
#undef  TOOL_CHAIN_CC386
#undef  TOOL_CHAIN_NRK

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

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PNBOARDS_PSI_CFG_PLF_EPS_H */
