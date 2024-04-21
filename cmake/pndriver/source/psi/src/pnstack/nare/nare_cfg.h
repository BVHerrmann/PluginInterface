
#ifndef NARE_CFG_H                       /* ----- reinclude-protection ----- */
#define NARE_CFG_H

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
/*  F i l e               &F: nare_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for NARE:                                                  */
/*  Defines constants, types and macros for NARE.                            */
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
#ifdef NARE_MESSAGE
/*  24.06.03    JS    initial version.                                       */
/*  11.12.03    JS    default of NARE_CFG_EDD_RQB_NO_ERR changed to "undef"  */
/*                    default of NARE_CFG_MAX_USER_CHANNELS set to 2 (from 5)*/
/*  02.03.04    JS    added NARE_CFG_DCP_FALTUNGS_FACTOR                     */
/*  30.06.04    JS    added NARE_COMMON_MEM_ATTR                             */
/*  13.10.04    JS    added support of external LSA_HEADER - defines.        */
/*  28.10.04    JS    added SOCK defines.                                    */
/*  01.06.05    JS    EDD_RQB_SET/GET_STATUS replaced by SET/GET_RESPONSE.   */
/*  27.06.06    JS    added  NARE_FILE_SYSTEM_EXTENSION                      */
/*  13.03.07    JS    added  NARE_CFG_MAX_NOTIFY_RQB                         */
/*  19.06.07    JS    removed NARE_CFG_DCP_FALTUNGS_FACTOR                   */
/*  21.06.07    JS    added   NARE_CFG_CHECK_IDENTIFY_RSP_STATION_NAME       */
/*  11.12.08    JS    removed NARE_CFG_CHECK_IDENTIFY_RSP_STATION_NAME       */
#endif
/*****************************************************************************/

void  nare_psi_startstop( int start );

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* If a a memory-free call error should result in an fatal-error call        */
/* with NARE_FATAL_ERR_MEM_FREE define NARE_CFG_FREE_ERROR_IS_FATAL. If not  */
/* memory-free errors will be ignored.                                       */
/*---------------------------------------------------------------------------*/

#define NARE_CFG_FREE_ERROR_IS_FATAL

/*---------------------------------------------------------------------------*/
/* If the host byteorder is big endian define NARE_CFG_BIG_ENDIAN. If        */
/* it is little endian undef NARE_CFG_BIG_ENDIAN.                            */
/*---------------------------------------------------------------------------*/

#ifdef LSA_HOST_ENDIANESS_BIG
#define NARE_CFG_BIG_ENDIAN
#else
#undef NARE_CFG_BIG_ENDIAN
#endif

/*---------------------------------------------------------------------------*/
/* If defined, all frame-buffers needed will be allocated on opening the     */
/* EDD systemchannel. This frame-buffers will stay till closing the channel. */
/* If not defined the frame-buffers will be allocated on need and freed      */
/* if not needed anymore (e.g. on every send)                                */
/*---------------------------------------------------------------------------*/
#define NARE_CFG_EDD_ALLOC_TX_MEM_ON_OPEN

/*---------------------------------------------------------------------------*/
/* Advanced protection and function handling settings.                       */
/*                                                                           */
/* NARE is designed to use the input functions nare_timeout() and            */
/* nare_ARPResponseFrameIn() without NARE_ENTER() and NARE_EXIT() even if    */
/* called from a priority above standard handling 1). The requirement for    */
/* this is, that both functions can not be intercepted by standard handling. */
/* If this requirement is not met, additional protection with NARE_ENTER()   */
/* and NARE_EXIT() is neccessary. If so the following defines may have to    */
/* be used to include protection within this functions.                      */
/* NARE_CFG_HANDLE_DO_TIMEOUT_INTERN is a little special, because it         */
/* simplyfies some handing if no context-change is neccessary.               */
/*                                                                           */
/* NARE_CFG_USE_ENTER_EXIT_WITH_SNIFFER                                      */
/*      defined: Uses the output macros NARE_ENTER() and NARE_EXIT() even    */
/*               with the ARP-Sniffer nare_ARPResponseFrameIn(). This must be*/
/*               used if it is neccessary to protect this function if it can */
/*               be interrupted by standard-handling 1)                      */
/*      undef:   No NARE_ENTER() and NARE_EXIT() present within function     */
/*               nare_ARPResponseFrameIn(). Can be used if function is not   */
/*               intercepted by standard handling 1)                         */
/*                                                                           */
/* NARE_CFG_USE_ENTER_EXIT_TIMEOUT                                           */
/*      defined: Uses the output macros NARE_ENTER() and NARE_EXIT() even    */
/*               with the Timeout-handler nare_timeout(). This must be       */
/*               used if it is neccessary to protect this function if it can */
/*               be interrupted by standard-handling 1)                      */
/*               Note that the output macro NARE_DO_TIMER_REQUEST() is called*/
/*               within NARE_ENTER() protected code sequence.                */
/*                                                                           */
/*      undef:   No NARE_ENTER() and NARE_EXIT() present within function     */
/*               nare_timeout(). Can be used if function is not              */
/*               intercepted by standard handling 1)                         */
/*                                                                           */
/* NARE_CFG_HANDLE_DO_TIMEOUT_INTERN:                                        */
/*      defined: NARE does not call the output macro NARE_DO_TIMER_REQUEST   */
/*               within nare_timeout() but does the neccessary standard      */
/*               handling within nare_timeout(). This can be set if no       */
/*               context change is neccessary or the function is protected   */
/*               with NARE_ENTER() and NARE_EXIT().                          */
/*      undef:   NARE uses NARE_DO_TIMER_REQUEST within nare_timeout() to    */
/*               do a context change. Within nare_timeout() very little is   */
/*               done, so not protection with NARE_ENTER() and NARE_EXIT() is*/
/*               neccessary if edd_timeout() is called within a priority     */
/*               above standard-input functions.                             */
/*                                                                           */
/* Defaults:                                                                 */
/*                                                                           */
/*  #undef NARE_CFG_USE_ENTER_EXIT_WITH_SNIFFER                              */
/*  #undef NARE_CFG_USE_ENTER_EXIT_TIMEOUT                                   */
/*  #undef NARE_CFG_HANDLE_DO_TIMEOUT_INTERN                                 */
/*                                                                           */
/*                                                                           */
/* 1) Standard handling is the following:                                    */
/*    * nare_init                                                            */
/*    * nare_undo_init                                                       */
/*    * nare_dcp_request_lower_done                                          */
/*    * nare_edd_request_lower_done                                          */
/*    * nare_open_channel                                                    */
/*    * nare_close_channel                                                   */
/*    * nare_request                                                         */
/*    * part of nare_timeout() triggerd by NARE_DO_TIMER_REQUEST.            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* PSI executes everything in NARE context => no nare enter/exit needed */
#undef NARE_CFG_USE_ENTER_EXIT_WITH_SNIFFER
#undef NARE_CFG_USE_ENTER_EXIT_TIMEOUT
#undef NARE_CFG_HANDLE_DO_TIMEOUT_INTERN

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Definition of RQB-HEADER:                                                 */
/*---------------------------------------------------------------------------*/

#define NARE_RQB_HEADER LSA_RQB_HEADER(NARE_UPPER_RQB_PTR_TYPE)

/*---------------------------------------------------------------------------*/
/* Definition of RQB-TRAILER                                                 */
/*---------------------------------------------------------------------------*/

#define NARE_RQB_TRAILER  LSA_RQB_TRAILER

/*---------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define NARE_RQB_SET_NEXT_RQB_PTR(_pRQB, _Value)      LSA_RQB_SET_NEXT_RQB_PTR(_pRQB, _Value)
#define NARE_RQB_GET_NEXT_RQB_PTR(_pRQB)              LSA_RQB_GET_NEXT_RQB_PTR(_pRQB)
#define NARE_RQB_SET_PREV_RQB_PTR(_pRQB, _Value)      LSA_RQB_SET_PREV_RQB_PTR(_pRQB, _Value)
#define NARE_RQB_GET_PREV_RQB_PTR(_pRQB)              LSA_RQB_GET_PREV_RQB_PTR(_pRQB)
#define NARE_RQB_SET_OPCODE(_pRQB, _Value)            LSA_RQB_SET_OPCODE(_pRQB, _Value)
#define NARE_RQB_GET_OPCODE(_pRQB)                    LSA_RQB_GET_OPCODE(_pRQB)
#define NARE_RQB_SET_HANDLE(_pRQB, _Value)            LSA_RQB_SET_HANDLE(_pRQB, _Value)
#define NARE_RQB_GET_HANDLE(_pRQB)                    LSA_RQB_GET_HANDLE(_pRQB)
#define NARE_RQB_SET_USERID_UVAR32(_pRQB, _Value)     LSA_RQB_SET_USER_ID_UVAR32(_pRQB, _Value)
#define NARE_RQB_GET_USERID_UVAR32(_pRQB)             LSA_RQB_GET_USER_ID_UVAR32(_pRQB)
#define NARE_RQB_SET_USERID_UVAR16_1(_pRQB, _Value)   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(_pRQB, _Value)
#define NARE_RQB_GET_USERID_UVAR16_1(_pRQB)           LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(_pRQB)
#define NARE_RQB_SET_USERID_UVAR16_2(_pRQB, _Value)   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(_pRQB, _Value)
#define NARE_RQB_GET_USERID_UVAR16_2(_pRQB)           LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(_pRQB)
#define NARE_RQB_SET_RESPONSE(_pRQB, _Value)          LSA_RQB_SET_RESPONSE(_pRQB, _Value)
#define NARE_RQB_GET_RESPONSE(_pRQB)                  LSA_RQB_GET_RESPONSE(_pRQB)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to EDD                                             */
/*===========================================================================*/

#define NARE_EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)   EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define NARE_EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)   EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define NARE_EDD_RQB_SET_OPCODE(rb_, val_)         EDD_RQB_SET_OPCODE(rb_, val_)
#define NARE_EDD_RQB_SET_SERVICE(rb_, val_)        EDD_RQB_SET_SERVICE(rb_, val_)
#define NARE_EDD_RQB_SET_HANDLE(rb_, val_)         EDD_RQB_SET_HANDLE(rb_, val_)
#define NARE_EDD_RQB_SET_USERID_UVAR32(rb_, val_)  EDD_RQB_SET_USERID_UVAR32(rb_, val_)
#define NARE_EDD_RQB_SET_RESPONSE(rb_, val_)       EDD_RQB_SET_RESPONSE(rb_, val_)

#define NARE_EDD_RQB_GET_NEXT_RQB_PTR(rb_)         EDD_RQB_GET_NEXT_RQB_PTR(rb_)
#define NARE_EDD_RQB_GET_PREV_RQB_PTR(rb_)         EDD_RQB_GET_PREV_RQB_PTR(rb_)
#define NARE_EDD_RQB_GET_OPCODE(rb_)               EDD_RQB_GET_OPCODE(rb_)
#define NARE_EDD_RQB_GET_SERVICE(rb_)              EDD_RQB_GET_SERVICE(rb_)
#define NARE_EDD_RQB_GET_HANDLE(rb_)               EDD_RQB_GET_HANDLE(rb_)
#define NARE_EDD_RQB_GET_USERID_UVAR32(rb_)        EDD_RQB_GET_USERID_UVAR32(rb_)
#define NARE_EDD_RQB_GET_RESPONSE(rb_)             EDD_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to DCP                                             */
/*===========================================================================*/

#define NARE_DCP_RQB_SET_NEXT_RQB_PTR(rb_, val_)   DCP_UPPER_RQB_NEXT_SET(rb_, val_)
#define NARE_DCP_RQB_SET_PREV_RQB_PTR(rb_, val_)   DCP_UPPER_RQB_PREV_SET(rb_, val_)
#define NARE_DCP_RQB_SET_OPCODE(rb_, val_)         DCP_UPPER_RQB_OPCODE_SET(rb_, val_)
#define NARE_DCP_RQB_SET_HANDLE(rb_, val_)         DCP_UPPER_RQB_HANDLE_SET(rb_, val_)
#define NARE_DCP_RQB_SET_USERID_UVAR32(rb_, val_)  DCP_UPPER_RQB_USER32_SET(rb_, val_)

#define NARE_DCP_RQB_GET_NEXT_RQB_PTR(rb_)         DCP_UPPER_RQB_NEXT_GET(rb_)
#define NARE_DCP_RQB_GET_PREV_RQB_PTR(rb_)         DCP_UPPER_RQB_PREV_GET(rb_)
#define NARE_DCP_RQB_GET_OPCODE(rb_)               DCP_UPPER_RQB_OPCODE_GET(rb_)
#define NARE_DCP_RQB_GET_HANDLE(rb_)               DCP_UPPER_RQB_HANDLE_GET(rb_)
#define NARE_DCP_RQB_GET_USERID_UVAR32(rb_)        DCP_UPPER_RQB_USER32_GET(rb_)
#define NARE_DCP_RQB_GET_RESPONSE(rb_)             DCP_UPPER_RQB_RESPONSE_GET(rb_)

/*===========================================================================*/
/* macro name:    NARE_FILE_SYSTEM_EXTENSION(module_id_)                     */
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

#ifndef NARE_FILE_SYSTEM_EXTENSION
#define NARE_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif

/*===========================================================================*/
/* LTRC - Support (LSA-Trace)                                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define the trace mode for NARE                                            */
/* 0: no traces or external traces (default)                                 */
/* 1: enable traces and use LTRC (LSA-trace module) see nare_ltrc.h          */
/* 2: enable traces and use NARE internal trace-functions (developer only)   */
/*---------------------------------------------------------------------------*/

#define NARE_CFG_TRACE_MODE             (PSI_CFG_TRACE_MODE)


/*=============================================================================
 * If the LSA component LTRC isn't used for trace in the target system, then
 * the NARE trace macros can be defined here. On default they are empty.
 *
 * See also files nare_trc.h and lsa_cfg.h/txt.
 *
 *===========================================================================*/

/*===========================================================================*/
/* Debugging (for developer use)                                             */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define to enable special additonal assertion-checks (e.g. NULL-Ptr)       */
/* This assertion will cause an fatal-error.                                 */
/*---------------------------------------------------------------------------*/
/* #define NARE_CFG_DEBUG_ASSERT                    */

#if PSI_DEBUG
#define NARE_CFG_DEBUG_ASSERT   /* default: enabled */
#else
#undef NARE_CFG_DEBUG_ASSERT
#endif

#define NARE_CFG_TRACE_RQB_ERRORS      (PSI_CFG_NARE_CFG_TRACE_RQB_ERRORS)
/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*- alloc mem macros----------------------------------------------------------*/
#define NARE_ALLOC_UPPER_RQB_LOCAL(mem_ptr_ptr, length)                  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_NARE, PSI_MTYPE_UPPER_RQB)
#define NARE_DCP_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_DCP)
#define NARE_EDD_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_EDD)
#define NARE_DCP_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_DCP)
#define NARE_EDD_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr)  PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_EDD)
#define NARE_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)                        PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_NARE, PSI_MTYPE_LOCAL_MEM)

#define NARE_FREE_UPPER_RQB_LOCAL(ret_val_ptr, upper_rqb_ptr)            PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), 0, LSA_COMP_ID_NARE, PSI_MTYPE_UPPER_RQB)
#define NARE_DCP_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_DCP)
#define NARE_EDD_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_RQB_EDD)
#define NARE_DCP_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_DCP)
#define NARE_EDD_FREE_LOWER_MEM(ret_val_ptr, lower_rqb_ptr, sys_ptr)     PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_NARE, PSI_MTYPE_LOWER_MEM_EDD)
#define NARE_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)                  PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_NARE, PSI_MTYPE_LOCAL_MEM)

/*****************************************************************************/
/*  end of file NARE_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NARE_CFG_H */
