
#ifndef GSY_TRC_H                     /* ----- reinclude-protection ----- */
#define GSY_TRC_H

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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_trc.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface for LSA-trace                                           */
/*  Defines LSA-trace macros for GSY.                                        */
/*                                                                           */
/*      Here the macros are adapted to the LSA component LTRC.               */
/*                                                                           */
/*      If LTRC shall not be used, then either the LSA_TRACE macros used     */
/*      here must be defined by the system in the file lsa_cfg.h/txt or      */
/*      directly the GSY-trace macro must be redefined on the desired        */
/*      function/macro in the gsy_cfg.h/txt file.                            */
/*      If the system isn't able to store trace data then an "empty          */
/*      definition" must be implemented in file gsy_cfg.h/txt (or the        */
/*      LSA_TRACE macros must be defined "empty" in lsa_cfg.h/txt).          */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version         Who What                                     */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2006-08-31  P03.12.00.00_00.25.03.02 lrg GSY_ERROR_TRACE_00/02 added     */
/*  2007-01-22  P03.12.00.00_00.40.07.02 lrg GSY_PRM_TRACE added             */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2010-07-20  P05.00.00.00_00.01.06.02 lrg: AP00999110 GSY_CFG_TRACE_MODE  */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-07-01  T05.00.00.00_00.05.31.02 lrg: GSY_ERROR_TRACE_02 -> 03       */
/*  2014-09-12  P06.00.00.00_00.01.44.01 1092865 GSY_ERROR_TRACE_05/06       */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 [GSY] GSY_FUNCTION_TRACE_05 */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* system interface output macros
 *
 * Note: There aren't any prototypes for these macros so that the target system
 * defines the pointer size and not the GSY. GSY doesn't cast the 
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
 *    LOWER:     All requirements to the lower layer are written into this 
 *               subsystem.
 *               Requests to the lower layer are marked with         >>>
 *               Confirmations from the lower layer are marked with  <<<
 *    MEMORY:    All memory requests for RQBs, data buffer, TA-Buffer, ...
 *               are written into this subsystem. Conversions of UPPER_TO_LOWER
 *               and reversed also belong to this subsystem.
 *               For functions which the system invokes:
 *               Calls of the system are marked with                 -->
 *               The end of the functions are marked with            <--
 *    SYSTEM:    All other requirements to / from the system are written into
 *               this subsystem. For example: Timers, path information, copying
 *               processes of COMMON memory, ....
 *               For functions which the system invokes:
 *               Calls of the system are marked with                 -->
 *               The end of the functions are marked with            <--
 *    PROTOCOL:  The protocol is written down here.
 *    FUNCTION:  (Internal) function call trace.
 *
 *    If the component developer wants to make trace entries of  other things 
 *    than above mentioned, he can define other subsystems.
 *
 * Note: The system integrator must define the following identifiers (if LTRC
 * is used in the file ltrc_sub.h/txt or if LTRC isn't used in a system file):
 *    TRACE_SUBSYS_GSY_YYYYY
 *    TRACE_SUBSYS_GSY_YYYYY_INST
 *    etc.
 */

/* used e.g. if TraceIdx not present (not initialized, not available in function, etc.) */
#define GSY_UNDEF_TRACE_IDX 0

/*****************************************************************************/
#if GSY_CFG_TRACE_MODE == 1                                     /* LSA trace */
/*****************************************************************************/
#ifndef GSY_UPPER_TRACE_01
#define GSY_UPPER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_UPPER, level_, msg_, para1_)
#endif
#ifndef GSY_UPPER_TRACE_02
#define GSY_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_UPPER, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_UPPER_TRACE_03
#define GSY_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_UPPER, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_UPPER_TRACE_04
#define GSY_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_UPPER, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef GSY_LOWER_TRACE_03
#define GSY_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_LOWER, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef GSY_SYSTEM_TRACE_00
#define GSY_SYSTEM_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_GSY_SYSTEM, level_, msg_)
#endif
#ifndef GSY_SYSTEM_TRACE_01
#define GSY_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_SYSTEM, level_, msg_, para1_)
#endif
#ifndef GSY_SYSTEM_TRACE_02
#define GSY_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_SYSTEM, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_SYSTEM_TRACE_03
#define GSY_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_SYSTEM, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_SYSTEM_TRACE_04
#define GSY_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_SYSTEM, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef GSY_FUNCTION_TRACE_00
#define GSY_FUNCTION_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_GSY_FUNCTION, level_, msg_)
#endif
#ifndef GSY_FUNCTION_TRACE_01
#define GSY_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_FUNCTION, level_, msg_, para1_)
#endif
#ifndef GSY_FUNCTION_TRACE_02
#define GSY_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_FUNCTION, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_FUNCTION_TRACE_03
#define GSY_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_FUNCTION, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_FUNCTION_TRACE_04
#define GSY_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_FUNCTION, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_FUNCTION_TRACE_05
#define GSY_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_GSY_FUNCTION, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef GSY_ERROR_TRACE_00
#define GSY_ERROR_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_GSY_ERROR, level_, msg_)
#endif
#ifndef GSY_ERROR_TRACE_01
#define GSY_ERROR_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_ERROR, level_, msg_, para1_)
#endif
#ifndef GSY_ERROR_TRACE_02
#define GSY_ERROR_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_ERROR, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_ERROR_TRACE_03
#define GSY_ERROR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_ERROR, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_ERROR_TRACE_04
#define GSY_ERROR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_ERROR, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_ERROR_TRACE_05
#define GSY_ERROR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_GSY_ERROR, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_ERROR_TRACE_06
#define GSY_ERROR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
		LSA_TRACE_06(TRACE_SUBSYS_GSY_ERROR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef GSY_ERROR_TRACE_07
#define GSY_ERROR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
		LSA_TRACE_07(TRACE_SUBSYS_GSY_ERROR, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif

#ifndef GSY_DIAG_TRACE_04
#define GSY_DIAG_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_DIAG, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef GSY_DEL_TRACE_00
#define GSY_DEL_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_GSY_DEL, level_, msg_)
#endif
#ifndef GSY_DEL_TRACE_01
#define GSY_DEL_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_)
#endif
#ifndef GSY_DEL_TRACE_02
#define GSY_DEL_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_DEL_TRACE_03
#define GSY_DEL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_DEL_TRACE_04
#define GSY_DEL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_DEL_TRACE_08
#define GSY_DEL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
		LSA_TRACE_08(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_DEL_TRACE_09
#define GSY_DEL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
		LSA_TRACE_09(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif
#ifndef GSY_DEL_TRACE_10
#define GSY_DEL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)	\
		LSA_TRACE_10(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif
#ifndef GSY_DEL_TRACE_11
#define GSY_DEL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)	\
		LSA_TRACE_11(TRACE_SUBSYS_GSY_DEL, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef GSY_FWD_TRACE_00
#define GSY_FWD_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_GSY_FWD, level_, msg_)
#endif
#ifndef GSY_FWD_TRACE_01
#define GSY_FWD_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_FWD, level_, msg_, para1_)
#endif
#ifndef GSY_FWD_TRACE_02
#define GSY_FWD_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_FWD, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_FWD_TRACE_03
#define GSY_FWD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_FWD, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_FWD_TRACE_04
#define GSY_FWD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_FWD, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_FWD_TRACE_05
#define GSY_FWD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_GSY_FWD, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_FWD_TRACE_06
#define GSY_FWD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
		LSA_TRACE_06(TRACE_SUBSYS_GSY_FWD, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef GSY_PRM_TRACE_00
#define GSY_PRM_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_GSY_PRM, level_, msg_)
#endif
#ifndef GSY_PRM_TRACE_01
#define GSY_PRM_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_)
#endif
#ifndef GSY_PRM_TRACE_02
#define GSY_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_PRM_TRACE_03
#define GSY_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_PRM_TRACE_04
#define GSY_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_PRM_TRACE_05
#define GSY_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_PRM_TRACE_06
#define GSY_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
		LSA_TRACE_06(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef GSY_PRM_TRACE_07
#define GSY_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
		LSA_TRACE_07(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef GSY_PRM_TRACE_08
#define GSY_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
		LSA_TRACE_08(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_PRM_TRACE_09
#define GSY_PRM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
		LSA_TRACE_09(TRACE_SUBSYS_GSY_PRM, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

#ifndef GSY_SYNC_TRACE_00
#define GSY_SYNC_TRACE_00(idx_, level_, msg_)	\
		LSA_TRACE_00(TRACE_SUBSYS_GSY_SYNC, level_, msg_)
#endif
#ifndef GSY_SYNC_TRACE_01
#define GSY_SYNC_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_)
#endif
#ifndef GSY_SYNC_TRACE_02
#define GSY_SYNC_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_SYNC_TRACE_03
#define GSY_SYNC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
		LSA_TRACE_03(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_SYNC_TRACE_04
#define GSY_SYNC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
		LSA_TRACE_04(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_SYNC_TRACE_05
#define GSY_SYNC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_SYNC_TRACE_06
#define GSY_SYNC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
		LSA_TRACE_06(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef GSY_SYNC_TRACE_07
#define GSY_SYNC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
		LSA_TRACE_07(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef GSY_SYNC_TRACE_08
#define GSY_SYNC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
		LSA_TRACE_08(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_SYNC_TRACE_09
#define GSY_SYNC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
		LSA_TRACE_09(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif
#ifndef GSY_SYNC_TRACE_10
#define GSY_SYNC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)	\
		LSA_TRACE_10(TRACE_SUBSYS_GSY_SYNC, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef GSY_MASTER_TRACE_00
#define GSY_MASTER_TRACE_00(idx_, level_, msg_) \
		LSA_TRACE_00(TRACE_SUBSYS_GSY_MASTER, level_, msg_)
#endif
#ifndef GSY_MASTER_TRACE_01
#define GSY_MASTER_TRACE_01(idx_, level_, msg_, para1_)	\
		LSA_TRACE_01(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_)
#endif
#ifndef GSY_MASTER_TRACE_02
#define GSY_MASTER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
		LSA_TRACE_02(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_MASTER_TRACE_03
#define GSY_MASTER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
		LSA_TRACE_03(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_MASTER_TRACE_04
#define GSY_MASTER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
		LSA_TRACE_04(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_MASTER_TRACE_05
#define GSY_MASTER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
		LSA_TRACE_05(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_MASTER_TRACE_06
#define GSY_MASTER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
		LSA_TRACE_06(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef GSY_MASTER_TRACE_07
#define GSY_MASTER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
		LSA_TRACE_07(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef GSY_MASTER_TRACE_08
#define GSY_MASTER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
		LSA_TRACE_08(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_MASTER_TRACE_09
#define GSY_MASTER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
		LSA_TRACE_09(TRACE_SUBSYS_GSY_MASTER, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

/*=============================================================================
 * macro names:   GSY_YYYYY_TRACE_BYTE_ARRAY
 *
 * macro:         Trace entry of a byte array for the subsystem "YYYYY".
 *
 *                The system integrator must define the identifier 
 *                TRACE_SUBSYS_GSY_YYYYY (if LTRC is used in the file
 *                ltrc_sub.h/txt or if LTRC isn't used in a system file)!
 *
 *                Note: GSY_YYYYY_TRACE_BYTE_ARRAY isn't supported by
 *                LSA_LTRC_P01.00.00.00_00.02.02.01 yet.
 *
 * parameters:    level_:   LSA_TRACE_LEVEL_OFF, LSA_TRACE_LEVEL_FATAL,
 *                          LSA_TRACE_LEVEL_ERROR, LSA_TRACE_LEVEL_UNEXP,
 *                          LSA_TRACE_LEVEL_WARN, LSA_TRACE_LEVEL_NOTE_HIGH,
 *                          LSA_TRACE_LEVEL_NOTE, LSA_TRACE_LEVEL_NOTE_LOW or
 *                          LSA_TRACE_LEVEL_CHAT
 *                msg_:     string similar to "printf"
 *                ptr_:     pointer to byte array
 *                len_:     number of bytes of the array; len <= 1024
 *
 * return value:  LSA_VOID
 *===========================================================================*/
#ifndef GSY_SNDRCV_TRACE_BYTE_ARRAY
#define GSY_SNDRCV_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
		LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_GSY_SNDRCV, level_, msg_, ptr_, len_)
#endif
#ifndef GSY_ERROR_TRACE_BYTE_ARRAY
#define GSY_ERROR_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
		LSA_TRACE_BYTE_ARRAY(TRACE_SUBSYS_GSY_ERROR, level_, msg_, ptr_, len_)
#endif

/*****************************************************************************/
#elif GSY_CFG_TRACE_MODE == 2                            /* LSA indexed trace*/
/*****************************************************************************/
#ifndef GSY_UPPER_TRACE_01
#define GSY_UPPER_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_UPPER, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_UPPER_TRACE_02
#define GSY_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_UPPER, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_UPPER_TRACE_03
#define GSY_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_UPPER, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_UPPER_TRACE_04
#define GSY_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_UPPER, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef GSY_LOWER_TRACE_03
#define GSY_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_LOWER, idx_, level_, msg_, para1_, para2_, para3_)
#endif

#ifndef GSY_SYSTEM_TRACE_00
#define GSY_SYSTEM_TRACE_00(idx_, level_, msg_)	\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_SYSTEM, idx_, level_, msg_)
#endif
#ifndef GSY_SYSTEM_TRACE_01
#define GSY_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_SYSTEM, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_SYSTEM_TRACE_02
#define GSY_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_SYSTEM, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_SYSTEM_TRACE_03
#define GSY_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_SYSTEM, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_SYSTEM_TRACE_04
#define GSY_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_SYSTEM, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef GSY_FUNCTION_TRACE_00
#define GSY_FUNCTION_TRACE_00(idx_, level_, msg_)	\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_FUNCTION, idx_, level_, msg_)
#endif
#ifndef GSY_FUNCTION_TRACE_01
#define GSY_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_FUNCTION, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_FUNCTION_TRACE_02
#define GSY_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_FUNCTION, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_FUNCTION_TRACE_03
#define GSY_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_FUNCTION, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_FUNCTION_TRACE_04
#define GSY_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_FUNCTION, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_FUNCTION_TRACE_05
#define GSY_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_GSY_FUNCTION, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif

#ifndef GSY_ERROR_TRACE_00
#define GSY_ERROR_TRACE_00(idx_, level_, msg_)	\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_)
#endif
#ifndef GSY_ERROR_TRACE_01
#define GSY_ERROR_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_ERROR_TRACE_02
#define GSY_ERROR_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_ERROR_TRACE_03
#define GSY_ERROR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_ERROR_TRACE_04
#define GSY_ERROR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_ERROR_TRACE_05
#define GSY_ERROR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_ERROR_TRACE_06
#define GSY_ERROR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef GSY_ERROR_TRACE_07
#define GSY_ERROR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef GSY_DIAG_TRACE_04
#define GSY_DIAG_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_DIAG, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif

#ifndef GSY_DEL_TRACE_00
#define GSY_DEL_TRACE_00(idx_, level_, msg_)	\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_)
#endif
#ifndef GSY_DEL_TRACE_01
#define GSY_DEL_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_DEL_TRACE_02
#define GSY_DEL_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_DEL_TRACE_03
#define GSY_DEL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_DEL_TRACE_04
#define GSY_DEL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_DEL_TRACE_08
#define GSY_DEL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_DEL_TRACE_09
#define GSY_DEL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif
#ifndef GSY_DEL_TRACE_10
#define GSY_DEL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)	\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif
#ifndef GSY_DEL_TRACE_11
#define GSY_DEL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)	\
    LSA_IDX_TRACE_11(TRACE_SUBSYS_GSY_DEL, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)
#endif

#ifndef GSY_FWD_TRACE_00
#define GSY_FWD_TRACE_00(idx_, level_, msg_)	\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_FWD, idx_, level_, msg_)
#endif
#ifndef GSY_FWD_TRACE_01
#define GSY_FWD_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_FWD, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_FWD_TRACE_02
#define GSY_FWD_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_FWD, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_FWD_TRACE_03
#define GSY_FWD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_FWD, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_FWD_TRACE_04
#define GSY_FWD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_FWD, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_FWD_TRACE_05
#define GSY_FWD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_GSY_FWD, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_FWD_TRACE_06
#define GSY_FWD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_GSY_FWD, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif

#ifndef GSY_PRM_TRACE_00
#define GSY_PRM_TRACE_00(idx_, level_, msg_)	\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_)
#endif
#ifndef GSY_PRM_TRACE_01
#define GSY_PRM_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_PRM_TRACE_02
#define GSY_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_PRM_TRACE_03
#define GSY_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_PRM_TRACE_04
#define GSY_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_PRM_TRACE_05
#define GSY_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_PRM_TRACE_06
#define GSY_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef GSY_PRM_TRACE_07
#define GSY_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef GSY_PRM_TRACE_08
#define GSY_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_PRM_TRACE_09
#define GSY_PRM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_GSY_PRM, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif
#ifndef GSY_SYNC_TRACE_00
#define GSY_SYNC_TRACE_00(idx_, level_, msg_)	\
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_)
#endif

#ifndef GSY_SYNC_TRACE_01
#define GSY_SYNC_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_SYNC_TRACE_02
#define GSY_SYNC_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_SYNC_TRACE_03
#define GSY_SYNC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)	\
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_SYNC_TRACE_04
#define GSY_SYNC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)	\
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_SYNC_TRACE_05
#define GSY_SYNC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_SYNC_TRACE_06
#define GSY_SYNC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef GSY_SYNC_TRACE_07
#define GSY_SYNC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef GSY_SYNC_TRACE_08
#define GSY_SYNC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_SYNC_TRACE_09
#define GSY_SYNC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif
#ifndef GSY_SYNC_TRACE_10
#define GSY_SYNC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)	\
    LSA_IDX_TRACE_10(TRACE_SUBSYS_GSY_SYNC, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#endif

#ifndef GSY_MASTER_TRACE_00
#define GSY_MASTER_TRACE_00(idx_, level_, msg_) \
    LSA_IDX_TRACE_00(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_)
#endif
#ifndef GSY_MASTER_TRACE_01
#define GSY_MASTER_TRACE_01(idx_, level_, msg_, para1_)	\
    LSA_IDX_TRACE_01(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_)
#endif
#ifndef GSY_MASTER_TRACE_02
#define GSY_MASTER_TRACE_02(idx_, level_, msg_, para1_, para2_)	\
    LSA_IDX_TRACE_02(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_)
#endif
#ifndef GSY_MASTER_TRACE_03
#define GSY_MASTER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_) \
    LSA_IDX_TRACE_03(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_, para3_)
#endif
#ifndef GSY_MASTER_TRACE_04
#define GSY_MASTER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_) \
    LSA_IDX_TRACE_04(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_, para3_, para4_)
#endif
#ifndef GSY_MASTER_TRACE_05
#define GSY_MASTER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)	\
    LSA_IDX_TRACE_05(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#endif
#ifndef GSY_MASTER_TRACE_06
#define GSY_MASTER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)	\
    LSA_IDX_TRACE_06(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#endif
#ifndef GSY_MASTER_TRACE_07
#define GSY_MASTER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)	\
    LSA_IDX_TRACE_07(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#endif
#ifndef GSY_MASTER_TRACE_08
#define GSY_MASTER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)	\
    LSA_IDX_TRACE_08(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#endif
#ifndef GSY_MASTER_TRACE_09
#define GSY_MASTER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)	\
    LSA_IDX_TRACE_09(TRACE_SUBSYS_GSY_MASTER, idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#endif

/*=============================================================================
 * macro names:   GSY_YYYYY_TRACE_BYTE_ARRAY
 *
 * macro:         Trace entry of a byte array for the subsystem "YYYYY".
 *
 *                The system integrator must define the identifier 
 *                TRACE_SUBSYS_GSY_YYYYY (if LTRC is used in the file
 *                ltrc_sub.h/txt or if LTRC isn't used in a system file)!
 *
 *                Note: GSY_YYYYY_TRACE_BYTE_ARRAY isn't supported by
 *                LSA_LTRC_P01.00.00.00_00.02.02.01 yet.
 *
 * parameters:    level_:   LSA_TRACE_LEVEL_OFF, LSA_TRACE_LEVEL_FATAL,
 *                          LSA_TRACE_LEVEL_ERROR, LSA_TRACE_LEVEL_UNEXP,
 *                          LSA_TRACE_LEVEL_WARN, LSA_TRACE_LEVEL_NOTE_HIGH,
 *                          LSA_TRACE_LEVEL_NOTE, LSA_TRACE_LEVEL_NOTE_LOW or
 *                          LSA_TRACE_LEVEL_CHAT
 *                msg_:     string similar to "printf"
 *                ptr_:     pointer to byte array
 *                len_:     number of bytes of the array; len <= 1024
 *
 * return value:  LSA_VOID
 *===========================================================================*/
#ifndef GSY_SNDRCV_TRACE_BYTE_ARRAY
#define GSY_SNDRCV_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
    LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_GSY_SNDRCV, idx_, level_, msg_, ptr_, len_)
#endif
#ifndef GSY_ERROR_TRACE_BYTE_ARRAY
#define GSY_ERROR_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)	\
    LSA_IDX_TRACE_BYTE_ARRAY(TRACE_SUBSYS_GSY_ERROR, idx_, level_, msg_, ptr_, len_)
#endif

/*****************************************************************************/
#elif GSY_CFG_TRACE_MODE == 0                                    /* no trace */
/*****************************************************************************/
#define GSY_ERROR_TRACE_00(idx_, level_, msg_)
#define GSY_ERROR_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_ERROR_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_ERROR_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_ERROR_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define GSY_ERROR_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define GSY_ERROR_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define GSY_ERROR_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)

#define GSY_UPPER_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_UPPER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_UPPER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_UPPER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)

#define GSY_LOWER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)

#define GSY_SYSTEM_TRACE_00(idx_, level_, msg_)
#define GSY_SYSTEM_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_SYSTEM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_SYSTEM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_SYSTEM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)

#define GSY_FUNCTION_TRACE_00(idx_, level_, msg_)
#define GSY_FUNCTION_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_FUNCTION_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_FUNCTION_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_FUNCTION_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define GSY_FUNCTION_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)

#define GSY_SYNC_TRACE_00(idx_, level_, msg_)
#define GSY_SYNC_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_SYNC_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_SYNC_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_SYNC_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define GSY_SYNC_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define GSY_SYNC_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define GSY_SYNC_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define GSY_SYNC_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define GSY_SYNC_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define GSY_SYNC_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)

#define GSY_MASTER_TRACE_00(idx_, level_, msg_)
#define GSY_MASTER_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_MASTER_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_MASTER_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_MASTER_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define GSY_MASTER_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define GSY_MASTER_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define GSY_MASTER_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define GSY_MASTER_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define GSY_MASTER_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)

#define GSY_FWD_TRACE_00(idx_, level_, msg_)
#define GSY_FWD_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_FWD_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_FWD_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_FWD_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define GSY_FWD_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define GSY_FWD_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)

#define GSY_DIAG_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)

#define GSY_DEL_TRACE_00(idx_, level_, msg_)
#define GSY_DEL_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_DEL_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_DEL_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_DEL_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define GSY_DEL_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define GSY_DEL_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)
#define GSY_DEL_TRACE_10(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_)
#define GSY_DEL_TRACE_11(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_, para10_, para11_)

#define GSY_PRM_TRACE_00(idx_, level_, msg_)
#define GSY_PRM_TRACE_01(idx_, level_, msg_, para1_)
#define GSY_PRM_TRACE_02(idx_, level_, msg_, para1_, para2_)
#define GSY_PRM_TRACE_03(idx_, level_, msg_, para1_, para2_, para3_)
#define GSY_PRM_TRACE_04(idx_, level_, msg_, para1_, para2_, para3_, para4_)
#define GSY_PRM_TRACE_05(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_)
#define GSY_PRM_TRACE_06(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
#define GSY_PRM_TRACE_07(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
#define GSY_PRM_TRACE_08(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
#define GSY_PRM_TRACE_09(idx_, level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_, para9_)

#define GSY_SNDRCV_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)
#define GSY_ERROR_TRACE_BYTE_ARRAY(idx_, level_, msg_, ptr_, len_)
/*****************************************************************************/
#else
#error "GSY_CFG_TRACE_MODE has unknown value"
#endif
/*****************************************************************************/
/*  end of file gsy_trc.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_TRC_H */
