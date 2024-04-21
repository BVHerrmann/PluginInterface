#ifndef PSI_CFG_H               /* ----- reinclude-protection ----- */
#define PSI_CFG_H

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: psi_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System integration of LSA-component PSI.                                 */
/*  Using the   PNIO framework.                                              */
/*  Note: In case of EPS common used keys are included from EPS.             */
/*                                                                           */
/*****************************************************************************/

/*
* included by "psi_inc.h"
*/

#define _EPSPSICFG_QUOTEME(x) #x
#define EPSPSICFG_QUOTEME(x) _EPSPSICFG_QUOTEME(x) 

#ifndef EPS_PSI_CFG_PLATFORM_H
    #error "EPS_PSI_CFG_PLATFORM_H not defined in makefiles"
#endif
#include EPSPSICFG_QUOTEME(EPS_PSI_CFG_PLATFORM_H)   /* EPS specific PSI compile keys which need to be defined by compiler args (e.g. -DEPS_PSI_CFG_PLATFORM_H=pnboards_psi_cfg_plf_eps.h) */

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#ifndef EPS_PLF
    #error "config error, EPS_PLF is not defined"
#endif

#ifndef PSI_DEBUG
    #error "config error, PSI_DEBUG is not defined"
#endif

/*---------------------------------------------------------------------------*/
/* definition for invalid LOCK- and Event-Handles                            */
/*---------------------------------------------------------------------------*/

#define PSI_LOCK_ID_INVALID     0
#define PSI_EVENT_NO_TIMEOUT    0

/*---------------------------------------------------------------------------*/
/* Type of systemhandles used by system for device and interface             */
/*---------------------------------------------------------------------------*/

#define PSI_SYS_HANDLE      LSA_VOID_PTR_TYPE
#define PSI_EDD_HDDB        LSA_VOID_PTR_TYPE

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/
/* none PSI only supports flat memory modell */

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/
/* none PSI only supports flat memory modell */

/*===========================================================================*/
/* LSA-HEADER and LSA-TRAILER                                                */
/*===========================================================================*/

#define PSI_RQB_HEADER      LSA_RQB_HEADER(PSI_UPPER_RQB_PTR_TYPE)

#define PSI_RQB_TRAILER     LSA_RQB_TRAILER

#define PSI_RQB_SET_NEXT_RQB_PTR(rb,v)      LSA_RQB_SET_NEXT_RQB_PTR(rb,v)
#define PSI_RQB_SET_PREV_RQB_PTR(rb,v)      LSA_RQB_SET_PREV_RQB_PTR(rb,v)
#define PSI_RQB_SET_OPCODE(rb,v)            LSA_RQB_SET_OPCODE(rb,v)
#define PSI_RQB_SET_HANDLE(rb,v)            LSA_RQB_SET_HANDLE(rb,v)
#define PSI_RQB_SET_RESPONSE(rb,v)          LSA_RQB_SET_RESPONSE(rb,v)
#define PSI_RQB_SET_USER_ID_PTR(rb,v)       LSA_RQB_SET_USER_ID_PTR(rb,v)
#define PSI_RQB_SET_COMP_ID(rb,v)           LSA_RQB_SET_COMP_ID(rb,v)

#define PSI_RQB_GET_NEXT_RQB_PTR(rb)        LSA_RQB_GET_NEXT_RQB_PTR(rb)
#define PSI_RQB_GET_PREV_RQB_PTR(rb)        LSA_RQB_GET_PREV_RQB_PTR(rb)
#define PSI_RQB_GET_OPCODE(rb)              LSA_RQB_GET_OPCODE(rb)
#define PSI_RQB_GET_HANDLE(rb)              LSA_RQB_GET_HANDLE(rb)
#define PSI_RQB_GET_RESPONSE(rb)            LSA_RQB_GET_RESPONSE(rb)
#define PSI_RQB_GET_USER_ID_PTR(rb)         LSA_RQB_GET_USER_ID_PTR(rb)
#define PSI_RQB_GET_COMP_ID(rb)             LSA_RQB_GET_COMP_ID(rb)

#define PSI_EDD_RQB_SET_HANDLE_LOWER(rb,v)  EDD_RQB_SET_HANDLE_LOWER(rb,v)

#define PSI_EDD_RQB_GET_HANDLE_LOWER(rb)    EDD_RQB_GET_HANDLE_LOWER(rb)

/*------------------------------------------------------------------------------
// enable / disable PSI_ASSERT
//----------------------------------------------------------------------------*/

#if PSI_DEBUG

    /* check for condition: internal programming error */
    #define PSI_ASSERT(cond) if (! (cond)) PSI_FATAL (0) 

#else

    /* no more programming error exists :-) */
	#define PSI_ASSERT(cond) { LSA_UNUSED_ARG(cond); } // Added body to avoid unreferenced parameters compiler warnings.

#endif

/*------------------------------------------------------------------------------
// pointer test
//----------------------------------------------------------------------------*/

void* PSI_TEST_POINTER( void const * ptr );
void* PSI_TEST_ALIGN2( void const * ptr );
void* PSI_TEST_ALIGN4( void const * ptr );

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef PSI_FILE_SYSTEM_EXTENSION
    #define PSI_FILE_SYSTEM_EXTENSION(module_id_)
#endif

/*---------------------------------------------------------------------------*/
/* Inline function implementation                                            */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure PNIO packages based on PSI base configuration                   */
/*---------------------------------------------------------------------------*/
#define PSI_CFG_MAX_READ_IMPL   (PSI_CFG_MAX_DIAG_TOOLS)

/*---------------------------------------------------------------------------*/
/* Configure Tracing                                                         */
/*---------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
// interface to LSA trace
//  0 .. no LSA trace
//  1 .. LSA trace [default]
//  2 .. LSA index trace
//----------------------------------------------------------------------------*/
#define PSI_CFG_TRACE_MODE  2

/*---------------------------------------------------------------------------*/
/* Configure Common                                                          */
/*---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure CM                                                              */
/*---------------------------------------------------------------------------*/
// Note: Must be done before configuring CLRPC since some of the defines from here are used later.
// IOC not used? -> Set values to 0
#if (PSI_CFG_USE_IOC == 0)
    #define PSI_CFG_MAX_CL_DEVICES       0
    #define PSI_CFG_MAX_CL_OSU_DEVICES   0
#endif

// IOM not used? -> Set values to 0
#if (PSI_CFG_USE_IOM == 0)
    #define PSI_CFG_MAX_MC_DEVICES       0
#endif

// IOD not used? -> Set values to 0
#if (PSI_CFG_USE_IOD == 0)
    #define PSI_CFG_MAX_SV_DEVICES       0
    #define PSI_CFG_MAX_SV_IO_ARS        0
#endif

/*---------------------------------------------------------------------------*/
/* PNIO configuration for PSI-LD (LD components)                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure CLRPC                                                           */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_CLRPC_CFG_USE_CASTS         1
#define PSI_CFG_CLRPC_CFG_MAX_CHANNELS      (3 * (PSI_CFG_MAX_IF_CNT)) // per NIC: 1(PATH_TYPE_EPM) + 2(PATH_TYPE_USER, CM and Easy-Supervisor)
#define PSI_CFG_CLRPC_CFG_MAX_CLIENTS       (((PSI_CFG_MAX_CL_DEVICES) + (PSI_CFG_MAX_SV_DEVICES)*(PSI_CFG_CM_CFG_MAX_SV_ARS)) * (PSI_CFG_MAX_IF_CNT)) /*AP01511246 This constant will be replaced by a runtime parameter in future*/
#define PSI_CFG_CLRPC_CFG_MAX_SERVERS       (1/*EPM*/ + (1/*CM_CL*/ + 1*(PSI_CFG_MAX_SV_DEVICES)/*CM-SV*/) * (PSI_CFG_MAX_IF_CNT)) /* 0 = no server-part */
#define PSI_CFG_CLRPC_CFG_MAX_SCALLS        (LSA_MAX(((PSI_CFG_MAX_CL_DEVICES)+(PSI_CFG_MAX_READ_IMPL)),((PSI_CFG_MAX_SV_DEVICES)*((PSI_CFG_CM_CFG_MAX_SV_ARS) + (PSI_CFG_MAX_READ_IMPL))))) /*AP01511410 This constant will be replaced by a runtime parameter in future*/

#define PSI_CFG_CLRPC_CFG_MAX_READ_IMPLICIT_PER_OBJ_UUID    (PSI_CFG_MAX_READ_IMPL) /*AP01511234 This constant will be replaced by a runtime parameter in future*/
#define PSI_CFG_CLRPC_CFG_MAX_CONTEXTS                      (PSI_CFG_MAX_DIAG_TOOLS)

/*---------------------------------------------------------------------------*/
/* Configure DNS                                                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure OHA                                                             */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1) 
    #define PSI_CFG_OHA_CFG_MAX_CHANNELS    (1 /* Appl global */ + 1 /* GLO_SYS_SNMPX_OHA */ + (2 * (PSI_CFG_MAX_IF_CNT) /* IF User (Appl If, CM) */))
#else
    #define PSI_CFG_OHA_CFG_MAX_CHANNELS    (1 /* Appl global */ + (2 * (PSI_CFG_MAX_IF_CNT) /* IF User (Appl If, CM) */))
#endif
#define PSI_CFG_OHA_CFG_MAX_NICS            (PSI_CFG_MAX_IF_CNT)   /* Pnio If Cnt */
#define PSI_CFG_OHA_CFG_MAX_PORTS           (PSI_CFG_MAX_PORT_CNT) /* Max Ports/Interface */
#define PSI_CFG_OHA_MAX_DESCRIPTION_LEN     255                    /* Max len for oha description strings (system, interface and ports) from: LLDP_MAX_PORT_ID_STRING_SIZE: 255 */

/* The system capabilities contains the primary functions of the system (IEEE 802.1AB) */
#define PSI_CFG_OHA_LLDP_SYSTEM_CAPABILITIES    (LLDP_TLV_SYSCAB_STATION_ONLY+LLDP_TLV_SYSCAB_BRIDGE)

/* The enabled capabilities contains the currently enabled functions of the system capabilities (IEEE 802.1AB) */
#define PSI_CFG_OHA_LLDP_ENABLED_CAPABILITIES   (LLDP_TLV_SYSCAB_STATION_ONLY+LLDP_TLV_SYSCAB_BRIDGE)

#define PSI_CFG_HELLO_SERVER_RESOURCE_COUNT_DCP     ( LSA_MAX(((PSI_CFG_MAX_CL_OSU_DEVICES) * 3),16) )

#define PSI_CFG_OHA_CFG_ALLOW_UPPER_CASES   0 /* Only on classic controller/devices (not set on CPU15xxx, ET200SP Devices, ET200MP Devices */

/*---------------------------------------------------------------------------*/
/* Configure SNMPX                                                           */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_SNMPX_CFG_TRACE_RQB_ERRORS      0

#if (defined(PSI_CFG_MAX_SNMPX_MGR_SESSIONS) && (PSI_CFG_MAX_SNMPX_MGR_SESSIONS > 0))
    #define PSI_CFG_USE_SNMPX_MGR           1
    #define PSI_CFG_SNMPX_CFG_SNMP_MANAGER
    #define PSI_CFG_SNMPX_CFG_MAX_MANAGER_SESSIONS  (PSI_CFG_MAX_SNMPX_MGR_SESSIONS )
#endif

/*---------------------------------------------------------------------------*/
/* Configure  SOCKAPP                                                        */
/*---------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_SOCKAPP == 1 )
#define PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS   4
#else
#define PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS   0
#endif

/*---------------------------------------------------------------------------*/
/* Configure SOCK                                                            */
/*---------------------------------------------------------------------------*/

#if (!defined (PSI_CFG_MAX_SOCK_APP_CHANNELS)) || (PSI_CFG_MAX_SOCK_APP_CHANNELS < 1) || (PSI_CFG_MAX_SOCK_APP_CHANNELS > 8)
    #error "PSI_CFG_MAX_SOCK_APP_CHANNELS not defined or out of range (1..8)"
#endif

#if (!defined (PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS)) || !((PSI_CFG_MAX_SOCK_APP_CHANNELS >= 0) && (PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS <= 4))
#error "PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS not defined or out of range (0..4)"
#endif

#define PSI_CFG_SOCK_CFG_MAX_CHANNELS                   (3 + (PSI_CFG_MAX_SOCK_APP_CHANNELS) + (PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS) ) /* global CLRPC + SNMPX + OHA + SOCK App Users + SOCKApp Channels */ 
#define PSI_CFG_SOCK_CFG_MAX_INTERFACE_CNT              (PSI_CFG_MAX_IF_CNT) /* same as EDD_CFG_MAX_INTERFACE_CNT */

/*----------------------------------------------------------*/
/* calculation of PSI_CFG_MAX_SOCKETS                       */
/* same as TCIP_CFG_MAX_SOCKETS                             */
/*----------------------------------------------------------*/
// Calculate the PSI_CFG_MAX_SOCKETS value by calculating the helper values PSI_MAX_SOCKETS_PCIOX_STATIC and PSI_MAX_SOCKETS_IOC
#define PSI_MAX_SOCKETS_PCIOX_STATIC    ( 1 /*EPM*/ + 1 /*SNMP (UDP)*/ + 1 /*WebSrv Listening*/ + 1 /*WebSrv Worker*/ + 1 /*ScriptSrv Listening*/ + 1 /*ScriptSrv Worker*/ + 1 /*ScriptSrv Client*/ )

#ifdef PSI_CFG_USE_IOC
    #define PSI_MAX_SOCKETS_IOC     1
#else
    #define PSI_MAX_SOCKETS_IOC     0
#endif

#define PSI_CFG_MAX_SOCKETS     ( (PSI_MAX_SOCKETS_PCIOX_STATIC) + (PSI_CFG_MAX_SV_DEVICES)*(PSI_CFG_MAX_IF_CNT) +  (PSI_MAX_SOCKETS_IOC)*(PSI_CFG_MAX_IF_CNT) )

/*---------------------------------------------------------------------------*/
/* Configure TCIP                                                            */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_TCIP == 1)

#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
    /* OpenBSD Page Memory: 500 pages with 4k size */
    #define PSI_CFG_TCIP_CFG_OBSD_PAGE_SIZE         4096
    #define PSI_CFG_TCIP_CFG_OBSD_NUMBER_OF_PAGES   500
    #define PSI_CFG_SOCK_CFG_ENABLE_MULTICASTS  1
    #define PSI_CFG_TCIP_CFG_ENABLE_MULTICAST   1
#endif

#if (PSI_CFG_TCIP_STACK_INTERNICHE == 1)
    #define PSI_CFG_SOCK_CFG_ENABLE_MULTICASTS  1
    #define PSI_CFG_TCIP_CFG_ENABLE_MULTICAST   1

    /* The include of this header file is necessary because the Interniche stack includes stdint.h and other files. 
    The stdint.h and other headers are included here, that way the reinclude protection of the headers saves the day later. 
    Do not remove this unless you know what you are doing. */
    //lint --e(537) eps_lib.h may also be included in eps_sys.h 
    #include <eps_lib.h>              // Std Libs (target / OS specific)
#endif

#if (PSI_CFG_TCIP_STACK_WINSOCK == 1)
    #define PSI_CFG_SOCK_CFG_ENABLE_MULTICASTS  0
    #define PSI_CFG_TCIP_CFG_ENABLE_MULTICAST   0
#endif // #if (PSI_CFG_TCIP_STACK_WINSOCK == 1)

#if (PSI_CFG_TCIP_STACK_CUSTOM == 1)
    // #define PSI_CFG_SOCK_CFG_ENABLE_MULTICASTS  ?
    #define PSI_CFG_TCIP_CFG_ENABLE_MULTICAST   0
#endif // #if (PSI_CFG_TCIP_STACK_CUSTOM == 1)

#endif // #if (PSI_CFG_USE_TCIP == 1)

#define PSI_CFG_TCIP_CFG_SOMAXCONN              10 /* maximum length of the TCP backlog (1-127, 0 defaults to 5) */
#define PSI_CFG_TCIP_CFG_SYS_SERVICES           0x4C /* 0x4C == 0b01001100 == Layer 7+4+3, see RFC3418, sysServices */ 

/* Used for TCIP_CFG_MAXARPS */
#define PSI_CFG_MAX_SV_IOC_ARS              9 

/*-------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* PNIO configuration for PSI-HD (HD components)                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure ACP                                                             */
/*---------------------------------------------------------------------------*/

/* PSI_CFG_MAX_CM_CL_CONNECTS_PER_SECOND = Count of devices CM-CL can handle in parallel (pDetail->u.cl.max_connects_per_second an associated) */
/* PSI_CFG_MAX_CM_CL_CONNECTS_PER_SECOND is limited to 50 by system technic */

#if (PSI_CFG_MAX_CL_DEVICES > 50) 
	#define PSI_CFG_MAX_CM_CL_CONNECTS_PER_SECOND   50
#else 
	#define PSI_CFG_MAX_CM_CL_CONNECTS_PER_SECOND   (PSI_CFG_MAX_CL_DEVICES)
#endif

// IOC
#define PSI_CFG_CM_CFG_MAX_CL_FSU_DEVICES       (PSI_CFG_MAX_CL_OSU_DEVICES)

// IOD
#define PSI_CFG_CM_CFG_MAX_SV_ARS       127  /* Fixed to max value as this doesn't cost much memory */ /*AP01511178 This constant will be replaced by a runtime parameter in future*/ 

/*---------------------------------------------------------------------------*/
/* Configure DCP                                                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure EDDx (for all)                                                  */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_EDD_CFG_MAX_PORT_CNT                    (PSI_CFG_MAX_PORT_CNT)
#define PSI_CFG_EDD_CFG_MAX_INTERFACE_CNT               (PSI_CFG_MAX_IF_CNT)
#define PSI_CFG_EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE      ((PSI_CFG_MAX_PORT_CNT) * (PSI_CFG_MAX_IF_CNT))
#define PSI_CFG_EDD_CFG_MAX_MRP_INSTANCE_CNT            2
#define PSI_CFG_EDD_CFG_MAX_MRP_IN_INSTANCE_CNT         1

#if (((PSI_CFG_MAX_SV_DEVICES) + (PSI_CFG_MAX_CL_DEVICES) + (PSI_CFG_MAX_MC_DEVICES)) < 64) /* EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT */
	#define PSI_CFG_MAX_CRT_FRAMES_PER_CYCLE    ((PSI_CFG_MAX_SV_DEVICES) + (PSI_CFG_MAX_CL_DEVICES) + (PSI_CFG_MAX_MC_DEVICES))
#else
	#define PSI_CFG_MAX_CRT_FRAMES_PER_CYCLE    64 /* EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT */
#endif

/*---------------------------------------------------------------------------*/
/* Configure EDDI                                                            */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_EDDI == 1)

/* Event mode - Interrupt or polling */
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)        // Light -> Polling Mode - Polling-Cycle ~1ms
    #define PSI_CFG_EDDI_CFG_SII_POLLING_MODE
#elif (EPS_PLF == EPS_PLF_ADONIS_X86)       // Advanced -> ExtTimerMode -> but without Interrupts
    #define PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON       // this settings are identical to PLC 1517 - 2016-04-21    
    #define PSI_CFG_EDDI_CFG_SII_INT_RESTART_MODE_LOOP
#elif ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000))    // Linux -> Polling Mode - Polling-Cycle ~1ms
    #define PSI_CFG_EDDI_CFG_SII_POLLING_MODE        
#elif (EPS_PLF == EPS_PLF_LINUX_SOC1)       // SOC1 Linux -> ExtTimer + Interrupt
    #define PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON
    #define PSI_CFG_EDDI_CFG_SII_INT_RESTART_MODE_LOOP     
#elif (EPS_PLF == EPS_PLF_SOC_MIPS)         // BASIC MIPS (Soc) - ExtTimer + Interrupt
    #define PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON   // this settings are identical to PLC 1516/1518 - 2016-04-21
    #define PSI_CFG_EDDI_CFG_SII_INT_RESTART_MODE_LOOP
#elif (EPS_PLF == EPS_PLF_PNIP_ARM9)        // BASIC PNIP (EB200P) - EDDP!
    // -> EDDP
#elif (EPS_PLF == EPS_PLF_PNIP_A53)        // BASIC PNIP (HERA) - EDDP!
    // -> EDDP
#elif (EPS_PLF == EPS_PLF_ERTEC200_ARM9)    // BASIC ERTEC200 - Polling
    #define PSI_CFG_EDDI_CFG_SII_POLLING_MODE
#elif (EPS_PLF == EPS_PLF_ERTEC400_ARM9)    // BASIC ERTEC400 - Polling
    #define PSI_CFG_EDDI_CFG_SII_POLLING_MODE
#endif

// When external LED functions are used, for EDDI the EDDI_CFG_PHY_BLINK_EXTERNAL_NSC define has to be set
#if defined EPS_CFG_USE_EXTERNAL_LED
#define EDDI_CFG_PHY_BLINK_EXTERNAL_NSC
#endif

/*---------------------------------------------------------------------------*/
/* Max number of providers per interface supported.                          */
/* (1..0x7FFF)                                                               */
/*---------------------------------------------------------------------------*/
#define PSI_CFG_EDDI_CFG_MAX_NR_PROVIDERS       ((PSI_CFG_MAX_CL_DEVICES) + (PSI_CFG_MAX_MC_DEVICES) + (PSI_CFG_MAX_SV_DEVICES))

/* Supported PHYs*/
#ifdef PSI_CFG_EDDI_CFG_SOC
    // EPS variants support all PHYs that are supported in SOC1 boards
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NSC
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NEC
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_BROADCOM
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_TI
    #define PSI_CFG_EDDI_CFG_PHY_BLINK_EXTERNAL_NEC
#endif // #if (PSI_CFG_EDDI_CFG_SOC == 1)

#ifdef PSI_CFG_EDDI_CFG_ERTEC_200
    // EPS variants support all PHYs that are supported in EB200PCIe and EB200PCI boards    
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NSC
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NEC
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_BROADCOM
    #define PSI_CFG_EDDI_CFG_PHY_BLINK_EXTERNAL_NEC
#endif // #if (PSI_CFG_EDDI_CFG_ERTEC_200 == 1)

#ifdef PSI_CFG_EDDI_CFG_ERTEC_400
    // EPS variants support all PHYs that are supported in ERTEC400 PCIe and CP1616 boards
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NSC           /* NSC Transceiver (PHY)                */
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NEC           /* BROADCOM (BCM5221) Transceiver (PHY) */
    #define PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_BROADCOM      /* NEC (AATHPHYC2) Transceiver (PHY)    */
#endif // #if (PSI_CFG_EDDI_CFG_ERTEC_400 == 1)

#endif // #if (PSI_CFG_USE_EDDI == 1)

/*---------------------------------------------------------------------------*/
/* Configure EDDP                                                            */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_EDDP == 1)

#endif // #if (PSI_CFG_USE_EDDP == 1)

/*---------------------------------------------------------------------------*/
/* Configure EDDS                                                            */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_EDDS == 1)

#endif // #if (PSI_CFG_USE_EDDS == 1)

/*---------------------------------------------------------------------------*/
/* Configure EDDT                                                            */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_EDDT == 1)

#endif // #if (PSI_CFG_USE_EDDT == 1)

/*---------------------------------------------------------------------------*/
/* Configure GSY                                                             */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_GSY_CFG_CH_MAX_PORTS        (PSI_CFG_MAX_PORT_CNT)
#define PSI_CFG_GSY_CFG_MAX_SYS_CHANNELS    (PSI_CFG_MAX_IF_CNT)
#define PSI_CFG_GSY_CFG_MAX_USR_CHANNELS    (PSI_CFG_MAX_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure MRP                                                             */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_MRP_MAX_NO_OF_INTERFACES    (PSI_CFG_MAX_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure NARE                                                            */
/*---------------------------------------------------------------------------*/
#define PSI_CFG_NARE_CFG_TRACE_RQB_ERRORS   0
/*---------------------------------------------------------------------------*/
/* Configure POF                                                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure IOH                                                             */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure HERA IO module                                                  */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_HERA_IO_CFG_MAX_IF          (PSI_CFG_MAX_IF_CNT)

/*-------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Configure PNTRACE                                                         */
/*---------------------------------------------------------------------------*/

#define PNTRC_CFG_COMPILE_MODULE_ID     1
#define PNTRC_CFG_COMPILE_FILE         	1
#define PNTRC_CFG_COMPILE_LINE         	1
#define PNTRC_CFG_LEVEL_DEFAULT        	4
#define PNTRC_CFG_LOCAL_SYSTEM_ID      	1   /* PNTRC_SYSTEM_ID_APP */
#define PNTRC_CFG_SYNC_INTERVAL        	3000 /* 3000 ms Time Sync Interval to lower systems, timer is currently used for periodic trace buffer switching */
#define PNTRC_CFG_MAX_LOWER_CHANNELS    (PSI_CFG_MAX_IF_CNT)

/*---------------------------------------------------------------------------*/
/* Configure HIF                                                             */
/*---------------------------------------------------------------------------*/

#define PSI_CFG_HIF_CFG_MAX_LD_CH_HANDLES           100 // globale User + MX IF* IF Pipes
#define PSI_CFG_HIF_CFG_MAX_HD_CH_HANDLES           155 // Max IF * HD IF Pipes + HD Pipe

#define PSI_CFG_HIF_CFG_POLL_INTERVAL               1  // Set to 1 in PSI, timer base is 100ms base. Results in a timer with 100ms base.

#define PSI_CFG_HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS  0 // Don't use app specific HD Sock channels

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
    //#define PSI_CFG_HIF_DEBUG_MEASURE_PERFORMANCE // set during performance measurement if needed
#else
    #define PSI_CFG_HIF_CFG_ALIGNMENT_SAVE_COPY_FUNCTION // attention: for arm platforms (ertec200p, ertec200, ertec400) this define has to be set! 32 bit copy does not work for misaligned 8/16 bit enums!
#endif

/**
  * Enables the HIF Special Mode (HSM) to get serialization in Windows
  * when running on a single core.
  **/
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    /* Defines are inactive and set by developers to enable debugging for specific platforms.*/
    //#define HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
    //#define HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT
    //#define HIF_SHM_HSM_SIZE			0x400000
#endif

// Don't use DMA for hif if PSI_CFG_USE_HIF_DMA not defined.
#ifndef PSI_CFG_USE_HIF_DMA
#define PSI_CFG_USE_HIF_DMA     0
#endif

/*---------------------------------------------------------------------------*/
/* Configure HSA                                                             */
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PSI_CFG_H */
