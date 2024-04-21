#ifndef PND_PSI_CFG_PLF_LINUX_IOT2000_OBSD_H
#define PND_PSI_CFG_PLF_LINUX_IOT2000_OBSD_H

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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_psi_cfg_plf_linux_iot2000_obsd.h      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PSI EPS Platform defintions                                              */
/*                                                                           */
/*****************************************************************************/

/* included by "psi_cfg.h" */
#include "eps_plf_types.h"  /* for defines of EPS_PLATFORM and EPS_PLF */

//Use PnDevDrv
#define EPS_CFG_USE_PNDEVDRV

/*===========================================================================*/
/*        Platform settings                                                  */
/*===========================================================================*/
/* EPS_PLATFORM_PNDRIVER and EPS_PLF_LINUX_IOT2000 are defined in <eps_plf_types.h> */
#define EPS_PLATFORM    EPS_PLATFORM_PNDRIVER
#define EPS_PLF         EPS_PLF_LINUX_IOT2000

/* extra warnings for compile level 3 MsDev */
#if defined(_MSC_VER)
#pragma warning(3:4032) /* formal parameter 'number' has different type when promoted */
#pragma warning(3:4130) /* 'operator ' : logical operation on address of string constant */
#pragma warning(3:4131) /* 'function' : uses old-style declarator */

#pragma warning(3:4209) /* nonstandard extension used : benign typedef redefinition */
#pragma warning(3:4727) /* conditional expression is constant */

#pragma warning(3:4189) /* 'identifier' : local variable is initialized but not referenced */

#pragma warning(3:4100) /* Unreferenced formal parameter */
#pragma warning(3:4101) /* Unreferenced local variable */
#pragma warning(3:4121) /* structure is sensitive to alignment */
#pragma warning(3:4389) /* '!=' : signed/unsigned mismatch */
#pragma warning(3:4701) /* local may be used w/o init */
// #pragma warning(3:4702) /* Unreachable code */
#pragma warning(3:4705) /* Statement has no effect */

// #pragma warning(3:4242) /* 'identifier' : conversion from 'type1' to 'type2', possible loss of data */
// #pragma warning(3:4244) /* '=/+=' : conversion from 'int' to 'uint16_t', possible loss of data */
#pragma warning(3:4254) /* 'operator' : conversion from 'type1' to 'type2', possible loss of data */
#pragma warning(3:4289) /* nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope */
#pragma warning(3:4389) /* An operation involved signed and unsigned variables. This could result in a loss of data. */
#pragma warning(3:4431) /* missing type specifier - int assumed. Note: C no longer supports default-int */

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

#if defined(_DEBUG)
  #define PSI_DEBUG     1
#else
  #define PSI_DEBUG     0
#endif

/*---------------------------------------------------------------------------*/
/* Common defines valid for all targets                                      */
/*---------------------------------------------------------------------------*/


// suppress printf
//#define EPS_SUPPRESS_PRINTF

// Do not use HIF serialisation
#define EPS_LOCAL_PN_USER

//Enable NEA for EDDS
#define PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE 

#define PSI_CFG_MAX_HD_CNT       1 /* Max number of HD(HardwareDevices) [1..N] for one LD instance */
#define PSI_CFG_MAX_HD_IF_CNT    1 /* Max number of IF(Interfaces) [1..4] for one HD instance      */
#define PSI_CFG_MAX_PNIO_IF_CNT  1 /* Maximum number of PNIO interfaces of all EDDs integrated     */
#define PSI_CFG_MAX_PORT_CNT     1 /* Maximum number of ports supported by EDD API services        */
	
#define PSI_TCIP_CFG_COPY_ON_SEND   1 //Using COPY IF for TCIP

/* Enable the automatic filling up to 3 bytes with 0 in NRT-Send-Buffer by EDDI     */
/* Switching on is only allowed, if PSI_TCIP_CFG_COPY_ON_SEND is defined!           */
#define PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT

#define PSI_CFG_MAX_SOCK_APP_CHANNELS           1 /* Maximum number of user application sock channels, Values (1..8) */ // TODO: Setting 0 generates compile error!
#define PSI_CFG_MAX_DIAG_TOOLS                  8 /* Maximum number of diagnosis tools used in the system */
#define PSI_CFG_SOCK_MULTICAST_SUPPORT          0 /* No Multicasts */

#define PSI_CFG_CM_CFG_SIMATIC_B000             0
#define PSI_CFG_CM_CFG_CL_ALLOW_DIAG_AND_PDEV   0

// Compile Keys for CM/SNMPX role specific features
#define PSI_CFG_USE_IOC        1
#define PSI_CFG_USE_IOM        0
#define PSI_CFG_USE_IOD        0
#define PSI_CFG_USE_IOD_MC     0     // don't activiate, feature not release
#define PSI_CFG_USE_SNMPX_AGT  0     // don't activiate, feature not release

#define PSI_CFG_MAX_SNMPX_MGR_SESSIONS 32

// Compile Keys PNIO component role and specific
// Note: 0 is off, 1 used on HOST, 2 used on embedded Testsystem

#define PSI_CFG_USE_DNS     0
#define PSI_CFG_USE_EDDI    0
#define PSI_CFG_USE_EDDP    0
#define PSI_CFG_USE_EDDS    1
#define PSI_CFG_USE_EDDT    0  
#define PSI_CFG_USE_GSY     0
#define PSI_CFG_USE_HIF     1
#define PSI_CFG_USE_HIF_HD  1
#define PSI_CFG_USE_HIF_LD  1
#define PSI_CFG_USE_LD_COMP 1   
#define PSI_CFG_USE_HD_COMP 1
#define PSI_CFG_USE_HSA     0
#define PSI_CFG_USE_IOH     0
#define PSI_CFG_USE_MRP     0
#define PSI_CFG_USE_POF     0
#define PSI_CFG_USE_PNTRC   1
#define PSI_CFG_USE_SOCKAPP 0 
#define PSI_CFG_USE_TCIP    1  // Interniche IP-Stack

#define PSI_CFG_USE_TCIP_OBSD  1

#define PSI_ALLOW_NONE_PNIO_TYPES  1  // Allow 10MB Mau-Types

#define PSI_MAX_CL_DEVICES     64       // Max devices for create client
#define PSI_MAX_CL_IRT_DEVICES 0         // Max IRT devices
#define PSI_MAX_CL_DEVICE_NR   2048      // Max devic Nr [1..2048] for the client
#define PSI_MAX_CL_RECORD_SIZE (32*1024) // Max Record size = 32K
#define PSI_MAX_CL_READ_IMPL   16
#define PSI_MAX_CL_OSU_DEVICES 8

#define PSI_MAX_MC_DEVICES   0
#define PSI_MAX_MC_DEVICE_NR 0

#define PSI_MAX_SV_DEVICES     0
#define PSI_MAX_SV_READ_IMPL   0
#define PSI_MAX_SV_AR          0
#define PSI_MAX_SV_RECORD_SIZE 0


#if ( PSI_CFG_USE_HIF == 0 )
#undef  PSI_CFG_USE_HIF_HD
#define PSI_CFG_USE_HIF_HD 0
#endif

/*============================================================================
 *       Endianess:
 * If you have a computer system whitch stores the most significant byte at the
 * lowest address of the word/doubleword: Define this
 * #define LSA_HOST_ENDIANESS_BIG
 * else define this
 * #define LSA_HOST_ENDIANESS_LITTLE
 *===========================================================================*/

#undef  LSA_HOST_ENDIANESS_BIG
#define LSA_HOST_ENDIANESS_LITTLE

/*------------------------------------------------------------------------------
// interface to LSA trace
//  0 .. no LSA trace
//  1 .. LSA trace [default]
//  2 .. LSA index trace
//----------------------------------------------------------------------------*/
#define PSI_CFG_TRACE_MODE  2

#define EPS_CFG_USE_EDDI    (PSI_CFG_USE_EDDI && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDP    (PSI_CFG_USE_EDDP && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDS    (PSI_CFG_USE_EDDS && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_EDDT    (PSI_CFG_USE_EDDT && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_GSY     (PSI_CFG_USE_GSY && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_LLDP    PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_MRP     (PSI_CFG_USE_MRP && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_DNS     (PSI_CFG_USE_DNS && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_SNMPX   PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_SOCK    PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_TCIP    (PSI_CFG_USE_TCIP && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_CLRPC   PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_OHA     PSI_CFG_USE_LD_COMP
#define EPS_CFG_USE_ACP     PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_CM      PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_DCP     PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_NARE    PSI_CFG_USE_HD_COMP
#define EPS_CFG_USE_HIF     PSI_CFG_USE_HIF 
#define EPS_CFG_USE_HIF_LD  PSI_CFG_USE_HIF_LD
#define EPS_CFG_USE_HIF_HD  PSI_CFG_USE_HIF_HD
#define EPS_CFG_USE_PSI     LSA_TRUE            //EPS uses PSI (Starts PSI task in eps_task.c)
#define EPS_CFG_USE_POF     (PSI_CFG_USE_POF && PSI_CFG_USE_HD_COMP)
#define EPS_CFG_USE_HSA     PSI_CFG_USE_HSA
#define EPS_CFG_USE_IOH     (PSI_CFG_USE_IOH && PSI_CFG_USE_LD_COMP)
#define EPS_CFG_USE_SOCKAPP (PSI_CFG_USE_SOCKAPP && PSI_CFG_USE_LD_COMP) 
#define EPS_CFG_USE_EPS     LSA_TRUE            //EPS uses EPS (Starts EPS task in eps_task.c)
#define EPS_CFG_USE_PNTRC	PSI_CFG_USE_PNTRC


#define PSI_CFG_HIF_CFG_COMPILE_SERIALIZATION   0
#define PSI_CFG_MAX_IF_CNT                      PSI_CFG_MAX_PNIO_IF_CNT
#define PSI_CFG_TCIP_STACK_OPEN_BSD             1
#define PSI_CFG_TCIP_CFG_COPY_ON_SEND           (PSI_TCIP_CFG_COPY_ON_SEND)
#define PSI_CFG_MAX_CL_DEVICES                  PSI_MAX_CL_DEVICES
#define PSI_CFG_MAX_CL_OSU_DEVICES              PSI_MAX_CL_OSU_DEVICES
#define PSI_CFG_HIF_CFG_MAX_LD_INSTANCES        2
#define PSI_CFG_HIF_CFG_MAX_HD_INSTANCES        (PSI_CFG_MAX_HD_CNT*2)
#define PSI_CFG_MAX_EDDS_DEVICES                4 // Maximum number of devices that the EDDS supports

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_PSI_CFG_PLF_LINUX_IOT2000_OBSD_H */
