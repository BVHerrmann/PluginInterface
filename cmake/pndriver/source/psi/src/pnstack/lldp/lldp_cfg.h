
#ifndef LLDP_CFG_H                       /* ----- reinclude-protection ----- */
#define LLDP_CFG_H

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
/*  F i l e               &F: lldp_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for LLDP:                                                  */
/*  Defines constants, types and macros for LLDP.                            */
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
#ifdef LLDP_MESSAGE
/*  05.04.04    JS    initial version.                                       */
#endif
/*****************************************************************************/

void lldp_psi_startstop( int start );

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Set some local_mem static or dynamic:                                     */
/* If LLDP_CFG_LOCAL_MEM_STATIC is not defined, all local-memory will be     */
/* allocated dynamically. If defined, some local memory is static.           */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_LOCAL_MEM_STATIC

/*---------------------------------------------------------------------------*/
/* If defined, all frame-buffers needed will be allocated on opening the     */
/* EDD systemchannel. This frame-buffers will stay till closing the channel. */
/* If not defined the frame-buffers will be allocated on need and freed      */
/* if not needed anymore (e.g. on every send)                                */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_EDD_ALLOC_TX_MEM_ON_OPEN

/*---------------------------------------------------------------------------*/
/* Number of Receive Resources used per Port. This defines the number of     */
/* EDD- Receive-Requests used per EDD-Channel. The number of Requests is     */
/* LLDP_CFG_RECV_RESOURCE_PER_PORT * PortCnt.                                */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_RECV_RESOURCE_PER_PORT   2

/*---------------------------------------------------------------------------*/
/* Defines if we should send a Shutdown-frame if stopping a started port.    */
/* If defined we send a shutdown, if not we dont.                            */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_SEND_SHUTDOWN_ON_STOP

/*---------------------------------------------------------------------------*/
/* Advanced protection and function handling settings.                       */
/*                                                                           */
/* LLDP is designed to use the input functions lldp_timeout() without        */
/* LLDP_ENTER() and LLDP_EXIT() even if called from a priority above standard*/
/* handling 1). The requirement for this is, that lldp_timeout() can not be  */
/* intercepted by standard handling.                                         */
/* If this requirement is not met, additional protection with LLDP_ENTER()   */
/* and LLDP_EXIT() is neccessary. If so the following defines may have to    */
/* be used to include protection within this function.                       */
/* LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN is a little special, because it         */
/* simplyfies some handing if no context-change is neccessary.               */
/*                                                                           */
/* LLDP_CFG_USE_ENTER_EXIT_TIMEOUT                                           */
/*      defined: Uses the output macros LLDP_ENTER() and LLDP_EXIT() even    */
/*               with the timeout-handler lldp_timeout(). This must be       */
/*               used if it is neccessary to protect this function if it can */
/*               be interrupted by standard-handling 1)                      */
/*               Note that the output macro LLDP_DO_TIMER_REQUEST() is called*/
/*               within LLDP_ENTER() protected code sequence.                */
/*                                                                           */
/*      undef:   No LLDP_ENTER() and LLDP_EXIT() present within function     */
/*               lldp_timeout(). Can be used if function is not              */
/*               intercepted by standard handling 1)                         */
/*                                                                           */
/* LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN:                                        */
/*      defined: LLDP does not call the output macro LLDP_DO_TIMER_REQUEST   */
/*               within lldp_timeout() but does the neccessary standard      */
/*               handling within lldp_timeout(). This can be set if no       */
/*               context change is neccessary or the function is protected   */
/*               with LLDP_ENTER() and LLDP_EXIT().                          */
/*      undef:   LLDP uses LLDP_DO_TIMER_REQUEST within lldp_timeout() to    */
/*               trigger a context change. Within lldp_timeout() very little */
/*               is done, so no protection with LLDP_ENTER() and LLDP_EXIT() */
/*               is neccessary if lldp_timeout() is called within a priority */
/*               above standard-input functions.                             */
/*                                                                           */
/* Defaults:                                                                 */
/*                                                                           */
/*  #undef LLDP_CFG_USE_ENTER_EXIT_TIMEOUT                                   */
/*  #undef LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN                                 */
/*                                                                           */
/*                                                                           */
/* 1) Standard handling is the following:                                    */
/*    * lldp_init                                                            */
/*    * lldp_undo_init                                                       */
/*    * lldp_edd_request_lower_done                                          */
/*    * lldp_open_channel                                                    */
/*    * lldp_close_channel                                                   */
/*    * lldp_request                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#undef LLDP_CFG_USE_ENTER_EXIT_TIMEOUT
#undef LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN

/*---------------------------------------------------------------------------*/
/* Definition of RQB-HEADER:                                                 */
/*---------------------------------------------------------------------------*/

#define LLDP_RQB_HEADER   LSA_RQB_HEADER(LLDP_UPPER_RQB_PTR_TYPE)

/*---------------------------------------------------------------------------*/
/* Definition of RQB-TRAILER                                                 */
/*                                                                           */
/* If necessary during system integration, additional parameters can be      */
/* added!                                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define LLDP_RQB_TRAILER   LSA_RQB_TRAILER

/*---------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define LLDP_RQB_SET_NEXT_RQB_PTR(_pRQB, _Value)    LSA_RQB_SET_NEXT_RQB_PTR(_pRQB, _Value)
#define LLDP_RQB_GET_NEXT_RQB_PTR(_pRQB)            LSA_RQB_GET_NEXT_RQB_PTR(_pRQB)
#define LLDP_RQB_SET_PREV_RQB_PTR(_pRQB, _Value)    LSA_RQB_SET_PREV_RQB_PTR(_pRQB, _Value)
#define LLDP_RQB_GET_PREV_RQB_PTR(_pRQB)            LSA_RQB_GET_PREV_RQB_PTR(_pRQB)
#define LLDP_RQB_SET_OPCODE(_pRQB, _Value)          LSA_RQB_SET_OPCODE(_pRQB, _Value)
#define LLDP_RQB_GET_OPCODE(_pRQB)                  LSA_RQB_GET_OPCODE(_pRQB)
#define LLDP_RQB_SET_HANDLE(_pRQB, _Value)          LSA_RQB_SET_HANDLE(_pRQB, _Value)
#define LLDP_RQB_GET_HANDLE(_pRQB)                  LSA_RQB_GET_HANDLE(_pRQB)
#define LLDP_RQB_SET_USERID_UVAR32(_pRQB, _Value)   LSA_RQB_SET_USER_ID_UVAR32(_pRQB, _Value)
#define LLDP_RQB_GET_USERID_UVAR32(_pRQB)           LSA_RQB_GET_USER_ID_UVAR32(_pRQB)
#define LLDP_RQB_SET_USERID_UVAR16_1(_pRQB, _Value) LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW(_pRQB, _Value)
#define LLDP_RQB_GET_USERID_UVAR16_1(_pRQB)         LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW(_pRQB)
#define LLDP_RQB_SET_USERID_UVAR16_2(_pRQB, _Value) LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH(_pRQB, _Value)
#define LLDP_RQB_GET_USERID_UVAR16_2(_pRQB)         LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH(_pRQB)
#define LLDP_RQB_SET_RESPONSE(_pRQB, _Value)        LSA_RQB_SET_RESPONSE(_pRQB, _Value)
#define LLDP_RQB_GET_RESPONSE(_pRQB)                LSA_RQB_GET_RESPONSE(_pRQB)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to EDD                                             */
/*===========================================================================*/

#define LLDP_EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)   EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define LLDP_EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)   EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define LLDP_EDD_RQB_SET_OPCODE(rb_, val_)         EDD_RQB_SET_OPCODE(rb_, val_)
#define LLDP_EDD_RQB_SET_SERVICE(rb_, val_)        EDD_RQB_SET_SERVICE(rb_, val_)
#define LLDP_EDD_RQB_SET_HANDLE(rb_, val_)         EDD_RQB_SET_HANDLE(rb_, val_)
#define LLDP_EDD_RQB_SET_USERID_UVAR32(rb_, val_)  EDD_RQB_SET_USERID_UVAR32(rb_, val_)
#define LLDP_EDD_RQB_SET_RESPONSE(rb_, val_)       EDD_RQB_SET_RESPONSE(rb_, val_)

#define LLDP_EDD_RQB_GET_NEXT_RQB_PTR(rb_)         EDD_RQB_GET_NEXT_RQB_PTR(rb_)
#define LLDP_EDD_RQB_GET_PREV_RQB_PTR(rb_)         EDD_RQB_GET_PREV_RQB_PTR(rb_)
#define LLDP_EDD_RQB_GET_OPCODE(rb_)               EDD_RQB_GET_OPCODE(rb_)
#define LLDP_EDD_RQB_GET_SERVICE(rb_)              EDD_RQB_GET_SERVICE(rb_)
#define LLDP_EDD_RQB_GET_HANDLE(rb_)               EDD_RQB_GET_HANDLE(rb_)
#define LLDP_EDD_RQB_GET_USERID_UVAR32(rb_)        EDD_RQB_GET_USERID_UVAR32(rb_)
#define LLDP_EDD_RQB_GET_RESPONSE(rb_)             EDD_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* macro name:    LLDP_FILE_SYSTEM_EXTENSION(module_id_)                     */
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

#ifndef LLDP_FILE_SYSTEM_EXTENSION
#define LLDP_FILE_SYSTEM_EXTENSION(module_id_) /* Currently not supported */
#endif

/*===========================================================================*/
/* LTRC - Support (LSA-Trace)                                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define the Trace mode for LLDP                                            */
/* 0: no traces or external traces (default)                                 */
/* 1: enable Traces and use LTRC (LSA-Trace module) see LLDP_ltrc.h          */
/* 2: enables full index trace  (LSA-index Trace module)                     */
/*---------------------------------------------------------------------------*/

#define LLDP_CFG_TRACE_MODE   (PSI_CFG_TRACE_MODE)

/*===========================================================================*/
/* Debugging (for developer use)                                             */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define to enable spezial additonal assertion-checks (e.g. NULL-Ptr)       */
/* This assertion will cause an fatal-error.                                 */
/*---------------------------------------------------------------------------*/

#if PSI_DEBUG /* 0 or 1 (not undef/def) */
#define LLDP_CFG_DEBUG_ASSERT
#else
#undef LLDP_CFG_DEBUG_ASSERT
#endif

/*- alloc mem macros----------------------------------------------------------*/
#define LLDP_ALLOC_UPPER_RQB_LOCAL(mem_ptr_ptr, length)                 PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_LLDP, PSI_MTYPE_UPPER_RQB)
#define LLDP_EDD_ALLOC_LOWER_RQB(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_RQB_EDD)
#define LLDP_EDD_ALLOC_LOWER_MEM(mem_ptr_ptr, user_id, length, sys_ptr) PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), (user_id), (length), (sys_ptr), LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_MEM_EDD)
#define LLDP_ALLOC_LOCAL_MEM(mem_ptr_ptr, length)                       PSI_ALLOC_LOCAL_MEM((mem_ptr_ptr), 0, (length), 0, LSA_COMP_ID_LLDP, PSI_MTYPE_LOCAL_MEM)

#define LLDP_FREE_UPPER_RQB_LOCAL(ret_val_ptr, upper_rqb_ptr)           PSI_FREE_LOCAL_MEM((ret_val_ptr), (upper_rqb_ptr), 0, LSA_COMP_ID_LLDP, PSI_MTYPE_UPPER_RQB)
#define LLDP_EDD_FREE_LOWER_RQB(ret_val_ptr, lower_rqb_ptr, sys_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_rqb_ptr), (sys_ptr), LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_RQB_EDD)
#define LLDP_EDD_FREE_LOWER_MEM(ret_val_ptr, lower_mem_ptr, sys_ptr)    PSI_FREE_LOCAL_MEM((ret_val_ptr), (lower_mem_ptr), (sys_ptr), LSA_COMP_ID_LLDP, PSI_MTYPE_LOWER_MEM_EDD)
#define LLDP_FREE_LOCAL_MEM(ret_val_ptr, local_mem_ptr)                 PSI_FREE_LOCAL_MEM((ret_val_ptr), (local_mem_ptr), 0, LSA_COMP_ID_LLDP, PSI_MTYPE_LOCAL_MEM)


/*****************************************************************************/
/*  end of file LLDP_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of LLDP_CFG_H */
