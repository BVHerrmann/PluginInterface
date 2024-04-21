#ifndef OHA_CFG_H                        /* ----- reinclude-protection ----- */
#define OHA_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: oha_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for OHA:                                                   */
/*  Defines constants, types and macros for OHA.                             */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

void oha_psi_startstop( int start );

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/*             Components EDD, DCP, NARE, LLDP, MRP, SOCK and DNS            */
/*                        OHA_CFG_NO_NARE, OHA_CFG_NO_MRP,                   */
/*                        OHA_CFG_USE_DNS                                    */
/*---------------------------------------------------------------------------*/

/* Component NARE not used (no NARE-channel is openend): the request         */
/* OHA_OPC_RESOLVE_ADDRESS is therfore not supported !                       */
/*** #define OHA_CFG_NO_NARE  ***/
#if 0  // Nare is a mandatory component
#define OHA_CFG_NO_NARE // Turn off NARE
#endif

/* Component MRP not used (no MRP-channel is openend): the request           */
/* OHA_OPC_CONTROL_MRP_OID is therfore not supported !                       */
/*** #define OHA_CFG_NO_MRP  ***/
#if (PSI_CFG_USE_MRP == 0)  // not used
#define OHA_CFG_NO_MRP  // Turn off MRP
#endif

/* Component DNS shall be used (a DNS-channel is openend): the record        */
/* DNSParameters therfore is supported !                                     */
/*** #define OHA_CFG_USE_DNS	                                             */
#if (PSI_CFG_USE_DNS == 1)
#define OHA_CFG_USE_DNS
#else
#undef OHA_CFG_USE_DNS
#endif

/* OHA_SOCK_SNMP_OFF: No AutoActivate/AutoDeactivate of the OHA subagent in SOCK.*/
/* OHA activates its agent in SOCK by OhaSystemOpenChannel and deactivates   */
/* his agent by OhaSystemCloseChannel "automatically". There is no external  */
/* (user-) activation in SOCK necessary, neither an activation of the        */
/* internal agent nor an activation of the (external) OHA agent!             */
/* LSA_SOCK_P04.01.00.00_00.01.04.01 can do AutoActivation.                  */
#if (PSI_CFG_TCIP_STACK_OPEN_BSD == 1)
#define OHA_SOCK_SNMP_OFF /* no SNMP Agents */
#endif

/* The MRP-MIB (IEC-62439-2-MIB) is not supported.                           */
/*** #define OHA_CFG_NO_MRP_MIB	                                             */

/*---------------------------------------------------------------------------*/
/*                         OHA_CFG_NO_REMA                                   */
/*---------------------------------------------------------------------------*/
/* The PDEV parameterization must also be stored in "remote" parameterization*/
/* and it should be possible to load it during the next restart.             */
/* For this purpose, the Remanence Manager (REMA) is required.               */
/* The use of REMA in OHA can be turned off.                                 */
/*** #define OHA_CFG_NO_REMA  ***/

/*---------------------------------------------------------------------------*/
/* Define if RQB-Priority support should be included.                        */
/* Don't define it not supported by PNIO stack yet                           */
/*---------------------------------------------------------------------------*/

#undef OHA_CFG_USE_RQB_PRIORITY

/*---------------------------------------------------------------------------*/
/* Maximum number of user-channels (max. 128, because of handle-management)  */
/*---------------------------------------------------------------------------*/

#define OHA_CFG_MAX_CHANNELS        (PSI_CFG_OHA_CFG_MAX_CHANNELS)

/*---------------------------------------------------------------------------*/
/* Maximum number of NIC's (max. 32, because of handle-management)           */
/*---------------------------------------------------------------------------*/

#define OHA_CFG_MAX_NICS      		(PSI_CFG_OHA_CFG_MAX_NICS)

/*---------------------------------------------------------------------------*/
/* Maximum number of Ports of a Channel                                      */
/* must be >= MaxPortCnt                                                     */
/*---------------------------------------------------------------------------*/
#define OHA_CFG_MAX_PORTS     		(PSI_CFG_OHA_CFG_MAX_PORTS)

/*---------------------------------------------------------------------------*/
/* Configure validity of NameOfStation and PortID.                           */
/*                                                                           */
/* OHA_CFG_ALLOW_UPPER_CASES                                                 */
/*                                                                           */
/*    defined: Upper cases are converted to lower cases before the           */
/*             NameOfStation or the PortID is checked for PNIO standard      */
/*             conventions.                                                  */
/*    undef:   The NameOfStation or the PortID is strictly checked for       */
/*             PNIO standard conventions                                     */
/*                                                                           */
/*    Note:    If the NameOfStation or PortID is valid, the NameOfStation or */
/*             PortID is only stored with (converted) lower cases.           */
/*                                                                           */
/*             If the NameOfStation or PortID is invalid                     */
/*                a) a local Set or remote DCP-Set is rejected,              */
/*                b) a remote station is invalid.                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#if (!defined PSI_CFG_OHA_CFG_ALLOW_UPPER_CASES) || ((PSI_CFG_OHA_CFG_ALLOW_UPPER_CASES != 0) && (PSI_CFG_OHA_CFG_ALLOW_UPPER_CASES != 1))
#error PSI_CFG_OHA_ALLOW_UPPER_CASES not defined correctly
#endif

#if (PSI_CFG_OHA_CFG_ALLOW_UPPER_CASES == 1)
#define OHA_CFG_ALLOW_UPPER_CASES /* Only on classic controller/devices (not set on CPU15xxx, ET200SP Devices, ET200MP Devices */
#else
#undef OHA_CFG_ALLOW_UPPER_CASES
#endif

/*---------------------------------------------------------------------------*/
/* Additional hello delay by LinkUp in 10ms (20ms default)                   */
/*---------------------------------------------------------------------------*/

/* OHA does a HelloDelay(LinkUp) := "AddDelay" + HelloDelay from the record */

#define OHA_ADDITIONAL_HELLO_DELAY_BY_LINK_UP   2 /* 20ms fixed (AP00511181) */

/*===========================================================================*/
/*                         Endianess                                         */
/*===========================================================================*/

#ifdef LSA_HOST_ENDIANESS_BIG 
    #define OHA_BIG_ENDIAN
#else
    #undef OHA_BIG_ENDIAN
#endif

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Definition of RQB-HEADER:                                                 */
/*                                                                           */
/* The order of the parameters can be changed!                               */
/* The length of the RQB-HEADER must be 2/4-byte-aligned according to the    */
/* system!                                                                   */
/* If necessary, dummy-bytes must be added!                                  */
/* If necessary during system integration, additional parameters can be added*/
/*                                                                           */
/* For example:                                                              */
/*   LSA_COMP_ID_TYPE                comp_id; // for lsa-component-id        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define OHA_RQB_HEADER  LSA_RQB_HEADER(OHA_UPPER_RQB_PTR_TYPE)


/*---------------------------------------------------------------------------*/
/* Definition of RQB-TRAILER                                                 */
/*                                                                           */
/* If necessary during system integration, additional parameters can be      */
/* added!                                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define OHA_RQB_TRAILER
/* nothing by default */


/*---------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define OHA_RQB_SET_NEXT_RQB_PTR      LSA_RQB_SET_NEXT_RQB_PTR
#define OHA_RQB_GET_NEXT_RQB_PTR      LSA_RQB_GET_NEXT_RQB_PTR
#define OHA_RQB_SET_PREV_RQB_PTR      LSA_RQB_SET_PREV_RQB_PTR
#define OHA_RQB_GET_PREV_RQB_PTR      LSA_RQB_GET_PREV_RQB_PTR
#define OHA_RQB_SET_OPCODE            LSA_RQB_SET_OPCODE
#define OHA_RQB_GET_OPCODE            LSA_RQB_GET_OPCODE
#define OHA_RQB_SET_HANDLE            LSA_RQB_SET_HANDLE
#define OHA_RQB_GET_HANDLE            LSA_RQB_GET_HANDLE
#define OHA_RQB_SET_USERID_UVAR32     LSA_RQB_SET_USER_ID_UVAR32
#define OHA_RQB_GET_USERID_UVAR32     LSA_RQB_GET_USER_ID_UVAR32
#define OHA_RQB_SET_USERID_UVAR16_1   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW
#define OHA_RQB_GET_USERID_UVAR16_1   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW
#define OHA_RQB_SET_USERID_UVAR16_2   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH
#define OHA_RQB_GET_USERID_UVAR16_2   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH
#define OHA_RQB_SET_RESPONSE          LSA_RQB_SET_RESPONSE
#define OHA_RQB_GET_RESPONSE          LSA_RQB_GET_RESPONSE
#define OHA_RQB_SET_USER_ID_PTR       LSA_RQB_SET_USER_ID_PTR
#define OHA_RQB_GET_USER_ID_PTR       LSA_RQB_GET_USER_ID_PTR

#ifdef OHA_CFG_USE_RQB_PRIORITY
#define OHA_RQB_SET_PRIORITY          LSA_RQB_SET_PRIORITY
#define OHA_RQB_GET_PRIORITY          LSA_RQB_GET_PRIORITY
#endif
/*===========================================================================*/
/* LOWER LAYER RQB MACROS to EDD                                             */
/*===========================================================================*/

#define OHA_EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)   EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)   EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_EDD_RQB_SET_OPCODE(rb_, val_)         EDD_RQB_SET_OPCODE(rb_, val_)
#define OHA_EDD_RQB_SET_SERVICE(rb_, val_)        EDD_RQB_SET_SERVICE(rb_, val_)
#define OHA_EDD_RQB_SET_HANDLE(rb_, val_)         EDD_RQB_SET_HANDLE(rb_, val_)
#define OHA_EDD_RQB_SET_USERID_UVAR16(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_EDD_RQB_SET_USERID_UVAR32(rb_, val_)  EDD_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_EDD_RQB_SET_RESPONSE(rb_, val_)       EDD_RQB_SET_STATUS(rb_, val_)

#define OHA_EDD_RQB_GET_NEXT_RQB_PTR(rb_)         EDD_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_EDD_RQB_GET_PREV_RQB_PTR(rb_)         EDD_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_EDD_RQB_GET_OPCODE(rb_)               EDD_RQB_GET_OPCODE(rb_)
#define OHA_EDD_RQB_GET_SERVICE(rb_)              EDD_RQB_GET_SERVICE(rb_)
#define OHA_EDD_RQB_GET_HANDLE(rb_)               EDD_RQB_GET_HANDLE(rb_)
#define OHA_EDD_RQB_GET_USERID_UVAR16(rb_)        LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_EDD_RQB_GET_USERID_UVAR32(rb_)        EDD_RQB_GET_USERID_UVAR32(rb_)
#define OHA_EDD_RQB_GET_RESPONSE(rb_)             EDD_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to DCP                                             */
/*===========================================================================*/

#define OHA_DCP_RQB_SET_NEXT_RQB_PTR(rb_, val_)   DCP_UPPER_RQB_NEXT_SET(rb_, val_)
#define OHA_DCP_RQB_SET_PREV_RQB_PTR(rb_, val_)   DCP_UPPER_RQB_PREV_SET(rb_, val_)
#define OHA_DCP_RQB_SET_OPCODE(rb_, val_)         DCP_UPPER_RQB_OPCODE_SET(rb_, val_)
#define OHA_DCP_RQB_SET_HANDLE(rb_, val_)         DCP_UPPER_RQB_HANDLE_SET(rb_, val_)
#define OHA_DCP_RQB_SET_USERID_UVAR16(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_DCP_RQB_SET_USERID_UVAR32(rb_, val_)  DCP_UPPER_RQB_USER32_SET(rb_, val_)
#define OHA_DCP_RQB_SET_RESPONSE(rb_, val_)       DCP_UPPER_RQB_RESPONSE_SET(rb_, val_)

#define OHA_DCP_RQB_GET_NEXT_RQB_PTR(rb_)         DCP_UPPER_RQB_NEXT_GET(rb_)
#define OHA_DCP_RQB_GET_PREV_RQB_PTR(rb_)         DCP_UPPER_RQB_PREV_GET(rb_)
#define OHA_DCP_RQB_GET_OPCODE(rb_)               DCP_UPPER_RQB_OPCODE_GET(rb_)
#define OHA_DCP_RQB_GET_HANDLE(rb_)               DCP_UPPER_RQB_HANDLE_GET(rb_)
#define OHA_DCP_RQB_GET_USERID_UVAR16(rb_)        LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_DCP_RQB_GET_USERID_UVAR32(rb_)        DCP_UPPER_RQB_USER32_GET(rb_)
#define OHA_DCP_RQB_GET_RESPONSE(rb_)             DCP_UPPER_RQB_RESPONSE_GET(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to LLDP                                            */
/*===========================================================================*/

#define OHA_LLDP_RQB_SET_NEXT_RQB_PTR(rb_, val_)   LLDP_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_LLDP_RQB_SET_PREV_RQB_PTR(rb_, val_)   LLDP_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_LLDP_RQB_SET_OPCODE(rb_, val_)         LLDP_RQB_SET_OPCODE(rb_, val_)
#define OHA_LLDP_RQB_SET_HANDLE(rb_, val_)         LLDP_RQB_SET_HANDLE(rb_, val_)
#define OHA_LLDP_RQB_SET_USERID_UVAR16(rb_, val_)  LLDP_RQB_SET_USERID_UVAR16_1(rb_, val_)
#define OHA_LLDP_RQB_SET_USERID_UVAR32(rb_, val_)  LLDP_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_LLDP_RQB_SET_RESPONSE(rb_, val_)       LLDP_RQB_SET_RESPONSE(rb_, val_)

#define OHA_LLDP_RQB_GET_NEXT_RQB_PTR(rb_)         LLDP_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_LLDP_RQB_GET_PREV_RQB_PTR(rb_)         LLDP_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_LLDP_RQB_GET_OPCODE(rb_)               LLDP_RQB_GET_OPCODE(rb_)
#define OHA_LLDP_RQB_GET_HANDLE(rb_)               LLDP_RQB_GET_HANDLE(rb_)
#define OHA_LLDP_RQB_GET_USERID_UVAR16(rb_)        LLDP_RQB_GET_USERID_UVAR16_1(rb_)
#define OHA_LLDP_RQB_GET_USERID_UVAR32(rb_)        LLDP_RQB_GET_USERID_UVAR32(rb_)
#define OHA_LLDP_RQB_GET_RESPONSE(rb_)             LLDP_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to NARE                                            */
/*===========================================================================*/

#define OHA_NARE_RQB_SET_NEXT_RQB_PTR(rb_, val_)   NARE_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_NARE_RQB_SET_PREV_RQB_PTR(rb_, val_)   NARE_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_NARE_RQB_SET_OPCODE(rb_, val_)         NARE_RQB_SET_OPCODE(rb_, val_)
#define OHA_NARE_RQB_SET_HANDLE(rb_, val_)         NARE_RQB_SET_HANDLE(rb_, val_)
#define OHA_NARE_RQB_SET_USERID_UVAR16(rb_, val_)  NARE_RQB_SET_USERID_UVAR16_1(rb_, val_)
#define OHA_NARE_RQB_SET_USERID_UVAR32(rb_, val_)  NARE_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_NARE_RQB_SET_RESPONSE(rb_, val_)       NARE_RQB_SET_RESPONSE(rb_, val_)

#ifdef OHA_CFG_USE_RQB_PRIORITY
#define OHA_NARE_RQB_SET_PRIORITY(rb_, val_)       NARE_RQB_SET_PRIORITY(rb_, val_)
#endif

#define OHA_NARE_RQB_GET_NEXT_RQB_PTR(rb_)         NARE_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_NARE_RQB_GET_PREV_RQB_PTR(rb_)         NARE_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_NARE_RQB_GET_OPCODE(rb_)               NARE_RQB_GET_OPCODE(rb_)
#define OHA_NARE_RQB_GET_HANDLE(rb_)               NARE_RQB_GET_HANDLE(rb_)
#define OHA_NARE_RQB_GET_USERID_UVAR16(rb_)        NARE_RQB_GET_USERID_UVAR16_1(rb_)
#define OHA_NARE_RQB_GET_USERID_UVAR32(rb_)        NARE_RQB_GET_USERID_UVAR32(rb_)
#define OHA_NARE_RQB_GET_RESPONSE(rb_)             NARE_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to MRP                                             */
/*===========================================================================*/

#define OHA_MRP_RQB_SET_NEXT_RQB_PTR(rb_, val_)   MRP_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_MRP_RQB_SET_PREV_RQB_PTR(rb_, val_)   MRP_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_MRP_RQB_SET_OPCODE(rb_, val_)         MRP_RQB_SET_OPCODE(rb_, val_)
#define OHA_MRP_RQB_SET_HANDLE(rb_, val_)         MRP_RQB_SET_HANDLE(rb_, val_)
#define OHA_MRP_RQB_SET_USERID_UVAR16(rb_, val_)  MRP_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_MRP_RQB_SET_RESPONSE(rb_, val_)       MRP_RQB_SET_RESPONSE(rb_, val_)

#define OHA_MRP_RQB_GET_NEXT_RQB_PTR(rb_)         MRP_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_MRP_RQB_GET_PREV_RQB_PTR(rb_)         MRP_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_MRP_RQB_GET_OPCODE(rb_)               MRP_RQB_GET_OPCODE(rb_)
#define OHA_MRP_RQB_GET_HANDLE(rb_)               MRP_RQB_GET_HANDLE(rb_)
#define OHA_MRP_RQB_GET_USERID_UVAR16(rb_)        MRP_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_MRP_RQB_GET_RESPONSE(rb_)             MRP_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to SOCK                                            */
/*===========================================================================*/

#define OHA_SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)   SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)   SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_SOCK_RQB_SET_OPCODE(rb_, val_)         SOCK_RQB_SET_OPCODE(rb_, val_)
#define OHA_SOCK_RQB_SET_HANDLE(rb_, val_)         SOCK_RQB_SET_HANDLE(rb_, val_)
#define OHA_SOCK_RQB_SET_USERID_UVAR16(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_SOCK_RQB_SET_USERID_UVAR32(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR32(rb_, val_)
#define OHA_SOCK_RQB_SET_RESPONSE(rb_, val_)       SOCK_RQB_SET_RESPONSE(rb_, val_)

#define OHA_SOCK_RQB_GET_NEXT_RQB_PTR(rb_)         SOCK_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_SOCK_RQB_GET_PREV_RQB_PTR(rb_)         SOCK_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_SOCK_RQB_GET_OPCODE(rb_)               SOCK_RQB_GET_OPCODE(rb_)
#define OHA_SOCK_RQB_GET_HANDLE(rb_)               SOCK_RQB_GET_HANDLE(rb_)
#define OHA_SOCK_RQB_GET_USERID_UVAR16(rb_)        LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_SOCK_RQB_GET_USERID_UVAR32(rb_)        LSA_RQB_GET_USER_ID_UVAR32(rb_)
#define OHA_SOCK_RQB_GET_RESPONSE(rb_)             SOCK_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to DNS                                           */
/*===========================================================================*/

#define OHA_DNS_RQB_SET_NEXT_RQB_PTR(rb_, val_)    DNS_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_DNS_RQB_SET_PREV_RQB_PTR(rb_, val_)    DNS_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_DNS_RQB_SET_OPCODE(rb_, val_)          DNS_RQB_SET_OPCODE(rb_, val_)
#define OHA_DNS_RQB_SET_HANDLE(rb_, val_)          DNS_RQB_SET_HANDLE(rb_, val_)
#define OHA_DNS_RQB_SET_USERID_UVAR16(rb_, val_)   LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_DNS_RQB_SET_USERID_UVAR32(rb_, val_)   LSA_RQB_SET_USER_ID_UVAR32(rb_, val_)
#define OHA_DNS_RQB_SET_RESPONSE(rb_, val_)        DNS_RQB_SET_RESPONSE(rb_, val_)

#define OHA_DNS_RQB_GET_NEXT_RQB_PTR(rb_)          DNS_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_DNS_RQB_GET_PREV_RQB_PTR(rb_)          DNS_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_DNS_RQB_GET_OPCODE(rb_)                DNS_RQB_GET_OPCODE(rb_)
#define OHA_DNS_RQB_GET_HANDLE(rb_)                DNS_RQB_GET_HANDLE(rb_)
#define OHA_DNS_RQB_GET_USERID_UVAR16(rb_)         LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_DNS_RQB_GET_USERID_UVAR32(rb_)         LSA_RQB_GET_USER_ID_UVAR32(rb_)
#define OHA_DNS_RQB_GET_RESPONSE(rb_)              DNS_RQB_GET_RESPONSE(rb_)


/*===========================================================================*/
/* LOWER LAYER RQB MACROS to SNMPX                                           */
/*===========================================================================*/

#define OHA_SNMPX_RQB_SET_NEXT_RQB_PTR(rb_, val_)  SNMPX_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_SNMPX_RQB_SET_PREV_RQB_PTR(rb_, val_)  SNMPX_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_SNMPX_RQB_SET_OPCODE(rb_, val_)        SNMPX_RQB_SET_OPCODE(rb_, val_)
#define OHA_SNMPX_RQB_SET_HANDLE(rb_, val_)        SNMPX_RQB_SET_HANDLE(rb_, val_)
#define OHA_SNMPX_RQB_SET_USERID_UVAR32(rb_, val_) SNMPX_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_SNMPX_RQB_SET_RESPONSE(rb_, val_)      SNMPX_RQB_SET_RESPONSE(rb_, val_)

#define OHA_SNMPX_RQB_GET_NEXT_RQB_PTR(rb_)        SNMPX_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_SNMPX_RQB_GET_PREV_RQB_PTR(rb_)        SNMPX_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_SNMPX_RQB_GET_OPCODE(rb_)              SNMPX_RQB_GET_OPCODE(rb_)
#define OHA_SNMPX_RQB_GET_HANDLE(rb_)              SNMPX_RQB_GET_HANDLE(rb_)
#define OHA_SNMPX_RQB_GET_USERID_UVAR32(rb_)       SNMPX_RQB_GET_USERID_UVAR32(rb_)
#define OHA_SNMPX_RQB_GET_RESPONSE(rb_)            SNMPX_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* BTRACE - Support                                                          */
/*===========================================================================*/

#ifndef OHA_FILE_SYSTEM_EXTENSION
#define OHA_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif

/*===========================================================================*/
/* LTRC - Support (LSA-Trace)                                                */
/*===========================================================================*/

/*------------------------------------------------------------------------------
* interface to LSA trace                                                     
* 0: no traces                                                               
* 1: enable LSA Traces                                                       
* 2: enable LSA Idx Traces                                                   
//----------------------------------------------------------------------------*/
#define OHA_CFG_TRACE_MODE          (PSI_CFG_TRACE_MODE)

/*=============================================================================
 * If the LSA component LTRC isn't used for trace in the target system, then
 * the OHA trace macros can be defined here. On default they are empty.
 *
 * See also files oha_trc.h and lsa_cfg.h/txt.
 *
 *===========================================================================*/

/*===========================================================================*/
/* Debugging (for developer use)                                             */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* If a a memory-free call error should result in an fatal-error call        */
/* with OHA_FATAL_ERR_MEM_FREE define OHA_CFG_FREE_ERROR_IS_FATAL. If not    */
/* memory-free errors will be ignored.                                       */
/*---------------------------------------------------------------------------*/

#define OHA_CFG_FREE_ERROR_IS_FATAL

/*---------------------------------------------------------------------------*/
/* define to enable spezial additonal assertion-checks (e.g. NULL-Ptr)       */
/* This assertion will cause an fatal-error.                                 */
/*---------------------------------------------------------------------------*/

#if PSI_DEBUG
#define OHA_CFG_DEBUG_ASSERT
#endif

/*----------------------------------------------------------------------------*/
/*  Assert                                                                    */
/*                                                                            */
/*  ==> MUST BE EMPTY in Release-Version!                                     */
/*                                                                            */
/*  check if condition holds or call fatal error otherwise (programming error)*/
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifdef OHA_CFG_DEBUG_ASSERT
#define OHA_ASSERT(Cond_)  \
{                                                         \
	if( ! (Cond_) )                                       \
	{                                                     \
		OHA_FatalError( OHA_FATAL_ERR_INCONSISTENZ,       \
			    	 	OHA_MODULE_ID,                    \
				     	__LINE__);                        \
	}                                                     \
}
#define OHA_ASSERT_NULL_PTR(pPtr) \
{                                                         \
	if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )          \
	{                                                     \
		OHA_FatalError( OHA_FATAL_ERR_NULL_PTR,           \
			    	 	OHA_MODULE_ID,                    \
				     	__LINE__);                        \
	}                                                     \
}

#define OHA_ASSERT_FALSE(Cond_)  \
{                                                         \
	if( ! (Cond_) )                                       \
	{                                                     \
		OHA_FatalError( OHA_FATAL_ERR_INCONSISTENZ,       \
			    	 	OHA_MODULE_ID,                    \
				     	__LINE__);                        \
	}                                                     \
}
#else
#define OHA_ASSERT(Condition) {}
#define OHA_ASSERT_NULL_PTR(pPtr) {}
#define OHA_ASSERT_FALSE(Condition) {}
#endif

/*===========================================================================*/
/*                          macros/function                                  */
/*===========================================================================*/

/*- alloc mem macros----------------------------------------------------------*/
#define OHA_ALLOC_UPPER_RQB_LOCAL(mem_ptr_ptr, length)                  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_RQB)
#define OHA_ALLOC_UPPER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)      PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_MEM)
#define OHA_DCP_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_DCP)
#define OHA_DCP_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_DCP)
#define OHA_LLDP_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_LLDP)
#define OHA_LLDP_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_LLDP)
#ifndef OHA_CFG_NO_NARE
#define OHA_NARE_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_NARE)
#define OHA_NARE_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_NARE)
#endif
#ifndef OHA_CFG_NO_MRP
#define OHA_MRP_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_MRP)
#define OHA_MRP_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_MRP)
#endif
#define OHA_EDD_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_EDD)
#define OHA_EDD_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_EDD)
#define OHA_SOCK_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SOCK)
#define OHA_SOCK_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SOCK)
#ifdef OHA_CFG_USE_DNS
#define OHA_DNS_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_DNS)
#define OHA_DNS_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_DNS)
#endif
#ifdef OHA_CFG_USE_SNMPX
#define OHA_SNMPX_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SNMPX)
#define OHA_SNMPX_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SNMPX)
#endif
#define OHA_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)                        PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM)


#define OHA_FREE_UPPER_RQB_LOCAL(ret_val_ptr, upper_rqb_ptr)            PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), 0, LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_RQB)
#define OHA_FREE_UPPER_MEM(ret_val_ptr, upper_mem_ptr, sys_ptr)         PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_UPPER_MEM)
#define OHA_DCP_FREE_LOWER_RQB(ret_val_ptr, upper_mem_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_DCP)
#define OHA_DCP_FREE_LOWER_MEM(ret_val_ptr, upper_mem_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_DCP)
#define OHA_LLDP_FREE_LOWER_RQB(ret_val_ptr, upper_mem_ptr, sys_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_LLDP)
#define OHA_LLDP_FREE_LOWER_MEM(ret_val_ptr, upper_mem_ptr, sys_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_LLDP)
#ifndef OHA_CFG_NO_NARE
#define OHA_NARE_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_NARE)
#define OHA_NARE_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_NARE)
#endif
#ifndef OHA_CFG_NO_MRP
#define OHA_MRP_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_MRP)
#define OHA_MRP_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_MRP)
#endif
#define OHA_EDD_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_EDD)
#define OHA_EDD_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_EDD)
#define OHA_SOCK_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SOCK)
#define OHA_SOCK_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SOCK)
#ifdef OHA_CFG_USE_DNS
#define OHA_DNS_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_DNS)
#define OHA_DNS_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_DNS)
#endif
#ifdef OHA_CFG_USE_SNMPX
#define OHA_SNMPX_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)   PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_RQB_SNMPX)
#define OHA_SNMPX_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)   PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_OHA, PSI_MTYPE_LOWER_MEM_SNMPX)
#endif
#define OHA_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)                  PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_OHA, PSI_MTYPE_LOCAL_MEM)

/*****************************************************************************/
/*  end of file OHA_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_CFG_H */
