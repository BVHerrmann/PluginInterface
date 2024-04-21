#ifndef EDDS_CFG_H                  /* ----- reinclude-protection ----- */
#define EDDS_CFG_H

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: edds_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for EDDS:                                                  */
/*  Defines constants, types and macros for EDDS.                            */
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
#ifdef EDDS_MESSAGE
/*  28.04.09    AH    initial version. LSA_EDDS_P04.02.00.00_00.01.01.01     */
/*  31.06.10    AH    added protection configuration defines (AP00952063)    */
#endif
/*****************************************************************************/

void edds_psi_startstop( int start );

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Set maximum multicast MAC address count for EDDS.                         */
/* Defines the number of multicast addresses supported by EDDS. Must be at   */
/* least 15 multicast MAC addresses.                                         */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_MAX_MC_MAC_CNT   15

/*---------------------------------------------------------------------------*/
/* Enable/disable ARP filtering in LL.                                       */
/* When LLIF_CFG_USE_LL_ARP_FILTER is defined and EDDS_LL_SET_ARP_FILTER is  */
/* present ARP filter are provided to the LL. The LL is then responsible for */
/* filtering ARP frames. EDDS does filter any ARP frames. When not defined   */
/* ARP filtering is only done within EDDS.                                   */             
/*---------------------------------------------------------------------------*/
#define LLIF_CFG_USE_LL_ARP_FILTER

/*---------------------------------------------------------------------------*/
/* If LSA_RESULT is not defined by LSA, we define it here.                   */
/*---------------------------------------------------------------------------*/

/* Note: LSA_RESULT has to be the same type as "response" within             */
/*       LSA-RQB-Header. This is currently LSA_UINT16!                       */

#ifndef LSA_RESULT
#define LSA_RESULT    LSA_RESPONSE_TYPE
#endif

/*---------------------------------------------------------------------------*/
/* maximum number of communication channels supported                        */
/* Note: Maximum is limited by LSA_HANDLE_TYPE range!                        */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_MAX_CHANNELS   (17 * PSI_CFG_MAX_EDDS_DEVICES)

#ifndef EDDS_CFG_MAX_CHANNELS
#error "EDDS_CFG_MAX_CHANNELS not defined"
#endif

/*---------------------------------------------------------------------------*/
/* Set some local_mem static or dynamic:                                     */
/* If EDDS_CFG_LOCAL_MEM_STATIC is not defined, all local-memory will be     */
/* allocated dynamically. If defined, some local memory is static.           */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_LOCAL_MEM_STATIC

/*---------------------------------------------------------------------------*/
/* Host-Byte-Order format:                                                   */
/* big-endian   : define EDDS_CFG_BIG_ENDIAN                                 */
/* little-endian: undef EDDS_CFG_BIG_ENDIAN (e.g. intel)                     */
/*---------------------------------------------------------------------------*/
#ifdef LSA_HOST_ENDIANESS_BIG
#define EDDS_CFG_BIG_ENDIAN
#endif

/*---------------------------------------------------------------------------*/
/* If a a memory-free call error should result in an fatal-error call        */
/* with EDDS_FATAL_ERR_MEM_FREE define EDDS_CFG_FREE_ERROR_IS_FATAL. If not  */
/* memory-free errors will be ignored.                                       */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_FREE_ERROR_IS_FATAL

/*---------------------------------------------------------------------------*/
/* If defined the Systemadaption can check if enough time elapsed since      */
/* last consumer-check (it can check if two consumer checks occur back to    */
/* back. e.g. to fast after each other, because of system delays/jitter)     */
/* The output-macros EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT() and              */
/* EDDS_CSRT_CONSUMER_CYCLE_START() used for this check if the               */
/* jitter check is defined.                                                  */
/*---------------------------------------------------------------------------*/
#undef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK

/*---------------------------------------------------------------------------*/
/* If EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK is defined, the EDDS can be  */
/* configured to call EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END() at the end  */
/* of consumer checks. With this the systemadaption can verify the time the  */
/* consumer checks ended. So on the next start of consumer checks it may     */
/* verify the time since last end instead of last start of consumer checks.  */
/* This is optional.                                                         */
/*---------------------------------------------------------------------------*/
#undef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK_END

/*---------------------------------------------------------------------------*/
/* Memory allocation                                                         */
/* If defined, some more local memory within edds will be allocated within   */
/* fast memory (EDDS_ALLOC_LOCAL_FAST_MEM) instead of normal memory.         */
/* Define if enough fast memory is present.                                  */
/*                                                                           */
/* GLOB:  Global EDDS management. Includes LLIF management.                  */
/* NRT :  NRT management                                                     */
/*                                                                           */
/* Note: SRT management always uses fast memory!                             */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_GLOB_FAST_MEM
#undef EDDS_CFG_NRT_FAST_MEM

/*===========================================================================*/
/*                            Default PHY adaption                           */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* If defined, the internal PHY adaptaion is included                        */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_USE_DEFAULT_PHY_ADAPTION

/*---------------------------------------------------------------------------*/
/* If defined, the internal BCM1000 PHY functions will be included with EDDS */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_PHY_BCM1000_SUPPORT

/*---------------------------------------------------------------------------*/
/* EDDS shall support the system redundancy API.                             */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_SYSRED_API_SUPPORT

/*===========================================================================*/
/* defaults for needed but not defined values..                              */
/*===========================================================================*/
#ifndef EDDS_FAR_FCT
#define EDDS_FAR_FCT
#endif

/*---------------------------------------------------------------------------*/
/* Type of systemhandles used by system for device and interface.            */
/*---------------------------------------------------------------------------*/
#define EDDS_SYS_HANDLE   LSA_SYS_PTR_TYPE // usage of SYS_PTR

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/

#define EDDS_SYSTEM_IN_FCT_ATTR  EDD_ATTR_FAR   /* system to prefix */
#define EDDS_SYSTEM_OUT_FCT_ATTR EDD_ATTR_FAR   /* prefix to system */
#define EDDS_LOCAL_FCT_ATTR      EDD_ATTR_NEAR  /* local function */

/*=====  data attributes  =====*/

#define EDDS_LOWER_MEM_ATTR      EDD_ATTR_HUGE  /* lower-memory data */
#define EDDS_SYSTEM_MEM_ATTR     EDD_ATTR_HUGE  /* system-memory data */
#define EDDS_LOCAL_MEM_ATTR      EDD_ATTR_SHUGE /* local data */

/*===========================================================================*/
/* macro name:    EDDS_FILE_SYSTEM_EXTENSION(module_id_)                     */
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
#ifndef EDDS_FILE_SYSTEM_EXTENSION
#define EDDS_FILE_SYSTEM_EXTENSION(module_id_)
#endif

/*===========================================================================*/
/* LTRC - Support (LSA-Trace)                                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define the Trace mode for EDDS                                            */
/* 0: no traces                                                              */
/* 1: enable LSA Traces                                                      */
/* 2: enable LSA Idx Traces                                                  */
/*---------------------------------------------------------------------------*/

#define EDDS_CFG_TRACE_MODE     (PSI_CFG_TRACE_MODE)


/*===========================================================================*/
/* Debugging                                                                 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* For additional internal debugging                                         */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_DEBUG_ENABLE
#undef  EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT

/*---------------------------------------------------------------------------*/
/* setting this define will push the call of EDDS_RequestFinish() for        */
/* NRT_SEND Frames back into the RQB-Context                                 */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_FINISHEDGENERALREQUESTQUEUE_NRT_SEND_ENABLE

/*---------------------------------------------------------------------------*/
/* setting this define will push the call of EDDS_RequestFinish() for        */
/* NRT_RCV Frames back into the RQB-Context                                  */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_FINISHEDGENERALREQUESTQUEUE_NRT_RCV_ENABLE

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* cpu cache line size in byte; could be a constant or a global variable (LSA_UINT) */
#undef EDDS_CPU_CACHE_LINE_SIZE

/*===========================================================================*/
/*                                io buffer                                  */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* If defined IOBuffer is supported and compiled with EDDS. Enable this      */
/* option for application and EDDS running on the same processor. In         */
/* distributed  case (application with IOBuffer handler and EDDS running on  */
/* a different processor) this define can be undefined.                      */
/*---------------------------------------------------------------------------*/
#define EDDS_CFG_USE_IOBUFFER

/*---------------------------------------------------------------------------*/
/* The system adaption has to set the type properly to meet the requirements */
/* for an implementation of the synchronization mechanism depending on the   */
/* destination platform. The type must be at least LSA_UINT16.               */
/*---------------------------------------------------------------------------*/
#define EDDS_EXCHANGE_TYPE  long

/*===========================================================================*/
/*                           alloc mem macros                                */
/*===========================================================================*/
#define EDDS_ALLOC_UPPER_RQB_LOCAL(mem_ptr_ptr, length)   PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_RQB)
#define EDDS_ALLOC_UPPER_MEM_LOCAL(mem_ptr_ptr, length)   PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_MEM)
#define EDDS_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)         PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)
#define EDDS_ALLOC_LOCAL_FAST_MEM(mem_ptr_ptr, length)    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)
#define EDDS_IOBUFFER_MEM_ALLOC(mem_ptr_ptr, length)      PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)
#define EDDS_IOBUFFER_ALLOC_MNGM_MEM(hSysDev, mem_ptr_ptr, length) \
            LSA_UNUSED_ARG(hSysDev); \
            PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)

#define EDDS_FREE_UPPER_RQB_LOCAL(ret_val_ptr, upper_rqb_ptr)   PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_RQB)
#define EDDS_FREE_UPPER_MEM_LOCAL(ret_val_ptr, upper_mem_ptr)   PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_mem_ptr), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_UPPER_MEM)
#define EDDS_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)         PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)
#define EDDS_FREE_LOCAL_FAST_MEM(ret_val_ptr, local_mem_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)
#define EDDS_IOBUFFER_MEM_FREE(ret_val_ptr, pMem)               PSI_FREE_LOCAL_MEM((ret_val_ptr), (pMem), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)
#define EDDS_IOBUFFER_FREE_MNGM_MEM(hSysDev, ret_val_ptr, pMem) \
            LSA_UNUSED_ARG(hSysDev); \
            PSI_FREE_LOCAL_MEM((ret_val_ptr), (pMem), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM)

/*****************************************************************************/
/*  end of file EDDS_CFG.H                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_CFG_H */
