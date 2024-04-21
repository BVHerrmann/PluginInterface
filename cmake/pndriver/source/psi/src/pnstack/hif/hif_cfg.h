#ifndef HIF_CFG_H               /* ----- reinclude-protection ----- */
#define HIF_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: hif_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System integration of LSA-component HIF using PSI.                       */
/*                                                                           */
/*****************************************************************************/

/*
 * included by "hif_inc.h"
 */

void hif_psi_startstop(int start);

/*===========================================================================*/
/*                                HIF-Settings                               */
/*===========================================================================*/

// Those defines are constant if HIF is used within PSI
#define HIF_CFG_USE_CLRPC       (1)
#define HIF_CFG_USE_OHA         (1)
#define HIF_CFG_USE_PSI         (1)

#define HIF_CFG_USE_ACP         (1)
#define HIF_CFG_USE_CM          (1)
#define HIF_CFG_USE_DCP         (1)
#define HIF_CFG_USE_DNS         (PSI_CFG_USE_DNS)
#define HIF_CFG_USE_EDD         (PSI_CFG_USE_EDDI | PSI_CFG_USE_EDDP | PSI_CFG_USE_EDDS | PSI_CFG_USE_EDDT)
#define HIF_CFG_USE_GSY         (PSI_CFG_USE_GSY)
#define HIF_CFG_USE_IOH         (PSI_CFG_USE_IOH)
#define HIF_CFG_USE_LLDP        (1)
#define HIF_CFG_USE_MRP         (PSI_CFG_USE_MRP)
#define HIF_CFG_USE_NARE        (1)
#define HIF_CFG_USE_PNTRC       (0)
#define HIF_CFG_USE_SOCK        (1)
#define	HIF_CFG_USE_SOCKAPP		(PSI_CFG_USE_SOCKAPP)
#define HIF_CFG_USE_SNMPX       (1)
#define HIF_CFG_USE_HIF         (PSI_CFG_USE_HIF)
#define HIF_CFG_USE_EPS_RQBS    (PSI_CFG_USE_EPS_RQBS)
#define	HIF_CFG_USE_HSA			(PSI_CFG_USE_HSA)
#define HIF_CFG_USE_DMA         (PSI_CFG_USE_HIF_DMA)

#ifdef PSI_CFG_HIF_CFG_ALIGNMENT_SAVE_COPY_FUNCTION
    #define HIF_CFG_ALIGNMENT_SAVE_COPY_FUNCTION
#endif

#ifdef PSI_CFG_HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC
    #define HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC
    #include "stdlib.h"                 //needed for malloc prototype 
#endif

// Not used at the moment
//#define HIF_CFG_USE_SND_WAIT_ACK          // Use Ack for Send/Receive

// measure the performance of HIF using LSA_TRACE
#ifdef PSI_CFG_HIF_DEBUG_MEASURE_PERFORMANCE
    #define HIF_DEBUG_MEASURE_PERFORMANCE 
#endif

/*------------------------------------------------------------------------------
// HIF: size of a LSA_BOOL for HIF (1 || 2 || 4 ==> 8 || 16 || 32 Bit)
//----------------------------------------------------------------------------*/
#define HIF_LSA_BOOL_SIZE   1

/*------------------------------------------------------------------------------
// HIF Context Change Output Macro on hif_lower_request_done() and hif_lower_sys_request_done() call
//----------------------------------------------------------------------------*/
#define HIF_CFG_USE_SYS_REQUEST_LOWER_DONE_OUTPUT_MACRO 0
#define HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO     0

/*------------------------------------------------------------------------------
// HIF Number of highest LSA_COMP_ID in system + 1
//----------------------------------------------------------------------------*/
#define HIF_MAX_COMP_ID (PSI_COMP_ID_LAST + 1)

/*------------------------------------------------------------------------------
// HIF Compile RQB serialization
//----------------------------------------------------------------------------*/
#define HIF_CFG_COMPILE_SERIALIZATION       (PSI_CFG_HIF_CFG_COMPILE_SERIALIZATION)

/*------------------------------------------------------------------------------
// HIF MEM malloc Settings
//----------------------------------------------------------------------------*/
#define HIF_CFG_MEM_MALLOC_ALIGN        32          /* 32 byte allignment     */

// a user may want to use HIF_MEM_MALLOC, currently not used by PSI.
#ifndef HIF_CFG_MEM_MALLOC_POOL_MAX
#define HIF_CFG_MEM_MALLOC_POOL_MAX     0           /* no global storage used */
#endif

/*------------------------------------------------------------------------------
// HIF MEM Ring Pool Settings
//----------------------------------------------------------------------------*/

#define HIF_CFG_MEM_RING_POOL_ALIGN     32          /* 32 byte allignment     */

/*===========================================================================*/
/*                       HIF_CFG_MEM_RING_CHECK_OVERWRITE                    */
/*===========================================================================*/
/** 
  * This switch can be activated to check HIF serialization/deserialization overwrite in Shared Memory.
  * Possible values are:
  *
  * HIF_MEM_RING_CHECK_START_STOP_1
  * Restrictions: currently only available when using HIF_MEM_RING
  * After every serialization the current SER writepointer will be checked against the RQB SHM start pointer + RQB serialization size.
  * Will generate a START sequenze (STAR (0x)) before the allocated SER buffer and a STOP sequenze (STOP (0x)) after the buffer. These 
  * 8 Bytes (2 * 4 Bytes) will be transparent for the "user". After every de-/serialization HIF will check if the START and STOP sequences
  * are okay and not overwritten.
  *
  * HIF_MEM_RING_CHECK_START_STOP_4
  * Restrictions: currently only available when using HIF_MEM_RING
  * Includes HIF_MEM_RING_CHECK_START_STOP_1 but with 4 START sequences at the beginning and 4 STOP sequences after the allocated shared memory
  **/
//#define HIF_CFG_MEM_RING_CHECK_OVERWRITE     HIF_MEM_RING_CHECK_START_STOP_4

/*------------------------------------------------------------------------------
// HIF User defined Pipes
//----------------------------------------------------------------------------*/

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 1)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK1_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_1 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK1_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 2)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK2_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_2 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK2_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 3)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK3_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_3 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK3_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 4)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK4_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_4 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK4_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 5)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK5_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_5 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK5_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 6)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK6_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_6 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK6_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 7)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK7_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_7 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK7_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS == 8)
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK8_PIPE { HIF_CPLD_GLO_APP_SOCK_USER_CH_8 , LSA_COMP_ID_SOCK  },
#else
    #define HIF_CFG_LD_GLOB_USR_APP_SOCK8_PIPE 
#endif

#if (PSI_CFG_MAX_SOCK_APP_CHANNELS > 8)
    #error "HIF: Only 8 sock app user channels supported by HIF"
#endif

#if (HIF_CFG_USE_SOCKAPP == 1)
#define HIF_CPLD_GLO_APP_SOCKAPP_USER { HIF_CPLD_GLO_APP_SOCKAPP_USER_CH , LSA_COMP_ID_SOCKAPP },
#else
#define HIF_CPLD_GLO_APP_SOCKAPP_USER
#endif

#if (HIF_CFG_USE_SNMPX == 1)
    #define HIF_CPLD_GLO_APP_SNMPX_USER { HIF_CPLD_GLO_APP_SNMPX_USER_CH , LSA_COMP_ID_SNMPX },
#else
    #define HIF_CPLD_GLO_APP_SNMPX_USER
#endif

#if (HIF_CFG_USE_DNS == 1)
    #define HIF_CPLD_GLO_APP_DNS_USER { HIF_CPLD_GLO_APP_DNS_USER_CH , LSA_COMP_ID_DNS },
#else
    #define HIF_CPLD_GLO_APP_DNS_USER
#endif

#if (HIF_CFG_USE_HSA == 1)
    #define HIF_CPLD_GLO_APP_HSA_USER { HIF_CPLD_GLO_APP_HSA_USER_CH , LSA_COMP_ID_HSA },
#else
    #define HIF_CPLD_GLO_APP_HSA_USER
#endif

/* User defined Interface specific Pipes for the HIF Logical Device */
#if (HIF_CFG_USE_IOH == 1)
#define HIF_CFG_LD_GLOB_USR_APP_IOH { HIF_CPLD_GLO_APP_IOH_USER_CH, LSA_COMP_ID_IOH  },
#else
#define HIF_CFG_LD_GLOB_USR_APP_IOH
#endif

/* User defined global Pipes for the HIF Logical Device */
#define HIF_CFG_LD_GLOB_USR_PIPES  {     \
		HIF_CFG_LD_GLOB_USR_APP_SOCK1_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK2_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK3_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK4_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK5_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK6_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK7_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_SOCK8_PIPE                              /* User Application Channel - Sock           */ \
		HIF_CFG_LD_GLOB_USR_APP_IOH                                     /* User Application Channel - IO-Application */ \
        HIF_CPLD_GLO_APP_SOCKAPP_USER                                   /* User Application Channel - SOCKAPP User   */ \
		HIF_CPLD_GLO_APP_SNMPX_USER                                     /* User Application Channel - SNMPX          */ \
        HIF_CPLD_GLO_APP_DNS_USER                                       /* User Application Channel - DNS User       */ \
        HIF_CPLD_GLO_APP_HSA_USER                                       /* User Application Channel - HSA User       */ \
		{ HIF_CPLD_GLO_APP_OHA_USER_CH    , LSA_COMP_ID_OHA   },        /* User Application Channel - Object Handler */ \
		{ HIF_CPLD_GLO_APP_CLRPC_USER_CH  , LSA_COMP_ID_CLRPC }         /* User Application Channel - CLRPC          */ \
	}

#define HIF_CFG_LD_IF_USR_PIPES  {     \
		{ HIF_CPLD_IF_APP_CMPD_USER_CH          , LSA_COMP_ID_CM   }, /* User Application Channel - Physical Device       */ \
		{ HIF_CPLD_IF_APP_IOC_CMCL_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Controller         */ \
		{ HIF_CPLD_IF_APP_IOM_CMMC_USER_CH      , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Multicast          */ \
		{ HIF_CPLD_IF_APP_IOD_CMSV_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Device             */ \
		{ HIF_CPLD_IF_APP_OHA_USER_CH           , LSA_COMP_ID_OHA  }, /* User Application Channel - Object Handler        */ \
		{ HIF_CPLD_IF_APP_EDD_SIMATIC_TIME_SYNC , LSA_COMP_ID_EDD  }, /* User Application Channel - Edd Simatic Time Sync */ \
		{ HIF_CPLD_IF_APP_NARE_IP_TEST          , LSA_COMP_ID_NARE }  /* User Application Channel - Nare Ip Test          */ \
	}

/* User defined Global Pipes for the HIF Hardware Device (not used) */
//#define HIF_CFG_HD_GLOB_USR_PIPES  {   
//		{ USR_PIPE1, LSA_COMP_ID_SOCK }, 
//		{ USR_PIPE2, LSA_COMP_ID_SOCK }  
//	}

/* User defined Interface specific Pipes for the HIF Hardware Device */
#if (HIF_CFG_USE_MRP == 1)
    #define HIF_CPHD_IF_SYS_MRP_EDD_ { HIF_CPHD_IF_SYS_MRP_EDD , LSA_COMP_ID_MRP },
#else
    #define HIF_CPHD_IF_SYS_MRP_EDD_
#endif

#if (HIF_CFG_USE_GSY == 1)
    #define HIF_CPHD_IF_SYS_GSY_EDD_SYNC_ { HIF_CPHD_IF_SYS_GSY_EDD_SYNC , LSA_COMP_ID_GSY },
    #define HIF_CPHD_IF_SYS_GSY_EDD_ANNO_ { HIF_CPHD_IF_SYS_GSY_EDD_ANNO , LSA_COMP_ID_GSY },
#else
    #define HIF_CPHD_IF_SYS_GSY_EDD_SYNC_
    #define HIF_CPHD_IF_SYS_GSY_EDD_ANNO_
#endif

#define HIF_CFG_HD_IF_USR_PIPES  {     \
		{ HIF_CPHD_IF_APP_CMPD_USER_CH          , LSA_COMP_ID_CM   }, /* User Application Channel - Physical Device       */ \
		{ HIF_CPHD_IF_APP_IOC_CMCL_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Controller         */ \
		{ HIF_CPHD_IF_APP_IOM_CMMC_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Multicast          */ \
		{ HIF_CPHD_IF_APP_IOD_CMSV_ACP_USER_CH  , LSA_COMP_ID_CM   }, /* User Application Channel - IO-Device             */ \
		{ HIF_CPHD_IF_APP_EDD_SIMATIC_TIME_SYNC , LSA_COMP_ID_EDD  }, /* User Application Channel - Edd Simatic Time Sync */ \
		{ HIF_CPHD_IF_APP_NARE_IP_TEST          , LSA_COMP_ID_NARE }, /* User Application Channel - Nare Ip Test          */ \
		{ HIF_CPHD_IF_SYS_CM_CLRPC              , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_OHA                , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		HIF_CPHD_IF_SYS_MRP_EDD_                                      /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_DCP_EDD               , LSA_COMP_ID_DCP  }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_NARE_EDD              , LSA_COMP_ID_NARE }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_NARE_DCP_EDD          , LSA_COMP_ID_NARE }, /* Pipe used by PSI LD Channel State Machine        */ \
		HIF_CPHD_IF_SYS_GSY_EDD_SYNC_                                 /* Pipe used by PSI LD Channel State Machine        */ \
		HIF_CPHD_IF_SYS_GSY_EDD_ANNO_                                 /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_ACP_EDD               , LSA_COMP_ID_ACP  }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_EDD                , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_MRP                , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_POF_EDD            , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_GSY_CLOCK          , LSA_COMP_ID_CM   }, /* Pipe used by PSI LD Channel State Machine        */ \
		{ HIF_CPHD_IF_SYS_CM_NARE               , LSA_COMP_ID_CM   }  /* Pipe used by PSI LD Channel State Machine        */ \
	}

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#define HIF_DEBUG  (PSI_DEBUG)

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/* LSA-HEADER and LSA-TRAILER                                                */
/*===========================================================================*/

#define HIF_RQB_HEADER						LSA_RQB_HEADER(HIF_RQB_PTR_TYPE)

#define HIF_RQB_TRAILER						LSA_RQB_TRAILER

#define HIF_RQB_SET_NEXT_RQB_PTR(rb,v)		LSA_RQB_SET_NEXT_RQB_PTR(rb,v)
#define HIF_RQB_SET_PREV_RQB_PTR(rb,v)		LSA_RQB_SET_PREV_RQB_PTR(rb,v)
#define HIF_RQB_SET_OPCODE(rb,v)			LSA_RQB_SET_OPCODE(rb,v)
#define HIF_RQB_SET_HANDLE(rb,v)			LSA_RQB_SET_HANDLE(rb,v)
#define HIF_RQB_SET_RESPONSE(rb,v)			LSA_RQB_SET_RESPONSE(rb,v)
#define HIF_RQB_SET_USER_ID_PTR(rb,v)		LSA_RQB_SET_USER_ID_PTR(rb,v)
#define HIF_RQB_SET_USER_ID(rb,v)			LSA_RQB_SET_USER_ID(rb,v)
#define HIF_RQB_SET_COMP_ID(rb,v)			LSA_RQB_SET_COMP_ID(rb,v)

#define HIF_RQB_GET_NEXT_RQB_PTR(rb)		LSA_RQB_GET_NEXT_RQB_PTR(rb)
#define HIF_RQB_GET_PREV_RQB_PTR(rb)		LSA_RQB_GET_PREV_RQB_PTR(rb)
#define HIF_RQB_GET_OPCODE(rb)				LSA_RQB_GET_OPCODE(rb)
#define HIF_RQB_GET_HANDLE(rb)				LSA_RQB_GET_HANDLE(rb)
#define HIF_RQB_GET_RESPONSE(rb)			LSA_RQB_GET_RESPONSE(rb)
#define HIF_RQB_GET_USER_ID_PTR(rb)			LSA_RQB_GET_USER_ID_PTR(rb)
#define HIF_RQB_GET_USER_ID(rb)				LSA_RQB_GET_USER_ID(rb)
#define HIF_RQB_GET_COMP_ID(rb)             LSA_RQB_GET_COMP_ID(rb)

#define HIF_EDD_RQB_SET_HANDLE_LOWER(rb,v)  EDD_RQB_SET_HANDLE_LOWER(rb,v)

#define HIF_EDD_RQB_GET_HANDLE_LOWER(rb)    EDD_RQB_GET_HANDLE_LOWER(rb)

/*------------------------------------------------------------------------------
// HIF System Handle definition
//----------------------------------------------------------------------------*/

#define HIF_SYS_HANDLE   LSA_SYS_PTR_TYPE // usage of SYS_PTR

/*------------------------------------------------------------------------------
// HD/IF Id (Hardware Device ID / Interface ID)
//----------------------------------------------------------------------------*/

#define HIF_HD_ID_TYPE   LSA_UINT16

/*------------------------------------------------------------------------------
// maximum number of HIF Instances
//----------------------------------------------------------------------------*/
#define HIF_CFG_MAX_LD_INSTANCES            (PSI_CFG_HIF_CFG_MAX_LD_INSTANCES)

#define HIF_CFG_MAX_HD_INSTANCES            (PSI_CFG_HIF_CFG_MAX_HD_INSTANCES)

/*------------------------------------------------------------------------------
// maximum number of HIF Channel handles
//----------------------------------------------------------------------------*/
#define HIF_CFG_MAX_LD_CH_HANDLES           (PSI_CFG_HIF_CFG_MAX_LD_CH_HANDLES)

#define HIF_CFG_MAX_HD_CH_HANDLES           (PSI_CFG_HIF_CFG_MAX_HD_CH_HANDLES)

/*------------------------------------------------------------------------------
// usage of App specific HD Sock channels not defined
//----------------------------------------------------------------------------*/
#define HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS  (PSI_CFG_HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS)

/*------------------------------------------------------------------------------
// HIF ISR Poll Interval
//----------------------------------------------------------------------------*/
#define HIF_CFG_POLL_INTERVAL               (PSI_CFG_HIF_CFG_POLL_INTERVAL)

/*------------------------------------------------------------------------------
// maximum number of HIF hardware devices per logical device
//----------------------------------------------------------------------------*/
#define HIF_LD_MAX_HD           (PSI_CFG_MAX_IF_CNT)

/*------------------------------------------------------------------------------
// HIF Options
//----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
// enable / disable cast-warnings
//----------------------------------------------------------------------------*/

#ifndef HIF_CFG_CAST_WARN
#define HIF_CFG_CAST_WARN  0
#endif

/*------------------------------------------------------------------------------
// enable / disable HIF_ASSERT
//----------------------------------------------------------------------------*/

#if HIF_DEBUG

/* check for condition: internal programming error */
#define HIF_ASSERT(cond)  if (! (cond)) HIF_FATAL (0)

#else

/* no more programming error exists :-) */
#define HIF_ASSERT(cond)

#endif

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef HIF_FILE_SYSTEM_EXTENSION
#define HIF_FILE_SYSTEM_EXTENSION(module_id_)
#endif

/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 .. no LSA trace
//	1 .. LSA trace [default]
//	2 .. LSA index trace
//----------------------------------------------------------------------------*/
#define HIF_CFG_TRACE_MODE      (PSI_CFG_TRACE_MODE)

/*===========================================================================*/
/*   stdLib                                                                  */
/*===========================================================================*/
#define HIF_MEMCPY              PSI_MEMCPY
#define HIF_MEMCMP              PSI_MEMCMP
#define HIF_MEMSET              PSI_MEMSET
#define HIF_STRLEN              PSI_STRLEN

/*===========================================================================*/
/*     Serialization alignment                                               */
/*===========================================================================*/

#ifndef HIF_SER_SIZEOF
#define HIF_SER_SIZEOF(x)   (((x) + 3) & ~3)                     /* Returns sizeof corrected to shared memory alignment */
#else
#error "by design a function!"
#endif

#ifndef HIF_SER_SIZEOF_BUF
// Inline implementation of "if (HIF_SER_SIZEOF(x) != 0) { <value> = (HIF_SER_SIZEOF(x) + (HIF_SER_SIZEOF(4)} else { <value> = 0 } "
#define HIF_SER_SIZEOF_BUF(x)\
    HIF_SER_SIZEOF(x)? (HIF_SER_SIZEOF(x) + (HIF_SER_SIZEOF(4))) : 0       /* Returns size required for buffer + "copy finsihed" flag*/ 
#else
#error "by design a function!"
#endif

/****************************************************************************
*  Name:        HIF_SER_COPY_HTSHM_BUF()/HIF_SER_COPY_HTSHM_BUF_DMA()
*  Purpose:     - copies data from host to shared-mem without swapping
*               - increases help-pointer
*  Input:       src, destination address and size
*  Return:      -
****************************************************************************/
#if (HIF_CFG_USE_DMA == 1)
#define HIF_SER_COPY_HTSHM_BUF_DMA(dst, src, size, hSysDev) \
{ \
    PSI_HIF_MEMCPY_BUF_DMA(*(dst), src, size, hSysDev); \
    *(dst) = (LSA_VOID*)((LSA_UINT8*)(*(dst)) + (HIF_SER_SIZEOF_BUF(size))); \
}
#else
#define HIF_SER_COPY_HTSHM_BUF(dst, src, size) \
{ \
    PSI_HIF_MEMCPY_BUF(*(dst), src, size); \
    *(dst) = (LSA_VOID*)((LSA_UINT8*)(*(dst)) + (HIF_SER_SIZEOF_BUF(size))); \
}
#endif

/****************************************************************************
*  Name:        HIF_BUF_DMA_PEND_FREE()
*  Purpose:     - Use dma to set the free dma buffer flag. 
*                 Buffer used by dma can only be freed after dma transfer 
*                 finished. HIF_DMA_FREE_BUF_FLAG will be set by dma after 
*                 all current transfers finished.
*  Input:       - buffer address
*  Output:      - pReadyForFree address where dma writes HIF_DMA_FREE_BUF_FLAG, 
*                 after buffer is no longer in use.
*  Return:      - LSA_FALSE when buffer is not in use by dma.
*               - LSA_TRUE when dma pended a free for buffer.
****************************************************************************/
#if (HIF_CFG_USE_DMA == 1)
#define HIF_BUF_DMA_PEND_FREE(ptr_mem, pReadyForFree, hSysDev) \
    PSI_HIF_BUF_DMA_PEND_FREE(ptr_mem, pReadyForFree, hSysDev); 
#endif

/*=============================================================================
* function name:  HIF_SER_COPY_SHMTH_BUF
*
* function:       Copies an Buffer from the shared memory to host memory
*                 and increases src pointer. This function waits until the magic value
*                 PSI_HIF_MEMCPY_BUF_WAIT_TX_FINISHED is written in the shared memory.
*                 If the size is 0, no data is copied and the magic value is not written.
*
* parameters:     LSA_UINT32*  dest  - Destination Ptr for the value
*                 LSA_UINT32** src   - Source Values which should be copied
*                 LSA_UINT32   size  - Length of the buffer in bytes
*
* return value:   LSA_VOID
*
*===========================================================================*/
#define HIF_SER_COPY_SHMTH_BUF(dest, src, size)\
{\
    if(size > 0) \
    {\
        PSI_HIF_MEMCPY_BUF_WAIT_TX_FINISHED(*(src)); \
        *(src) = (LSA_VOID*)((LSA_UINT8*)(*(src)) + (HIF_SER_SIZEOF(4))); \
        PSI_MEMCPY((dest), *(src), (size)); \
    }\
    *(src) = (LSA_VOID*)((LSA_UINT8*)(*(src)) + (HIF_SER_SIZEOF(size))); \
}


/*- alloc mem macros----------------------------------------------------------*/

// HIF_DEBUG_MEASURE_PERFORMANCE not useable for ALLOC Macros, because no traces in Macros are allowed

#if defined(HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC)
#define HIF_ALLOC_LOCAL_MEM(mem_ptr_ptr, length, componentId, memType) { \
                LSA_UNUSED_ARG(componentId); \
                LSA_UNUSED_ARG(memType); \
                *mem_ptr_ptr = malloc(length); \
            }
#else
#define HIF_ALLOC_LOCAL_MEM(mem_ptr_ptr, length, componentId, memType) { \
                if (memType == HIF_MEM_TYPE_RQB) \
                { \
                    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, (componentId), PSI_MTYPE_HIF_RQB_MEM); \
                } \
                else \
                { \
                    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, (componentId), PSI_MTYPE_HIF_NO_RQB_MEM); \
                } \
            }
#endif

#if ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))
#define HIF_NRT_ALLOC_RX_MEM(mem_ptr_ptr, length, hd_id) { \
            if (psi_hd_is_edd_nrt_copy_if_on(hd_id)) \
            { \
                PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); \
            } \
            else \
            {\
                LSA_INT const nrt_pool_handle = psi_hd_get_nrt_rx_pool_handle(hd_id); \
                if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle) \
                { \
                    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); \
                } \
                else \
                { \
                    if (PSI_CFG_USE_EDDT == 1) \
                    { \
                        LSA_UINT8 * help_mem_ptr; \
                        PSI_NRT_ALLOC_RX_MEM((LSA_VOID_PTR_TYPE*)&help_mem_ptr, (length + EDDT_DATABUFFER_RXHEADER_SIZE), nrt_pool_handle, LSA_COMP_ID_HIF); \
                        if (help_mem_ptr) \
                        { \
                            help_mem_ptr += EDDT_DATABUFFER_RXHEADER_SIZE; \
                        } \
                        *(mem_ptr_ptr) = (LSA_VOID_PTR_TYPE)help_mem_ptr; \
                    }\
                    else \
                    { \
                        PSI_NRT_ALLOC_RX_MEM((mem_ptr_ptr), (length), nrt_pool_handle, LSA_COMP_ID_HIF); \
                    } \
                } \
            } \
        }
#else
#define HIF_NRT_ALLOC_RX_MEM(mem_ptr_ptr, length, hd_id)
#endif // ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))

#ifdef PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT            //map to a 0 or 1 for the alloc macros
#define PSI_HIF_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT   1
#else
#define PSI_HIF_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT   0
#endif

#ifndef EDDT_DATABUFFER_TXHEADER_SIZE
#define EDDT_DATABUFFER_TXHEADER_SIZE   0                   //when it is not defined by eddt already, set it to '0'
#endif

#if ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))
#define HIF_NRT_ALLOC_TX_MEM(mem_ptr_ptr, length, hd_id ) { \
    if (psi_hd_is_edd_nrt_copy_if_on(hd_id)) \
    { \
		PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); /* allocate NRT buffer from local memory pool */ \
    } \
    else /* edd nrt copy interface off */ \
    { \
 	    LSA_INT const nrt_pool_handle = psi_hd_get_nrt_tx_pool_handle(hd_id); \
	    if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle) \
	    { \
		    PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); /* allocate NRT buffer from local memory pool */ \
	    } \
	    else /* regular NRT pool handle */ \
	    { \
            if (PSI_CFG_USE_EDDS == 1) \
            { \
                /* Here the EDD comp_id is not available. So the padding is made for all EDDx, not only for EDDS! */ \
                if ((length) < 60UL) \
                { \
                    (length) = 60UL; /*set minimum length required by TI AM5728 GMAC */ \
                } \
            } \
            if ((PSI_CFG_USE_EDDI == 1) && PSI_HIF_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT ) \
            { \
                /* Here the EDD comp_id is not available. So the padding is made for all EDDx, not only for EDDI! */ \
                if ((length) < 64UL) \
                { \
                    /* padding to 4-byte-aligned length (EDDI specific bug) */ \
                    if ((length) & 3UL) \
                    { \
                        (length) = (LSA_UINT32)((length) & (~3UL)) + 4UL; \
                    } \
                } \
            } \
            if (PSI_CFG_USE_EDDT == 1) \
            /* Here the EDD comp_id is not available. So the specific NRT TX buffer handling is made for all EDDx, not only for EDDT! */ \
            { \
	            LSA_UINT8 * help_mem_ptr; \
                /* allocate NRT buffer from NRT memory pool */ \
	            PSI_NRT_ALLOC_TX_MEM((LSA_VOID_PTR_TYPE*)&help_mem_ptr, ((length)+EDDT_DATABUFFER_TXHEADER_SIZE), nrt_pool_handle, LSA_COMP_ID_HIF); \
                if (help_mem_ptr) /* mem available? */ \
                { \
                    help_mem_ptr+=EDDT_DATABUFFER_TXHEADER_SIZE; \
                } \
                *(mem_ptr_ptr) = (LSA_VOID_PTR_TYPE)help_mem_ptr; \
            } \
            else /* PSI_CFG_USE_EDDT */ \
            { \
                /* allocate NRT buffer from NRT memory pool */ \
	            PSI_NRT_ALLOC_TX_MEM((mem_ptr_ptr), (length), nrt_pool_handle, LSA_COMP_ID_HIF); \
            } /* PSI_CFG_USE_EDDT */ \
        } \
    } \
}
#else
#define HIF_NRT_ALLOC_TX_MEM(mem_ptr_ptr, length, hd_id )
#endif // ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))

#if defined(HIF_ALLOC_LOCAL_MEM_WITH_OS_MALLOC)
#define HIF_FREE_LOCAL_MEM(ret_val_ptr, ptr_mem, componentId, memType) { \
                LSA_UNUSED_ARG(componentId); \
                LSA_UNUSED_ARG(memType); \
                free(ptr_mem); \
                *ret_val_ptr = HIF_OK; \
            }
#else
#define HIF_FREE_LOCAL_MEM(ret_val_ptr, ptr_mem, componentId, memType) { \
    if (memType == HIF_MEM_TYPE_RQB) \
    { \
        PSI_FREE_LOCAL_MEM((ret_val_ptr), (ptr_mem), 0, (componentId), PSI_MTYPE_HIF_RQB_MEM); \
    } \
    else \
    { \
        PSI_FREE_LOCAL_MEM((ret_val_ptr), (ptr_mem), 0, (componentId), PSI_MTYPE_HIF_NO_RQB_MEM); \
    } \
}
#endif

#if ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))
#define HIF_NRT_FREE_RX_MEM(ret_val_ptr, mem_ptr, hd_id ) { \
    if (psi_hd_is_edd_nrt_copy_if_on(hd_id)) \
    { \
        PSI_FREE_LOCAL_MEM((ret_val_ptr), (mem_ptr), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); \
    } \
    else { \
        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_rx_pool_handle(hd_id); \
        if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle) { \
            PSI_FREE_LOCAL_MEM((ret_val_ptr), (mem_ptr), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); \
        } \
        else { \
            PSI_NRT_FREE_RX_MEM((ret_val_ptr), (mem_ptr), nrt_pool_handle, LSA_COMP_ID_HIF); \
        } \
    } \
}
#else
#define HIF_NRT_FREE_RX_MEM(ret_val_ptr, mem_ptr, hd_id )
#endif // ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))


#if ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))
#define HIF_NRT_FREE_TX_MEM(ret_val_ptr, mem_ptr, hd_id) { \
    if (psi_hd_is_edd_nrt_copy_if_on(hd_id)) { \
        PSI_FREE_LOCAL_MEM((ret_val_ptr), (mem_ptr), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); \
    } \
    else { \
        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_tx_pool_handle(hd_id); \
        if (PSI_NRT_POOL_HANDLE_INVALID == nrt_pool_handle) { \
            PSI_FREE_LOCAL_MEM((ret_val_ptr), (mem_ptr), 0, LSA_COMP_ID_HIF, PSI_MTYPE_NRT_MEM); \
        } \
        else { \
            PSI_NRT_FREE_TX_MEM((ret_val_ptr), (mem_ptr), nrt_pool_handle, LSA_COMP_ID_HIF); \
        } \
    } \
}
#else
#define HIF_NRT_FREE_TX_MEM(ret_val_ptr, mem_ptr, hd_id)
#endif // ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of HIF_CFG_H */
