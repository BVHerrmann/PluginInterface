#ifndef EDDI_CFG_H          //reinclude-protection
#define EDDI_CFG_H

#ifdef __cplusplus          //If C++ - compiler: Use C linkage
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
/*  F i l e               &F: eddi_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for EDDI:                                                  */
/*  Defines constants, types and macros for eddi.                            */
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
/*                                                                           */
/*****************************************************************************/


/*---------------------------------------------------------------------------*/

void eddi_psi_startstop (int start);

/* check if condition holds or call fatal (programming error) */
#if PSI_DEBUG
#define EDDI_ASSERT(cond_) { if (!(cond_)) { PSI_FATAL_COMP( LSA_COMP_ID_EDDI, EDDI_MODULE_ID,0 ) } }
#else
#define EDDI_ASSERT(cond_) { /* empty-block in release version */ }
#endif

#if (!defined (PSI_CFG_EDDI_CFG_ERTEC_400) && !defined(PSI_CFG_EDDI_CFG_ERTEC_200) && !defined(PSI_CFG_EDDI_CFG_SOC))
#error "config mismatch, no EDDI compile revision selected"
#endif

#ifdef PSI_CFG_EDDI_CFG_ERTEC_400
#define EDDI_CFG_ERTEC_400
#endif
#ifdef PSI_CFG_EDDI_CFG_ERTEC_200
#define EDDI_CFG_ERTEC_200
#endif
#ifdef PSI_CFG_EDDI_CFG_SOC
#define EDDI_CFG_SOC
#endif



/*---------------------------------------------------------------------------*/
/* If LSA_RESULT is not defined by LSA, we define it here.                   */
/*---------------------------------------------------------------------------*/

/* Note: LSA_RESULT has to be the same type as "response" within             */
/*       LSA-RQB-Header. This is currently LSA_UINT16!                       */

#ifndef LSA_RESULT
#define LSA_RESULT   LSA_UINT16
#endif

#if defined (TOOL_CHAIN_MS_WINDOWS)

// #pragma warning( disable : 4214 )  //Nicht dem Standard entsprechende Erweiterung
// #pragma warning( disable : 4505 )

#endif

//#define EDDI_CFG_MONITOR_MODE_INCLUDE

/*---------------------------------------------------------------------------*/
/* Include xRT over UDP-Support (cylcic/azyclic).                            */
/* If not defined, all xRT over UDP-Frames will be handled like standard IP  */
/* frames.                                                                   */
/*---------------------------------------------------------------------------*/

//#define EDDI_CFG_XRT_OVER_UDP_INCLUDE  not implemented yet - do not use!!

/*---------------------------------------------------------------------------*/
/* maximum number of devices supported                                       */
/* allowed: 1..max nr supported by LSA_HANDLE_TYPE                           */
/*---------------------------------------------------------------------------*/
#define EDDI_CFG_MAX_DEVICES            (PSI_CFG_MAX_EDDI_DEVICES)
/*---------------------------------------------------------------------------*/
/* maximum number of communication channels supported                        */
/* allowed: 1..64                                                            */
/*---------------------------------------------------------------------------*/
#define EDDI_CFG_MAX_CHANNELS           (17 * (PSI_CFG_MAX_EDDI_DEVICES))
/*---------------------------------------------------------------------------*/
/* Max number of providers per interface supported.                          */
/* (1..0x7FFF)                                                               */
/*---------------------------------------------------------------------------*/
#define EDDI_CFG_MAX_NR_PROVIDERS       (PSI_CFG_EDDI_CFG_MAX_NR_PROVIDERS)

/*---------------------------------------------------------------------------*/
/* OPTIONAL: disable checking of overlaps in KRAM                            */
/*---------------------------------------------------------------------------*/
//#define EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK

/*===========================================================================*/
/* Compiler/HW-dependend optimization settings                               */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* For functions called from scratch-pad code but located outside (and vice  */
/* versa) we need an _far attribute with the tasking-Compiler in order to get*/
/* the correct call - opcode (if not the code will be out of range when      */
/* linked!)                                                                  */
/*---------------------------------------------------------------------------*/

#define EDDI_FAR_FCT

#define EDDI_SYS_HANDLE   LSA_SYS_PTR_TYPE // usage of SYS_PTR

/*---------------------------------------------------------------------------*/
/* if EDDI_INLINE is defined, several functions, including bitfield mani-    */
/* pulation, are inlined, resulting in higher speed, but more code           */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/
/*lint -e950     Note 950: Non-ANSI reserved word or construct: '__fastcall' */


/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/

#define EDDI_SYSTEM_IN_FCT_ATTR   EDD_ATTR_FAR   /* system to prefix */
#define EDDI_SYSTEM_OUT_FCT_ATTR  EDD_ATTR_FAR   /* prefix to system */
#define EDDI_LOCAL_FCT_ATTR       EDD_ATTR_NEAR  /* local function */

/*=====  data attributes  =====*/

#define EDDI_LOWER_MEM_ATTR       EDD_ATTR_HUGE  /* lower-memory data */
#define EDDI_SYSTEM_MEM_ATTR      EDD_ATTR_HUGE  /* system-memory data */
#define EDDI_LOCAL_MEM_ATTR       EDD_ATTR_SHUGE /* local data */

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
/*   LSA_COMP_ID_TYPE                comp_id;    for lsa-component-id        */
/*                                                                           */
/* If LSA_RQB_HEADER is defined, we use the LSA header definition.           */
/*                                                                           */
/* Note: LSA_RQB_HEADER currently not defined within standard LSA headers    */
/*       This is a option to simplify integration if used, because typically */
/*       all RQB-headers within a system have the same format.               */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*==== set and get parameter of the rqb-header ===*/
// #define EDDI_RQB_SET_COMPID(_pRQB, _Value)         ( (_pRQB)->comp_id = (_Value) )
// #define EDDI_RQB_GET_COMPID(_pRQB)                 ( (_pRQB)->comp_id )

#if defined (EDDI_CFG_SET_ENDIAN_EXTERN)
/* EDDI_CFG_xxxxx_ENDIAN has to be set by makefile  */
#else
#define EDDI_CFG_LITTLE_ENDIAN  /* PC */
//#define EDDI_CFG_BIG_ENDIAN    /* MIPS */
#endif

#undef EDDI_CFG_BIG_ENDIAN

#if defined (EDDI_CFG_SET_IO_ENDIAN_EXTERN)
/* EDDI_CFG_IO_xxxxx_ENDIAN has to be set by makefile  */
#else
#define EDDI_CFG_IO_LITTLE_ENDIAN  /* PC */
//#define EDDI_CFG_IO_BIG_ENDIAN    /* MIPS */
#endif

#undef EDDI_CFG_IO_BIG_ENDIAN

/*---------------------------------------------------------------------------*/
/* Definition of ERTEC200/400/SOC - Additions                                */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* Changed because this feature has to be controlable by makefiles */
//#define EDDI_CFG_ERTEC_400
//#define EDDI_CFG_ERTEC_200
//#define EDDI_CFG_SOC
/* End Changed because this feature has to be controlable by makefiles */

/* Hardware variants */
#if defined (EDDI_CFG_ERTEC_400)

#define EDDI_CFG_REV5

//SW-Fix for HW bug in the TimerScoreBoard of ERTEC400
#define EDDI_CFG_USE_SW_RPS

//SW-Fix for IRTE-IP bug (limiting the length)
#define EDDI_CFG_MAX_IR_DATA_LENGTH  210UL

//#define EDDI_CFG_XRT_OVER_UDP_INCLUDE

#elif defined (EDDI_CFG_ERTEC_200)

#define EDDI_CFG_REV6

//SW-Fix for IRTE-IP bug concerning IRT
#define EDDI_CFG_MAX_IR_DATA_LENGTH  280UL

//#define EDDI_CFG_XRT_OVER_UDP_INCLUDE

//Activate 3-buffer-IF in SW for providers
#define EDDI_CFG_USE_SW_IIIB_IF_PROV

#elif defined (EDDI_CFG_SOC)

#ifndef EDDI_CFG_REV7
#define EDDI_CFG_REV7
#endif

//#define EDDI_CFG_XRT_OVER_UDP_INCLUDE

#else
#error "neither EDDI_CFG_ERTEC_400 nor EDDI_CFG_ERTEC_200 nor EDDI_CFG_SOC defined in eddi_cfg.h!"
#endif

/* NSC Transceiver (PHY)                */
#ifdef PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NSC
    #define EDDI_CFG_PHY_TRANSCEIVER_NSC
#endif

/* BROADCOM (BCM5221) Transceiver (PHY) */
#ifdef PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_NEC
    #define EDDI_CFG_PHY_TRANSCEIVER_NEC
    #ifdef PSI_CFG_EDDI_CFG_PHY_BLINK_EXTERNAL_NEC
        #define EDDI_CFG_PHY_BLINK_EXTERNAL_NEC
    #endif
#endif

/* NEC (AATHPHYC2) Transceiver (PHY)    */
#ifdef PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_BROADCOM
    #define EDDI_CFG_PHY_TRANSCEIVER_BROADCOM
#endif

/* TI (TLK111) Transceiver (PHY)        */
#ifdef PSI_CFG_EDDI_CFG_PHY_TRANSCEIVER_TI
    #define EDDI_CFG_PHY_TRANSCEIVER_TI            
#endif



#define EDDI_CFG_DEFAULT_UCMC_LFSR_VALUE    0x4A9

/*===========================================================================*/
/*========================= IRT-INTERFACE ===================================*/
/*===========================================================================*/
#define EDDI_IRT_DATALEN_MAX_REV5_DEBUG_0   210
#define EDDI_IRT_DATALEN_MAX_REV6_DEBUG_0   280

#define EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED    //resetting of IRTCtrl when resetting the switch-module depends
                                            //on parameter bResetIRTCtrl in dsb.SWIPara

/* tap@2010-07-27: pcIOX erlaubt Rot-Verschiebung zum Testen der "alten" Projektierungen */
#define EDDI_PRM_CFG_ALLOW_REDSHIFT       //if set, a parametrization with RedPeriodBegin-Values != 0 is allowed, otherwise rejected (ERR_PARAM)

/*===========================================================================*/
/*============================ LSA-TRACE ====================================*/
/*===========================================================================*/
#define EDDI_CFG_TRACE_MODE             (PSI_CFG_TRACE_MODE)

/*---------------------------------------------------------------------------*/

#define EDDI_CFG_CONSTRACE_DEPTH      64  //Depth of consumer-trace. If not defined trace is not active.

/*---------------------------------------------------------------------------*/

#if defined (EDDI_CFG_ERTEC_400)
// Note: Activated for EDDI RQ-AP00852195
#define EDDI_CFG_NRT_TS_TRACE_DEPTH    16 //Depth of nrt-timestamp-trace. If not defined trace is not active. Only allowed with EDDI_CFG_ERTEC_400!
#endif

//optional für EDDI Anbindung
#define EDDI_CFG_DO_EV_TIMER_REST
#define EDDI_CFG_DO_EV_TIMER_NRT_CHECK_FRAG_QUEUES

/*---------------------------------------------------------------------------*/

//#define EDDI_CFG_REV5_IF_IDX_EXCLUDED_FROM_ARP_FILTER  EDDI_NRT_CHB_IF_1  //ARPs received on this if are sent directly to it, regardless of a set ARP-filter
                                                                            //can be one of: EDDI_NRT_CHA_IF_0, EDDI_NRT_CHA_IF_1, EDDI_NRT_CHB_IF_0, EDDI_NRT_CHB_IF_1


/*===========================================================================*/
/* macro name:    EDDI_FILE_SYSTEM_EXTENSION(module_id_)                     */
/*                                                                           */
/* macro:         System integrators can use the macro to smuggle something  */
/*                into the component source files. By default it's empty.    */
/*                                                                           */
/*                The system integrators are responsible for the contents of */
/*                that macro.                                                */
/*                                                                           */
/*                Internal data of LSA components may not be changed!        */
/*                                                                           */
/* parameter:     Module-id                                                  */
/*                                                                           */
/*                There's no prototype for that macro!                       */
/*===========================================================================*/

#ifndef EDDI_FILE_SYSTEM_EXTENSION
#define EDDI_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif

//Do not use these switches until you have problems with the EDDI code size
//#define  EDDI_CFG_NO_FATAL_FILE_INFO
//#define  EDDI_CFG_NO_DETAIL_ERROR_LINE_INFO

/*---------------------------------------------------------------------------*/
/* EDDI_CFG_APPLSYNC_XXX                                                     */
/*---------------------------------------------------------------------------*/
/* Enable isochronous support (application synchronisation) and select if    */
/* it shares it´s resources with EDDI or if it runs on a separate processor. */

#if (  (defined (PSI_CFG_EDDI_CFG_APPLSYNC_SHARED)          && defined (PSI_CFG_EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED)) \
    || (defined (PSI_CFG_EDDI_CFG_APPLSYNC_SHARED)          && defined (PSI_CFG_EDDI_CFG_APPLSYNC_SEPARATE))        \
    || (defined (PSI_CFG_EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED) && defined (PSI_CFG_EDDI_CFG_APPLSYNC_SEPARATE)))
#error "Only one PSI_CFG_EDDI_CFG_APPLSYNC_xxx-define allowed at one time!"
#endif

/* for using isochronous support on the same interrupt line than the rest of the EDD-interrupts */
#ifdef PSI_CFG_EDDI_CFG_APPLSYNC_SHARED
    #define EDDI_CFG_APPLSYNC_SHARED
#endif

/* The NewCycle-interrupt uses the same interrupt line than the rest of the EDD-interrupts,
 * the other ints for isochronous support are using a separate interrupt line */
#ifdef PSI_CFG_EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED
    #define EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED
#endif

/* for using isochronous support on a separate interrupt line */
#ifdef PSI_CFG_EDDI_CFG_APPLSYNC_SEPARATE
    #define EDDI_CFG_APPLSYNC_SEPARATE
#endif

/*---------------------------------------------------------------------------*/
/* EDDI event modes: Polling mode etc. Set by system adaptation              */
/*---------------------------------------------------------------------------*/

// active at all product variants - 2015-02-26
#define EDDI_CFG_SII_ADD_PRIO1_AUX  

#ifdef PSI_CFG_EDDI_CFG_SII_POLLING_MODE
#define EDDI_CFG_SII_POLLING_MODE
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON
#define EDDI_CFG_SII_EXTTIMER_MODE_ON
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_FLEX
#define EDDI_CFG_SII_EXTTIMER_MODE_FLEX
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_INT_RESTART_MODE_LOOP
#define EDDI_CFG_SII_INT_RESTART_MODE_LOOP
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_REMOVE_ISR_LOCKS
#define EDDI_CFG_SII_REMOVE_ISR_LOCKS
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE
#define EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION
#define EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_NO_PNDEVDRV_SUPPORT
#define EDDI_CFG_SII_NO_PNDEVDRV_SUPPORT
#endif

#define EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US   (PSI_CFG_EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US)

#ifdef PSI_CFG_EDDI_CFG_SII_USE_SPECIAL_EOI
#define EDDI_CFG_SII_USE_SPECIAL_EOI                    (PSI_CFG_EDDI_CFG_SII_USE_SPECIAL_EOI)
#endif


/*---------------------------------------------------------------------------*/
/* EDDI_CFG_NO_ENHANCED_CRS_DV                                               */
/*---------------------------------------------------------------------------*/
/* Disables the Enhanced CRS_DV mode for NSC PHY when using RMII             */
/* By default this feature is enabled.                                       */

//#define EDDI_CFG_NO_ENHANCED_CRS_DV

/*---------------------------------------------------------------------------*/
/* EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT                                         */
/*---------------------------------------------------------------------------*/
/* Enable automatic filling of up to 3 bytes with 0 .                        */
/* !!!Attention!!! do not switch on, as data corruptions due to              */
/* disadvantageous buffer handling in TCP/IP can occur!                      */
#ifdef PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT
#define EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT
#endif

#if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
#define EDDI_ENABLE_NRT_CACHE_SYNC
#endif

#define EDDI_CONVENIENCE_FUNCTIONS  // activated for NIF

/*===========================================================================*/
/*                           alloc mem macros                                */
/*===========================================================================*/
#define EDDI_ALLOC_UPPER_RQB_LOCAL(upper_rqb_ptr_ptr, length )      PSI_ALLOC_LOCAL_MEM((upper_rqb_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_UPPER_RQB)
#define EDDI_ALLOC_LOCAL_MEM(local_mem_ptr_ptr, length )            PSI_ALLOC_LOCAL_MEM((local_mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM)
#define EDDI_ALLOC_UPPER_MEM_LOCAL(upper_mem_ptr_ptr, length)       PSI_ALLOC_LOCAL_MEM((upper_mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_UPPER_MEM)

#define EDDI_FREE_UPPER_RQB_LOCAL(ret_val_ptr, upper_rqb_ptr)       PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_UPPER_RQB)
#define EDDI_FREE_UPPER_MEM_LOCAL(ret_val_ptr, upper_mem_ptr)       PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_UPPER_MEM)
#define EDDI_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)             PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM)

#if defined __cplusplus
}
#endif

#endif //EDDI_CFG_H


/*****************************************************************************/
/*  end of file eddi_cfg.h/txt                                               */
/*****************************************************************************/
