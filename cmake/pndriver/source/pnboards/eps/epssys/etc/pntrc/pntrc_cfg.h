#ifndef PNTRC_CFG_H
#define PNTRC_CFG_H

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
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_cfg.h.txt                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  System integration of LSA-component PNTRC                                */
/*                                                                           */
/*****************************************************************************/

/*
 * included by "pntrc_inc.h"
 */

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Max number of HD(HardwareDevices) [1..N] for one LD instance              */
/*---------------------------------------------------------------------------*/
#define PNTRC_CFG_MAX_LOWER_CPU_CNT    4

#if (EPS_PLF == EPS_PLF_PNIP_A53)
    // new time sync mode - not yet supported for HERA and TI-AM5728
    #undef PNTRC_CFG_NEW_TIMESYNC_MODE
#else
    // new time sync mode, see PNTRC_User.docx
    #define PNTRC_CFG_NEW_TIMESYNC_MODE
#endif

//lint -esym(755, PNTRC_SEND_SYNC_SCALER) global macro not referenced. It is used by PNTRC.
#define PNTRC_SEND_SYNC_SCALER	      30		/*Multiplicator for generating time sync interval from
												  thread service interval, if service interval is 100 ms,
												  then with a scaler of 30 the time sync interval is 3 s.*/

#define PNTRC_SYSTEM_ID_TYPE           LSA_UINT32

#if ( PSI_CFG_USE_PNTRC == 0 )
#define PNTRC_CFG_SUBSYSTEMS_DEFINED_EXTERNAL
#endif

/*===========================================================================*/
/*                             compiler-switches                             */
/*===========================================================================*/

#define PNTRC_DEBUG                    (PSI_DEBUG)

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

//Sys handles for user, may not be altered by PNTRC. This handle is generated by the user and passed to PNTRC. PNTRC uses this handle, when it calls user functions.
typedef LSA_UINT32 PNTRC_SYS_HANDLE_TYPE;

//Sys handle for lower CPUs, the boardnr is used for this
typedef LSA_UINT32 PNTRC_SYS_HANDLE_LOWER_TYPE;
typedef PNTRC_SYS_HANDLE_LOWER_TYPE* PNTRC_SYS_HANDLE_LOWER_PTR_TYPE;

typedef struct pntrc_init_trace_timer_tag
{
	LSA_UINT32 Resolution;
} PNTRC_INIT_TRACE_TIMER_TYPE;
typedef PNTRC_INIT_TRACE_TIMER_TYPE* PNTRC_INIT_TRACE_TIMER_PTR_TYPE;
//lint -esym(768, pntrc_init_trace_timer_tag::Resolution) global struct member '' not referenced. It is used in the advanced variants

/*===========================================================================*/
/*                                  macros                                    */
/*===========================================================================*/

LSA_VOID pntrc_startstop (LSA_INT start, LSA_VOID* pShmHw);

#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
LSA_UINT64 pntrc_get_trace_timer();
#endif

#if(EPS_PLF == EPS_PLF_WINDOWS_X86)
    #define PNTRC_OFFSET_OF(s,m) offsetof(s,m)
#else
    #define PNTRC_OFFSET_OF(s,m) (LSA_UINT32)((LSA_UINT32*)&(((s*)0x0)->m))
#endif


//lint -esym(755, PNTRC_RQB_SET_OPCODE) global macro not referenced. It is used by PNTRC.
#define PNTRC_RQB_SET_OPCODE(rb_, val_)          LSA_RQB_SET_OPCODE(rb_, val_)

//lint -esym(755, PNTRC_RQB_GET_OPCODE) global macro not referenced. It is used by PNTRC.
#define PNTRC_RQB_GET_OPCODE(rb_)                LSA_RQB_GET_OPCODE(rb_)

//lint -esym(755, PNTRC_RQB_SET_RESPONSE) global macro not referenced. It is used by PNTRC.
#define PNTRC_RQB_SET_RESPONSE(rb_, val_)        LSA_RQB_SET_RESPONSE(rb_, val_)

//lint -esym(755, PNTRC_RQB_GET_RESPONSE) global macro not referenced. It is used by PNTRC.
#define PNTRC_RQB_GET_RESPONSE(rb_)              LSA_RQB_GET_RESPONSE(rb_)

//lint -esym(755, PNTRC_RQB_GET_USER_ID) global macro not referenced. It is used by PNTRC.
#define PNTRC_RQB_GET_USER_ID(rb_)				 LSA_RQB_GET_USER_ID(rb_)


/*------------------------------------------------------------------------------
// enable / disable PNTRC_ASSERT
//----------------------------------------------------------------------------*/

#if PNTRC_DEBUG

	/* check for condition: internal programming error */
# define PNTRC_ASSERT(cond) if (! (cond)) PNTRC_FATAL (0)

#else

	/* no more programming error exists :-) */
# define PNTRC_ASSERT(cond)

#endif

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef PNTRC_FILE_SYSTEM_EXTENSION
# define PNTRC_FILE_SYSTEM_EXTENSION(module_id_)
#endif


/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 .. no LSA trace
//	1 .. LSA trace [default]
//	2 .. LSA index trace
//----------------------------------------------------------------------------*/
//lint -esym(755, PNTRC_CFG_TRACE_MODE) global macro not referenced. It is used by PNTRC.
#define PNTRC_CFG_TRACE_MODE  			(PSI_CFG_TRACE_MODE)

//lint --e(537) Repeated include file. Intended since the files may also include eps_mem.h through other headers. eps_mem_alloc is needed here.
#include "eps_mem.h"
#include "eps_trc.h"
// Inline implementation of PNTRC_ALLOC_LOCAL_MEM
#define PNTRC_ALLOC_LOCAL_MEM(local_mem_ptr_ptr,length) \
    {\
        *(local_mem_ptr_ptr) = eps_mem_alloc((length), LSA_COMP_ID_LTRC, 0); \
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "PNTRC_ALLOC_LOCAL_MEM, local_mem_ptr(0x%08x)", *(local_mem_ptr_ptr)); \
    }

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNTRC_CFG_H */