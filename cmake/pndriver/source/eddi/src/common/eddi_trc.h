#ifndef EDDI_TRC_H              //reinclude-protection
#define EDDI_TRC_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_trc.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface for LSA-trace                                           */
/*  Defines LSA-trace macros for EDDI.                                       */
/*                                                                           */
/*      Here the macros are adapted to the LSA component LTRC.               */
/*                                                                           */
/*      If LTRC shall not be used, then either the LSA_TRACE macros used     */
/*      here must be defined by the system in the file lsa_cfg.h/txt or      */
/*      directly the EDDI-trace macro must be redefined on the desired       */
/*      function/macro in the eddi_cfg.h/txt file.                           */
/*      If the system isn't able to store trace data then an "empty          */
/*      definition" must be implemented in file eddi_cfg.h/txt (or the       */
/*      LSA_TRACE macros must be defined "empty" in lsa_cfg.h/txt).          */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/* Usage of traces within EDDI: (have to be defines by LTRC or System!)      */
/*                                                                           */
/* Trace-Subsystems used:                                                    */
/*                                                                           */
/* TRACE_SUBSYS_EDDI_UPPER    : EDDI Core Upper-Interface calls/response     */
/*                              i.e. eddi_request, eddi_system,              */
/*                              low-level functions                          */
/* TRACE_SUBSYS_EDDI_LOWER    : EDDI Core Interrupts                         */
/* TRACE_SUBSYS_EDDI_SYSTEM   : not used                                     */
/* TRACE_SUBSYS_EDDI_FUNCTION : EDDI Core Function In/Out-Calls (general     */
/*                              part of eddi) - rest                         */
/*                                                                           */
/* TRACE_SUBSYS_EDDI_NRT      : EDDI Program flow and errors of NRT Modul    */
/* TRACE_SUBSYS_EDDI_PRM      : EDDI Program flow and errors of PRM Modul    */
/* TRACE_SUBSYS_EDDI_SWI      : EDDI Program flow and errors of Switch Modul */
/* TRACE_SUBSYS_EDDI_CRT      : EDDI Program flow and errors of CRT Modul    */
/* TRACE_SUBSYS_EDDI_SYNC     : EDDI Program flow and errors of SYNC Modul   */
/*                                                                           */
/* TRACE_SUBSYS_EDDI_PROGRAM  : EDDI Core Programm flow and errors           */
/*                              (general part of EDDI) - rest                */
/*                                                                           */
/* Trace-Level usage:                                                        */
/*                                                                           */
/* LSA_TRACE_LEVEL_FATAL      : Fatal errors                                 */
/* LSA_TRACE_LEVEL_ERROR      : Errors                                       */
/* LSA_TRACE_LEVEL_UNEXP      : not used                                     */
/* LSA_TRACE_LEVEL_NOTE_HIGH  : Higher important                             */
/* LSA_TRACE_LEVEL_NOTE       : Normal                                       */
/* LSA_TRACE_LEVEL_NOTE_LOW   : function calls                               */
/* LSA_TRACE_LEVEL_CHAT       : cyclic output                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* Notes:                                                                    */
/*                                                                           */
/* With PRM / NRT / SWI / SYNC / CRT Subsystems the EDDI uses the trace level*/
/* to differ function calls, cyclic output, errors and other notes           */
/* level for trace output:                                                   */
/*                                                                           */
/* LSA_TRACE_LEVEL_FATAL      : not used                                     */
/* LSA_TRACE_LEVEL_ERROR      : Errors                                       */
/* LSA_TRACE_LEVEL_UNEXP      : not used                                     */
/* LSA_TRACE_LEVEL_NOTE_HIGH  : user defined notes (high prio)               */
/* LSA_TRACE_LEVEL_NOTE       : user defined notes (low / normal prio)       */
/* LSA_TRACE_LEVEL_NOTE_LOW   : function calls / call stack                  */
/* LSA_TRACE_LEVEL_CHAT       : only for cyclic functions and output         */
/*                              Causing much traces!                         */
/*                                                                           */
/* With FUNCTION Subsystems the EDDI uses the trace level to differ function */
/* level for trace output:                                                   */
/*                                                                           */
/* LSA_TRACE_LEVEL_NOTE_HIGH  : High level functions                         */
/* LSA_TRACE_LEVEL_NOTE       : Normal functions                             */
/* LSA_TRACE_LEVEL_NOTE_LOW   : low prio functions                           */
/* LSA_TRACE_LEVEL_CHAT       : cyclic called functions.                     */
/*                              Causing much traces!                         */
/*                                                                           */
/*===========================================================================*/

/*===========================================================================*/
/* TRACE-Referenzes have to be defined by Systemadaption.                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* system interface output macros
 *
 * Note: There aren't any prototypes for these macros so that the target system
 * defines the pointer size and not the EDDI. EDDI doesn't cast the
 * parameters at the call.
 * A function can be nevertheless invoked within the macro.
 *
 * The subsytem YYYYY (see below) must be defined by the LSA component
 * developer. It can be =
 *    [empty]:   If this component uses only one subsystem.
 *    UPPER:     All requirements of the upper user are written into this
 *               subsystem.
 *               Calls of the user are marked with                   >>>
 *               Confirmations to the user are marked with           <<<
 *    LOWER:     All Interrupts are written into this   subsystem.
 *               Requests to the lower layer are marked with         >>>
 *               Confirmations from the lower layer are marked with  <<<
 *    MEMORY:    All memory requests for RQBs, data buffer, TA-Buffer, ...
 *               are written into this subsystem. Conversions of UPPER_TO_LOWER
 *               and reversed also belong to this subsystem.
 *    SYSTEM:    All other requirements to / from the system are written into
 *               this subsystem. For example: Timers, path information, copying
 *               processes of COMMON memory, ....
 *               For functions which the system invokes:
 *               Calls of the system are marked with                 -->
 *               The end of the functions are marked with            <--
 *    FUNCTION:  (Internal) function call trace.
 *
 *    If the component developer wants to make trace entries of  other things
 *    than above mentioned, he can define other subsystems.
 */

/*=============================================================================
 * macro names:   EDDI_YYYYY_TRACE_XX  (XX = 00, 01, 02, 03, ..., 16)
 *
 * macros:        Trace entry of 0, 1, 2, ... or 16 parameters for the
 *                subsystem "YYYYY".
 *
 *                The system integrator must define the identifier
 *                TRACE_SUBSYS_EDDI_YYYYY (if LTRC is used in the file
 *                ltrc_sub.h/txt or if LTRC isn't used in a system file)!
 *
 * parameters:    level_:   LSA_TRACE_LEVEL_OFF,   LSA_TRACE_LEVEL_FATAL,
 *                          LSA_TRACE_LEVEL_ERROR, LSA_TRACE_LEVEL_UNEXP,
 *                          LSA_TRACE_LEVEL_WARN,  LSA_TRACE_LEVEL_NOTE_HIGH,
 *                          LSA_TRACE_LEVEL_NOTE,  LSA_TRACE_LEVEL_NOTE_LOW or
 *                          LSA_TRACE_LEVEL_CHAT
 *                msg_:     string similar to "printf"
 *                para1_:   parameter 1
 *                para2_:   parameter 2
 *                 ...        ...
 *                para16_:  paramater 16
 *
 * return value:  LSA_VOID
 *===========================================================================*/

/*****************************************************************************/

#if !defined (EDDI_CFG_TRACE_MODE)
#error "EDDI_CFG_TRACE_MODE not defined in eddi_cfg.h!"
#elif (EDDI_CFG_TRACE_MODE != 0) && (EDDI_CFG_TRACE_MODE != 1) && (EDDI_CFG_TRACE_MODE != 2)
#error "EDDI_CFG_TRACE_MODE defined wrong in eddi_cfg.h!"
#endif  

/*****************************************************************************/

#if (EDDI_CFG_TRACE_MODE == 0) //no Trace

/*****************************************************************************/

//EDDI_UPPER_TRACE
#define EDDI_UPPER_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_UPPER_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_LOWER_TRACE
#define EDDI_LOWER_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_LOWER_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_SYSTEM_TRACE
#define EDDI_SYSTEM_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYSTEM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_FUNCTION_TRACE
#define EDDI_FUNCTION_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_FUNCTION_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_NRT_TRACE
#define EDDI_NRT_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_NRT_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_PRM_TRACE
#define EDDI_PRM_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PRM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_SWI_TRACE
#define EDDI_SWI_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SWI_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_CRT_TRACE
#define EDDI_CRT_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_CRT_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_SYNC_TRACE
#define EDDI_SYNC_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_SYNC_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

//EDDI_PROGRAM_TRACE
#define EDDI_PROGRAM_TRACE_STRING(idx_, level_, msg_, st_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_00(idx_, level_, msg_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_01(idx_, level_, msg_, para1_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15)\
    LSA_UNUSED_ARG(idx_)
#define EDDI_PROGRAM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    LSA_UNUSED_ARG(idx_)

/*****************************************************************************/

#elif (EDDI_CFG_TRACE_MODE == 1) //LSA-Trace 

/*****************************************************************************/

//EDDI_UPPER_TRACE
#ifndef EDDI_UPPER_TRACE_00
#define EDDI_UPPER_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_UPPER, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_01
#define EDDI_UPPER_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_02
#define EDDI_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_03
#define EDDI_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_04
#define EDDI_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_05
#define EDDI_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_06
#define EDDI_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_07
#define EDDI_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_08
#define EDDI_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_09
#define EDDI_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_10
#define EDDI_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_11
#define EDDI_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_12
#define EDDI_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_13
#define EDDI_UPPER_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_14
#define EDDI_UPPER_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_15
#define EDDI_UPPER_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_UPPER_TRACE_16
#define EDDI_UPPER_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_UPPER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_LOWER_TRACE
#ifndef EDDI_LOWER_TRACE_00
#define EDDI_LOWER_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_LOWER, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_01
#define EDDI_LOWER_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_02
#define EDDI_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_03
#define EDDI_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_04
#define EDDI_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_05
#define EDDI_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_06
#define EDDI_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_07
#define EDDI_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_08
#define EDDI_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_09
#define EDDI_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_10
#define EDDI_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_11
#define EDDI_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_12
#define EDDI_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_13
#define EDDI_LOWER_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_14
#define EDDI_LOWER_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_15
#define EDDI_LOWER_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_LOWER_TRACE_16
#define EDDI_LOWER_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_LOWER, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_SYSTEM_TRACE
#ifndef EDDI_SYSTEM_TRACE_00
#define EDDI_SYSTEM_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_01
#define EDDI_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_02
#define EDDI_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_03
#define EDDI_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_04
#define EDDI_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_05
#define EDDI_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_06
#define EDDI_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_07
#define EDDI_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_08
#define EDDI_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_09
#define EDDI_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_10
#define EDDI_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_11
#define EDDI_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_12
#define EDDI_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_13
#define EDDI_SYSTEM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_14
#define EDDI_SYSTEM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_15
#define EDDI_SYSTEM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYSTEM_TRACE_16
#define EDDI_SYSTEM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_SYSTEM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_FUNCTION_TRACE
#ifndef EDDI_FUNCTION_TRACE_00
#define EDDI_FUNCTION_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_01
#define EDDI_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_02
#define EDDI_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_03
#define EDDI_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_04
#define EDDI_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_05
#define EDDI_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_06
#define EDDI_FUNCTION_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_07
#define EDDI_FUNCTION_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_08
#define EDDI_FUNCTION_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_09
#define EDDI_FUNCTION_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_10
#define EDDI_FUNCTION_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_11
#define EDDI_FUNCTION_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_12
#define EDDI_FUNCTION_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_13
#define EDDI_FUNCTION_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_14
#define EDDI_FUNCTION_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_15
#define EDDI_FUNCTION_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_FUNCTION_TRACE_16
#define EDDI_FUNCTION_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_FUNCTION, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_NRT_TRACE
#ifndef EDDI_NRT_TRACE_00
#define EDDI_NRT_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_NRT, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_01
#define EDDI_NRT_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_02
#define EDDI_NRT_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_03
#define EDDI_NRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_04
#define EDDI_NRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_05
#define EDDI_NRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_06
#define EDDI_NRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_07
#define EDDI_NRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_08
#define EDDI_NRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_09
#define EDDI_NRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_10
#define EDDI_NRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_11
#define EDDI_NRT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_12
#define EDDI_NRT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_13
#define EDDI_NRT_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_14
#define EDDI_NRT_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_15
#define EDDI_NRT_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_NRT_TRACE_16
#define EDDI_NRT_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_NRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_PRM_TRACE
#ifndef EDDI_PRM_TRACE_00
#define EDDI_PRM_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_PRM, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_01
#define EDDI_PRM_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_02
#define EDDI_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_03
#define EDDI_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_04
#define EDDI_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_05
#define EDDI_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_06
#define EDDI_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_07
#define EDDI_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_08
#define EDDI_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_09
#define EDDI_PRM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_10
#define EDDI_PRM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_11
#define EDDI_PRM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_12
#define EDDI_PRM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_13
#define EDDI_PRM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_14
#define EDDI_PRM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_15
#define EDDI_PRM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PRM_TRACE_16
#define EDDI_PRM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_PRM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_SWI_TRACE
#ifndef EDDI_SWI_TRACE_00
#define EDDI_SWI_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_SWI, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_01
#define EDDI_SWI_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_02
#define EDDI_SWI_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_03
#define EDDI_SWI_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_04
#define EDDI_SWI_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_05
#define EDDI_SWI_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_06
#define EDDI_SWI_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_07
#define EDDI_SWI_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_08
#define EDDI_SWI_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_09
#define EDDI_SWI_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_10
#define EDDI_SWI_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_11
#define EDDI_SWI_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_12
#define EDDI_SWI_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_13
#define EDDI_SWI_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_14
#define EDDI_SWI_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_15
#define EDDI_SWI_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SWI_TRACE_16
#define EDDI_SWI_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_SWI, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_CRT_TRACE
#ifndef EDDI_CRT_TRACE_00
#define EDDI_CRT_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_CRT, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_01
#define EDDI_CRT_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_02
#define EDDI_CRT_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_03
#define EDDI_CRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_04
#define EDDI_CRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_05
#define EDDI_CRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_06
#define EDDI_CRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_07
#define EDDI_CRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_08
#define EDDI_CRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_09
#define EDDI_CRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_10
#define EDDI_CRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_11
#define EDDI_CRT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_12
#define EDDI_CRT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_13
#define EDDI_CRT_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_14
#define EDDI_CRT_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_15
#define EDDI_CRT_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_CRT_TRACE_16
#define EDDI_CRT_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_CRT, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_SYNC_TRACE
#ifndef EDDI_SYNC_TRACE_00
#define EDDI_SYNC_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_SYNC, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_01
#define EDDI_SYNC_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_02
#define EDDI_SYNC_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_03
#define EDDI_SYNC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_04
#define EDDI_SYNC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_05
#define EDDI_SYNC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_06
#define EDDI_SYNC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_07
#define EDDI_SYNC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_08
#define EDDI_SYNC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_09
#define EDDI_SYNC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_10
#define EDDI_SYNC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_11
#define EDDI_SYNC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_12
#define EDDI_SYNC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_13
#define EDDI_SYNC_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_14
#define EDDI_SYNC_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_15
#define EDDI_SYNC_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_SYNC_TRACE_16
#define EDDI_SYNC_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_SYNC, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

//EDDI_PROGRAM_TRACE
#ifndef EDDI_PROGRAM_TRACE_STRING
#define EDDI_PROGRAM_TRACE_STRING(idx_, level_, msg_, st_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_00
#define EDDI_PROGRAM_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_00(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_);\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_01
#define EDDI_PROGRAM_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_01(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_02
#define EDDI_PROGRAM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_02(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_03
#define EDDI_PROGRAM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_03(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_04
#define EDDI_PROGRAM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_04(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_05
#define EDDI_PROGRAM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_05(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_06
#define EDDI_PROGRAM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_06(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_07
#define EDDI_PROGRAM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_07(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_08
#define EDDI_PROGRAM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_08(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_09
#define EDDI_PROGRAM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_09(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_10
#define EDDI_PROGRAM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_10(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_11
#define EDDI_PROGRAM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_11(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_12
#define EDDI_PROGRAM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_12(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_13
#define EDDI_PROGRAM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_13(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_14
#define EDDI_PROGRAM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_14(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_15
#define EDDI_PROGRAM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_15(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_));\
    LSA_UNUSED_ARG(idx_)
#endif
#ifndef EDDI_PROGRAM_TRACE_16
#define EDDI_PROGRAM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_TRACE_16(TRACE_SUBSYS_EDDI_PROGRAM, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_));\
    LSA_UNUSED_ARG(idx_)
#endif

/*****************************************************************************/

#elif (EDDI_CFG_TRACE_MODE == 2) //LSA-IDX-Trace

/*****************************************************************************/

//EDDI_UPPER_TRACE
#ifndef EDDI_UPPER_TRACE_00
#define EDDI_UPPER_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_)
#endif
#ifndef EDDI_UPPER_TRACE_01
#define EDDI_UPPER_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_UPPER_TRACE_02
#define EDDI_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_UPPER_TRACE_03
#define EDDI_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_UPPER_TRACE_04
#define EDDI_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_UPPER_TRACE_05
#define EDDI_UPPER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_UPPER_TRACE_06
#define EDDI_UPPER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_UPPER_TRACE_07
#define EDDI_UPPER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_UPPER_TRACE_08
#define EDDI_UPPER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_UPPER_TRACE_09
#define EDDI_UPPER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_UPPER_TRACE_10
#define EDDI_UPPER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_UPPER_TRACE_11
#define EDDI_UPPER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_UPPER_TRACE_12
#define EDDI_UPPER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_UPPER_TRACE_13
#define EDDI_UPPER_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_UPPER_TRACE_14
#define EDDI_UPPER_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_UPPER_TRACE_15
#define EDDI_UPPER_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_UPPER_TRACE_16
#define EDDI_UPPER_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_UPPER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_LOWER_TRACE
#ifndef EDDI_LOWER_TRACE_00
#define EDDI_LOWER_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_)
#endif
#ifndef EDDI_LOWER_TRACE_01
#define EDDI_LOWER_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_LOWER_TRACE_02
#define EDDI_LOWER_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_LOWER_TRACE_03
#define EDDI_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_LOWER_TRACE_04
#define EDDI_LOWER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_LOWER_TRACE_05
#define EDDI_LOWER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_LOWER_TRACE_06
#define EDDI_LOWER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_LOWER_TRACE_07
#define EDDI_LOWER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_LOWER_TRACE_08
#define EDDI_LOWER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_LOWER_TRACE_09
#define EDDI_LOWER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_LOWER_TRACE_10
#define EDDI_LOWER_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_LOWER_TRACE_11
#define EDDI_LOWER_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_LOWER_TRACE_12
#define EDDI_LOWER_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_LOWER_TRACE_13
#define EDDI_LOWER_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_LOWER_TRACE_14
#define EDDI_LOWER_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_LOWER_TRACE_15
#define EDDI_LOWER_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_LOWER_TRACE_16
#define EDDI_LOWER_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_LOWER, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_SYSTEM_TRACE
#ifndef EDDI_SYSTEM_TRACE_00
#define EDDI_SYSTEM_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_)
#endif
#ifndef EDDI_SYSTEM_TRACE_01
#define EDDI_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_SYSTEM_TRACE_02
#define EDDI_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_SYSTEM_TRACE_03
#define EDDI_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_SYSTEM_TRACE_04
#define EDDI_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_SYSTEM_TRACE_05
#define EDDI_SYSTEM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_SYSTEM_TRACE_06
#define EDDI_SYSTEM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_SYSTEM_TRACE_07
#define EDDI_SYSTEM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_SYSTEM_TRACE_08
#define EDDI_SYSTEM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_SYSTEM_TRACE_09
#define EDDI_SYSTEM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_SYSTEM_TRACE_10
#define EDDI_SYSTEM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_SYSTEM_TRACE_11
#define EDDI_SYSTEM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_SYSTEM_TRACE_12
#define EDDI_SYSTEM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_SYSTEM_TRACE_13
#define EDDI_SYSTEM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_SYSTEM_TRACE_14
#define EDDI_SYSTEM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_SYSTEM_TRACE_15
#define EDDI_SYSTEM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_SYSTEM_TRACE_16
#define EDDI_SYSTEM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_SYSTEM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_FUNCTION_TRACE
#ifndef EDDI_FUNCTION_TRACE_00
#define EDDI_FUNCTION_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_)
#endif
#ifndef EDDI_FUNCTION_TRACE_01
#define EDDI_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_FUNCTION_TRACE_02
#define EDDI_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_FUNCTION_TRACE_03
#define EDDI_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_FUNCTION_TRACE_04
#define EDDI_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_FUNCTION_TRACE_05
#define EDDI_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_FUNCTION_TRACE_06
#define EDDI_FUNCTION_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_FUNCTION_TRACE_07
#define EDDI_FUNCTION_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_FUNCTION_TRACE_08
#define EDDI_FUNCTION_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_FUNCTION_TRACE_09
#define EDDI_FUNCTION_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_FUNCTION_TRACE_10
#define EDDI_FUNCTION_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_FUNCTION_TRACE_11
#define EDDI_FUNCTION_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_FUNCTION_TRACE_12
#define EDDI_FUNCTION_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_FUNCTION_TRACE_13
#define EDDI_FUNCTION_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_FUNCTION_TRACE_14
#define EDDI_FUNCTION_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_FUNCTION_TRACE_15
#define EDDI_FUNCTION_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_FUNCTION_TRACE_16
#define EDDI_FUNCTION_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_FUNCTION, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_NRT_TRACE
#ifndef EDDI_NRT_TRACE_00
#define EDDI_NRT_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_)
#endif
#ifndef EDDI_NRT_TRACE_01
#define EDDI_NRT_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_NRT_TRACE_02
#define EDDI_NRT_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_NRT_TRACE_03
#define EDDI_NRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_NRT_TRACE_04
#define EDDI_NRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_NRT_TRACE_05
#define EDDI_NRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_NRT_TRACE_06
#define EDDI_NRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_NRT_TRACE_07
#define EDDI_NRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_NRT_TRACE_08
#define EDDI_NRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_NRT_TRACE_09
#define EDDI_NRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_NRT_TRACE_10
#define EDDI_NRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_NRT_TRACE_11
#define EDDI_NRT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_NRT_TRACE_12
#define EDDI_NRT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_NRT_TRACE_13
#define EDDI_NRT_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_NRT_TRACE_14
#define EDDI_NRT_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_NRT_TRACE_15
#define EDDI_NRT_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_NRT_TRACE_16
#define EDDI_NRT_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_NRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_PRM_TRACE
#ifndef EDDI_PRM_TRACE_00
#define EDDI_PRM_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_)
#endif
#ifndef EDDI_PRM_TRACE_01
#define EDDI_PRM_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_PRM_TRACE_02
#define EDDI_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_PRM_TRACE_03
#define EDDI_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_PRM_TRACE_04
#define EDDI_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_PRM_TRACE_05
#define EDDI_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_PRM_TRACE_06
#define EDDI_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_PRM_TRACE_07
#define EDDI_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_PRM_TRACE_08
#define EDDI_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_PRM_TRACE_09
#define EDDI_PRM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_PRM_TRACE_10
#define EDDI_PRM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_PRM_TRACE_11
#define EDDI_PRM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_PRM_TRACE_12
#define EDDI_PRM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_PRM_TRACE_13
#define EDDI_PRM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_PRM_TRACE_14
#define EDDI_PRM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_PRM_TRACE_15
#define EDDI_PRM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_PRM_TRACE_16
#define EDDI_PRM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_PRM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_SWI_TRACE
#ifndef EDDI_SWI_TRACE_00
#define EDDI_SWI_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_)
#endif
#ifndef EDDI_SWI_TRACE_01
#define EDDI_SWI_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_SWI_TRACE_02
#define EDDI_SWI_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_SWI_TRACE_03
#define EDDI_SWI_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_SWI_TRACE_04
#define EDDI_SWI_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_SWI_TRACE_05
#define EDDI_SWI_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_SWI_TRACE_06
#define EDDI_SWI_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_SWI_TRACE_07
#define EDDI_SWI_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_SWI_TRACE_08
#define EDDI_SWI_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_SWI_TRACE_09
#define EDDI_SWI_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_SWI_TRACE_10
#define EDDI_SWI_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_SWI_TRACE_11
#define EDDI_SWI_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_SWI_TRACE_12
#define EDDI_SWI_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_SWI_TRACE_13
#define EDDI_SWI_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_SWI_TRACE_14
#define EDDI_SWI_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_SWI_TRACE_15
#define EDDI_SWI_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_SWI_TRACE_16
#define EDDI_SWI_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_SWI, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_CRT_TRACE
#ifndef EDDI_CRT_TRACE_00
#define EDDI_CRT_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_)
#endif
#ifndef EDDI_CRT_TRACE_01
#define EDDI_CRT_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_CRT_TRACE_02
#define EDDI_CRT_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_CRT_TRACE_03
#define EDDI_CRT_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_CRT_TRACE_04
#define EDDI_CRT_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_CRT_TRACE_05
#define EDDI_CRT_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_CRT_TRACE_06
#define EDDI_CRT_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_CRT_TRACE_07
#define EDDI_CRT_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_CRT_TRACE_08
#define EDDI_CRT_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_CRT_TRACE_09
#define EDDI_CRT_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_CRT_TRACE_10
#define EDDI_CRT_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_CRT_TRACE_11
#define EDDI_CRT_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_CRT_TRACE_12
#define EDDI_CRT_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_CRT_TRACE_13
#define EDDI_CRT_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_CRT_TRACE_14
#define EDDI_CRT_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_CRT_TRACE_15
#define EDDI_CRT_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_CRT_TRACE_16
#define EDDI_CRT_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_CRT, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_SYNC_TRACE
#ifndef EDDI_SYNC_TRACE_00
#define EDDI_SYNC_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_)
#endif
#ifndef EDDI_SYNC_TRACE_01
#define EDDI_SYNC_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_SYNC_TRACE_02
#define EDDI_SYNC_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_SYNC_TRACE_03
#define EDDI_SYNC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_SYNC_TRACE_04
#define EDDI_SYNC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_SYNC_TRACE_05
#define EDDI_SYNC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_SYNC_TRACE_06
#define EDDI_SYNC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_SYNC_TRACE_07
#define EDDI_SYNC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_SYNC_TRACE_08
#define EDDI_SYNC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_SYNC_TRACE_09
#define EDDI_SYNC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_SYNC_TRACE_10
#define EDDI_SYNC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_SYNC_TRACE_11
#define EDDI_SYNC_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_SYNC_TRACE_12
#define EDDI_SYNC_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_SYNC_TRACE_13
#define EDDI_SYNC_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_SYNC_TRACE_14
#define EDDI_SYNC_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_SYNC_TRACE_15
#define EDDI_SYNC_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_SYNC_TRACE_16
#define EDDI_SYNC_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_SYNC, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

//EDDI_PROGRAM_TRACE
#ifndef EDDI_PROGRAM_TRACE_STRING
#define EDDI_PROGRAM_TRACE_STRING(idx_, level_, msg_, st_) \
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_STRING(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, st_)
#endif
#ifndef EDDI_PROGRAM_TRACE_00
#define EDDI_PROGRAM_TRACE_00(idx_, level_, msg_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_)
#endif
#ifndef EDDI_PROGRAM_TRACE_01
#define EDDI_PROGRAM_TRACE_01(idx_, level_, msg_, para1_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_))
#endif
#ifndef EDDI_PROGRAM_TRACE_02
#define EDDI_PROGRAM_TRACE_02(idx_, level_, msg_, para1_, para2_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_))
#endif
#ifndef EDDI_PROGRAM_TRACE_03
#define EDDI_PROGRAM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_))
#endif
#ifndef EDDI_PROGRAM_TRACE_04
#define EDDI_PROGRAM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_))
#endif
#ifndef EDDI_PROGRAM_TRACE_05
#define EDDI_PROGRAM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_))
#endif
#ifndef EDDI_PROGRAM_TRACE_06
#define EDDI_PROGRAM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_))
#endif
#ifndef EDDI_PROGRAM_TRACE_07
#define EDDI_PROGRAM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_))
#endif
#ifndef EDDI_PROGRAM_TRACE_08
#define EDDI_PROGRAM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_))
#endif
#ifndef EDDI_PROGRAM_TRACE_09
#define EDDI_PROGRAM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_))
#endif
#ifndef EDDI_PROGRAM_TRACE_10
#define EDDI_PROGRAM_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_))
#endif
#ifndef EDDI_PROGRAM_TRACE_11
#define EDDI_PROGRAM_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_))
#endif
#ifndef EDDI_PROGRAM_TRACE_12
#define EDDI_PROGRAM_TRACE_12(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_12(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_))
#endif
#ifndef EDDI_PROGRAM_TRACE_13
#define EDDI_PROGRAM_TRACE_13(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_13(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_))
#endif
#ifndef EDDI_PROGRAM_TRACE_14
#define EDDI_PROGRAM_TRACE_14(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_14(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_))
#endif
#ifndef EDDI_PROGRAM_TRACE_15
#define EDDI_PROGRAM_TRACE_15(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_15(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_))
#endif
#ifndef EDDI_PROGRAM_TRACE_16
#define EDDI_PROGRAM_TRACE_16(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_, para12_, para13_, para14_, para15_, para16_)\
    /*lint --e(1776) --e(613) --e(641) --e(506) --e(944) --e(522) --e(961) --e(62)*/\
    LSA_IDX_TRACE_16(TRACE_SUBSYS_EDDI_PROGRAM, idx_, level_, msg_, (para1_), (para2_), (para3_), (para4_), (para5_), (para6_), (para7_), (para8_), (para9_), (para10_), (para11_), (para12_), (para13_), (para14_), (para15_), (para16_))
#endif

#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_TRC_H


/*****************************************************************************/
/*  end of file eddi_trc.h                                                   */
/*****************************************************************************/
