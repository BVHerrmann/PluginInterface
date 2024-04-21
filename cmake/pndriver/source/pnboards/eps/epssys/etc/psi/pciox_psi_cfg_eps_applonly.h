#ifndef PCIOX_PSI_CFG_EPS_APPLONLYH     /* ----- reinclude-protection ----- */
#define PCIOX_PSI_CFG_EPS_APPLONLYH

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
/*  F i l e               &F: pciox_psi_cfg_eps_applonly.h              :F&  */
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

/* included by "psi_cfg.h" */
#include "eps_plf_types.h"  /* for defines of EPS_PLATFORM and EPS_PLF */

/*===========================================================================*/
/*        Platform settings                                                  */
/*===========================================================================*/

#define PCIOX_PLATFORM_E400           1   // EDDI (E400) only
#define PCIOX_PLATFORM_E200           2   // EDDI (E200) only
#define PCIOX_PLATFORM_SOC1           3   // EDDI (SOC1) only
#define PCIOX_PLATFORM_PNIP           4   // EDDP only
#define PCIOX_PLATFORM_WPCAP          5   // EDDS with WPCAP and TCIP only
#define PCIOX_PLATFORM_WINSOCK        6   // EDDS with WPCAP and WINSOCK only
#define PCIOX_PLATFORM_ALL_E400       7   // EDDI (E400), EDDP and EDDS 
#define PCIOX_PLATFORM_ALL_E200       8   // EDDI (E200), EDDP and EDDS 
#define PCIOX_PLATFORM_ALL_SOC1       9   // EDDI (SOC), EDDP and EDDS 
#define PCIOX_PLATFORM_ALL_E400_OBSD  10  // EDDI (E400), EDDP and EDDS using OBSD 
#define PCIOX_PLATFORM_ALL_E200_OBSD  11  // EDDI (E200), EDDP and EDDS using OBSD  
#define PCIOX_PLATFORM_ALL_SOC1_OBSD  12  // EDDI (SOC), EDDP and EDDS using OBSD  
#define PCIOX_PLATFORM_WPCAP_OBSD     13
#define PCIOX_PLATFORM_APPLONLY       14

#ifndef PCIOX_PLATFORM
#error "PCIOX_PLATFORM not defined (in vcproj or makefile)"
#endif

/* see %USERNAME% */
#ifdef PCIOX_USER_atw117b0   // user GS
#define PCIOX_CFG_MESSAGE
#endif
#ifdef PCIOX_USER_atw112d0   // user HS
#define PCIOX_CFG_MESSAGE
#endif
#ifdef PCIOX_USER_atw116g0   // user FS
#define PCIOX_CFG_MESSAGE
#endif
#ifdef PCIOX_USER_pnio       // default user TestPC
#define PCIOX_CFG_MESSAGE
#endif

#define PCIOX_STRINGIFY_(n)  # n
#define PCIOX_STRINGIFY(n)   PCIOX_STRINGIFY_(n) // make it a string

#ifdef PCIOX_CFG_MESSAGE
#define PCIOX_MESSAGE(str_) message( __FILE__ "(" PCIOX_STRINGIFY(__LINE__) ") : " str_)
#endif

/* extra warnings for compile level 4 MsDev */
#pragma warning(4:4242) /* 'identifier' : conversion from 'type1' to 'type2', possible loss of data */
#pragma warning(4:4254) /* 'operator' : conversion from 'type1' to 'type2', possible loss of data */
#pragma warning(4:4289) /* nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope */

#pragma warning(error:4002) /* too many parameter for macro 'XXX' */
#pragma warning(error:4013) /* 'function' undefined; assuming extern returning int */
#pragma warning(error:4020) /* too many parameter */
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

/*===========================================================================*/
/*        Debug settings                                                     */
/*===========================================================================*/

#if defined (_DEBUG)
  #define PSI_DEBUG     1
#else
  #define PSI_DEBUG     0
#endif

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
#define TOOL_CHAIN_MICROSOFT

/*============================================================================*/
/*       Endianess:
/* If you have a computer system whitch stors most significant byte at the
/* lowest address of the word/doubleword: Define this
/* #define LSA_HOST_ENDIANESS_BIG
/* else define this
/* #define LSA_HOST_ENDIANESS_LITTLE
 *===========================================================================*/
#undef  LSA_HOST_ENDIANESS_BIG
#define LSA_HOST_ENDIANESS_LITTLE

/*------------------------------------------------------------------------------
// PN Platform defines (EPS compile keys)
//----------------------------------------------------------------------------*/
/* EPS_PLATFORM_PCIOX and EPS_PLF_WINDOWS_X86 are defined in <eps_plf_types.h> */
#define EPS_PLATFORM            EPS_PLATFORM_PCIOX
#define EPS_PLF                 EPS_PLF_WINDOWS_X86
#define EPS_LOCAL_PN_USER

#define EPS_SUPPRESS_PRINTF
#define EPS_CFG_DO_NOT_USE_TGROUPS

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
This define sets the IO memory to board accessible host mem. Currently SOC1 board only.
------------------------------------------------------------------------------*/
//#define EPS_CFG_USE_IO_HOSTMEM

/**
* This enables PNTRC to write the checksum of the tracemap into the header sections of the trace buffers.
* Do not enable this config switch if your toolchain is unable to run ltrcscanner to generate the pntrc_checksum.c
*/
#define PNTRC_CFG_CHECK_SCANNERINFO

/**
 * This define enables the TCP communication channel from SOCK to EDDx
 * By disabling this define a TCP communication is not available (e.g. webserver)
 * ARP, ICMP and UDP are still working
 */
#define PSI_CFG_TCIP_CFG_SUPPORT_PATH_EDD_TCP

/*---------------------------------------------------------------------------*/
/* PNIO component usage (compiling) keys                                     */
/*---------------------------------------------------------------------------*/
/* see platform specific include for settings                                */

#if PCIOX_PLATFORM == PCIOX_PLATFORM_WPCAP
	/* using EDDS, WinPcap and Interniche IP-Stack */
#include "pciox_psi_cfg_plf_wpcap.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_WINSOCK
	/* using EDDS, WinPcap and WINSOCK */
#include "pciox_psi_cfg_plf_winsock.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_ALL_E400
	/* using EDDI and an ERTEC400 board or EDDP and a PN-IP board or EDDS and StdMac board */
#include "pciox_psi_cfg_plf_all_e400.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_ALL_E200
	/* using EDDI and a EB200 board or EDDP and a PN-IP board or EDDS and StdMac board */
#include "pciox_psi_cfg_plf_all_e200.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_ALL_SOC1
	/* using EDDI and a SOC board or EDDP and a PN-IP board or EDDS and StdMac board */
#include "pciox_psi_cfg_plf_all_soc1.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_ALL_E400_OBSD
	/* using OBSD with EDDI and an ERTEC400 board or EDDP and a PN-IP board or EDDS and StdMac board */
#include "pciox_psi_cfg_plf_all_e400_obsd.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_ALL_E200_OBSD
	/* using OBSD with EDDI and a EB200 board or EDDP and a PN-IP board or EDDS and StdMac board */
#include "pciox_psi_cfg_plf_all_e200_obsd.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_ALL_SOC1_OBSD
	/* using OBSD with EDDI and a SOC board or EDDP and a PN-IP board or EDDS and StdMac board */
#include "pciox_psi_cfg_plf_all_soc1_obsd.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_WPCAP_OBSD
    /* using EDDS, WinPcap and Interniche IP-Stack */
#include "pciox_psi_cfg_plf_wpcap_obsd.h"
#elif PCIOX_PLATFORM == PCIOX_PLATFORM_APPLONLY
    /*   */
#include "pciox_psi_cfg_plf_applonly.h"
#else
#error "PCIOX_PLATFORM unknown"
#endif

/*---------------------------------------------------------------------------*/
/* PNIO component usage for task creation                                    */
/* Switches that are set here are common for PCIOX_NIF Variants,             */
/* Switches that are not set are PCIOX_NIF variant specific                  */
/*---------------------------------------------------------------------------*/

/* include optional components for HD */
/* #define PSI_CFG_USE_EDDI       0/1 < Set by PCIOX_NIF variant */
/* #define PSI_CFG_USE_EDDP       0/1 < Set by PCIOX_NIF variant */
/* #define PSI_CFG_USE_EDDS       0/1 < Set by PCIOX_NIF variant */
#define PSI_CFG_USE_GSY        1
#define PSI_CFG_USE_IOH        1
#define PSI_CFG_USE_MRP        1
/* #define PSI_CFG_USE_POF       0/1 < Set by PCIOX_NIF variant */

/* include optional components for LD */
#define PSI_CFG_USE_HSA        1
#define PSI_CFG_USE_SOCKAPP    1
/* #define PSI_CFG_USE_DNS        0/1 < Set by PCIOX_NIF variant, active for OBSD Variants. */
/* #define PSI_CFG_USE_TCIP       0/1 < Set by PCIOX_NIF variant. TODO: Should be 0 for WinSOCK after WinSOCK works again? */

/* Set configuration for HIF */
#define PSI_CFG_USE_HIF         1
#define PSI_CFG_USE_HIF_LD      1
#define PSI_CFG_USE_HIF_HD      0
#define PSI_CFG_USE_LD_COMP     0
#define PSI_CFG_USE_HD_COMP     0

/* Set configuration for system adaptation modules */
#define PSI_CFG_USE_PNTRC      1
#define PSI_CFG_USE_EPS_RQBS   1

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
#define EPS_CFG_USE_PSI         1                   //EPS uses PSI (Starts PSI task in eps_task.c)
#define EPS_CFG_USE_POF         (PSI_CFG_USE_POF && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_IOH         (PSI_CFG_USE_IOH && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_EPS         1                   //EPS uses EPS (Starts EPS task in eps_task.c)
#define EPS_CFG_USE_HSA         (PSI_CFG_USE_HSA && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_SOCKAPP     (PSI_CFG_USE_SOCKAPP && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_STATISTICS  0                   //EPS uses no statistics

#define PSI_CFG_USE_NRT_CACHE_SYNC  0       // Don't use cached NRT memory

/*---------------------------------------------------------------------------*/
/* Configure HIF                                                             */
/*---------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_HIF == 1 )
#define PSI_CFG_HIF_CFG_COMPILE_SERIALIZATION 1                         // Use Serialization
#define PSI_CFG_HIF_CFG_MAX_LD_INSTANCES      2                         // Number of LD * 2
#define PSI_CFG_HIF_CFG_MAX_HD_INSTANCES     ((PSI_CFG_MAX_IF_CNT)*2)   // Derive from  MAX HD *2, one for Upper and Lower each
#else
#define PSI_CFG_HIF_CFG_COMPILE_SERIALIZATION 0                         // Don't use Serialization
#define PSI_CFG_HIF_CFG_MAX_LD_INSTANCES      0                         // No need -> 0
#define PSI_CFG_HIF_CFG_MAX_HD_INSTANCES      0                         // No need -> 0
#endif // ( PSI_CFG_USE_HIF == 1 )

/*---------------------------------------------------------------------------*/
/* Configure IOH                                                             */
/*---------------------------------------------------------------------------*/
#if defined EPS_CFG_USE_IO_HOSTMEM
/* This define sets the IO memory to board accessible host mem.Currently SOC1 board only. */
#define PSI_CFG_IOH_USE_IO_HOSTMEM
#endif

/*---------------------------------------------------------------------------*/
/* Configure TCIP                                                            */
/*---------------------------------------------------------------------------*/

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
* PCIOX_NIF variants set either:
* -  PSI_CFG_TCIP_STACK_OPEN_BSD = 1, PSI_CFG_TCIP_STACK_INTERNICHE = 0, PSI_CFG_TCIP_STACK_WINSOCK = 0
* -  PSI_CFG_TCIP_STACK_OPEN_BSD = 0, PSI_CFG_TCIP_STACK_INTERNICHE = 1, PSI_CFG_TCIP_STACK_WINSOCK = 0
* -  PSI_CFG_TCIP_STACK_OPEN_BSD = 0, PSI_CFG_TCIP_STACK_INTERNICHE = 0, PSI_CFG_TCIP_STACK_WINSOCK = 1
* PSI_CFG_TCIP_STACK_CUSTOM is not supported for PCIOX_NIF variants, set it to 0 here.
*/
#define PSI_CFG_TCIP_STACK_CUSTOM     0

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PCIOX_PSI_CFG_EPS_APPLONLYH */
