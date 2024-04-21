#ifndef PNTRC_SUB_H
#define PNTRC_SUB_H

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
/*  C o m p o n e n t     &C: PNTRC (PROFINET Trace)                    :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_sub.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration for pntrc:                                                 */
/*  Defines subsystems for pntrc.                                            */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different subsystems for lsa-trace.       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
 
/*
 * Default compile Level
 */
#define PNTRC_CL_DEFAULT                                        PNTRC_TRACE_LEVEL_CHAT  ///< default compile level for  all components (if not specified different)

#define COMPILE_LEVEL(subsys) (PNTRC_COMPILE_LEVEL_ ## subsys)
#include "pntrc_compile_autogen.h"
#include "pntrc_sub_autogen.h"

/*=============================================================================
 * (4) Api
 *===========================================================================*/
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
#define PNTRC_COMPILE_MODULE_ID         LTRC_ACT_MODUL_ID,
#define PNTRC_COMPILE_MODULE_ID_TYPE    LSA_UINT32,
#else
#define PNTRC_COMPILE_MODULE_ID
#define PNTRC_COMPILE_MODULE_ID_TYPE
#endif

#if (PNTRC_CFG_COMPILE_FILE == 1)
#ifdef __cplusplus
#define PNTRC_COMPILE_FILE              (LSA_CHAR*)__FILE__,
#else
#define PNTRC_COMPILE_FILE              __FILE__,
#endif
#define PNTRC_COMPILE_FILE_TYPE         LSA_CHAR*,
#else
#define PNTRC_COMPILE_FILE
#define PNTRC_COMPILE_FILE_TYPE
#endif

#if (PNTRC_CFG_COMPILE_LINE == 1)
#define PNTRC_COMPILE_LINE              __LINE__,
#define PNTRC_COMPILE_LINE_TYPE         LSA_UINT32,
#else
#define PNTRC_COMPILE_LINE
#define PNTRC_COMPILE_LINE_TYPE
#endif

// Note: If no text is compiled, the EXT_ macros throw an unused arg warning for msg. PNTRC_SUPRESS_WARNING_TEXT is empty if msg is used and mapped to LSA_UNUSED_ARG if the msg is not used.
#define PNTRC_COMPILE_TEXT_TYPE
#define PNTRC_SUPRESS_WARNING_TEXT      LSA_UNUSED_ARG(msg); 


#define PNTRC_ATTR_DUMMY
struct pntrc_entry_fcts
{
    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry00)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry01)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry02)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry03)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry04)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry05)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry06)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry07)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry08)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry09)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry10)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry11)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry12)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry13)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry14)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry15)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entry16)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32,
       LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32, LSA_UINT32);

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entrybytearray)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32,
       const LSA_UINT8 *, LSA_UINT16);

	LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entrystring)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32,
       const LSA_CHAR * );

    LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entrysynctime)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32);

	LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entrysynctime_start)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32);

	LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entrysynctime_local)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
       LSA_UINT32);

	LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entrysynctime_lower)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
      LSA_UINT32, LSA_UINT32);

	LSA_BOOL LSA_FCT_PTR(PNTRC_ATTR_DUMMY, entrysynctime_scaler)
      (LTRC_SUBSYS_TYPE, PNTRC_LEVEL_TYPE, LSA_UINT32, PNTRC_COMPILE_MODULE_ID_TYPE PNTRC_COMPILE_FILE_TYPE PNTRC_COMPILE_LINE_TYPE PNTRC_COMPILE_TEXT_TYPE LSA_UINT32, LSA_UINT32,
	  LSA_UINT32);
};


#ifdef PNTRC_MODULE_ID
#if (PNTRC_MODULE_ID == 1)
	/* defined in pntrc_int.h */
#else
	LSA_EXTERN LSA_UINT8 ltrc_current_level[TRACE_SUBSYS_NUM];  /*legacy mode for adonis.c - to be removed*/
	LSA_EXTERN LSA_UINT8 pntrc_current_level[TRACE_SUBSYS_NUM];
	LSA_EXTERN struct pntrc_entry_fcts pntrc_current_fct;
#endif
#else
	LSA_EXTERN LSA_UINT8 ltrc_current_level[TRACE_SUBSYS_NUM];  /*legacy mode for adonis.c - to be removed*/
	LSA_EXTERN LSA_UINT8 pntrc_current_level[TRACE_SUBSYS_NUM];
	LSA_EXTERN struct pntrc_entry_fcts pntrc_current_fct;
#endif

#define CHECK_LEVEL(subsys, idx, level) \
    ((level <= COMPILE_LEVEL(subsys) ) && (level > PNTRC_TRACE_LEVEL_OFF ) && \
    (level <= pntrc_current_level [subsys + idx]))

    /*  To enable the tracing of the interface idx
        and to minimize the effort, the interface idx will be
        addded to the subsys number
        in pntrc_converter and viewer this sum will be divorced in their originally parts
        -> former subsys is now (LTRC_SUBSYS_TYPE)(subsys + idx)
    */
    /* CHECK_LEVEL is used to compile out the traces. 
    If the compile level matches the actual level, the trace macro extends to
        "pntrc_current_fct.entryxx( all parameters )" 
    without the semicolon ';' that is added by the caller
    If the compile level is below the actual level, the trace macro extends to
        "0" 
    without the semicolon ';' that is added by the caller

    */


    #define PNTRC_TRACE_00(subsys, idx, level, msg) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry00 \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__) \
			: 0)

    #define PNTRC_TRACE_01(subsys, idx, level, msg, p1) (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry01  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1)) \
			: 0)

    #define PNTRC_TRACE_02(subsys, idx, level, msg, p1, p2) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry02  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2)) \
			: 0)

    #define PNTRC_TRACE_03(subsys, idx, level, msg, p1, p2, p3) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry03  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3)) \
			: 0)

    #define PNTRC_TRACE_04(subsys, idx, level, msg, p1, p2, p3, p4) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry04  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4)) \
			: 0)

    #define PNTRC_TRACE_05(subsys, idx, level, msg, p1, p2, p3, p4, p5) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry05  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5)) \
			: 0)

    #define PNTRC_TRACE_06(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6) (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry06  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6)) \
			: 0)

    #define PNTRC_TRACE_07(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry07  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7)) \
			: 0)

    #define PNTRC_TRACE_08(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry08  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8)) \
			: 0)

    #define PNTRC_TRACE_09(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry09  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9)) \
			: 0)

    #define PNTRC_TRACE_10(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry10  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9),(LSA_UINT32) (p10)) \
			: 0)

    #define PNTRC_TRACE_11(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry11  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9),(LSA_UINT32) (p10),(LSA_UINT32) (p11)) \
			: 0)

    #define PNTRC_TRACE_12(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry12  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9),(LSA_UINT32) (p10),(LSA_UINT32) (p11),(LSA_UINT32) (p12)) \
			: 0)

    #define PNTRC_TRACE_13(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry13  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9),(LSA_UINT32) (p10),(LSA_UINT32) (p11),(LSA_UINT32) (p12),(LSA_UINT32) (p13)) \
			: 0)

    #define PNTRC_TRACE_14(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry14  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9),(LSA_UINT32) (p10),(LSA_UINT32) (p11),(LSA_UINT32) (p12),(LSA_UINT32) (p13),(LSA_UINT32) (p14)) \
			: 0)

    #define PNTRC_TRACE_15(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry15  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9),(LSA_UINT32) (p10),(LSA_UINT32) (p11),(LSA_UINT32) (p12),(LSA_UINT32) (p13),(LSA_UINT32) (p14),(LSA_UINT32) (p15)) \
			: 0)

    #define PNTRC_TRACE_16(subsys, idx, level, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry16  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT32) (p1),(LSA_UINT32) (p2),(LSA_UINT32) (p3),(LSA_UINT32) (p4),(LSA_UINT32) (p5),(LSA_UINT32) (p6),(LSA_UINT32) (p7),(LSA_UINT32) (p8),(LSA_UINT32) (p9),(LSA_UINT32) (p10),(LSA_UINT32) (p11),(LSA_UINT32) (p12),(LSA_UINT32) (p13),(LSA_UINT32) (p14),(LSA_UINT32) (p15),(LSA_UINT32) (p16)) \
			: 0)

    #define PNTRC_TRACE_BYTE_ARRAY(subsys, idx, level, msg, dataptr, datalen) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entrybytearray  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_UINT8*) (dataptr), (LSA_UINT16) (datalen)) \
			: 0)

    #define PNTRC_TRACE_STRING(subsys, idx, level, msg, st) (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entrystring  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
			(LSA_CHAR*) (st)) \
			: 0)

    #define PNTRC_TRACE_SYNC_TIME(subsys, idx, level, msg, p1, p2) (\
            CHECK_LEVEL(subsys, idx, level) ? \
            pntrc_current_fct.entrysynctime  \
            ((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
            ( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
            (LSA_UINT32) (p1),(LSA_UINT32) (p2)) \
            : 0)

    #define PNTRC_TRACE_SYNC_TIME_START(subsys, idx, level, msg, p1, p2) (\
            CHECK_LEVEL(subsys, idx, level) ? \
            pntrc_current_fct.entrysynctime_start  \
            ((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
            ( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
            (LSA_UINT32) (p1),(LSA_UINT32) (p2)) \
            : 0)

    #define PNTRC_TRACE_SYNC_TIME_LOCAL(subsys, idx, level, msg, p1, p2) (\
            CHECK_LEVEL(subsys, idx, level) ? \
            pntrc_current_fct.entrysynctime_local  \
            ((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
            ( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
            (LSA_UINT32) (p1),(LSA_UINT32) (p2)) \
            : 0)

    #define PNTRC_TRACE_SYNC_TIME_LOWER(subsys, idx, level, msg, p1, p2, p3) (\
            CHECK_LEVEL(subsys, idx, level) ? \
            pntrc_current_fct.entrysynctime_lower  \
            ((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
            ( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
            (LSA_UINT32) (p1),(LSA_UINT32) (p2), (LSA_UINT32) (p3)) \
            : 0)

    #define PNTRC_TRACE_SYNC_TIME_SCALER(subsys, idx, level, msg, p1, p2) (\
            CHECK_LEVEL(subsys, idx, level) ? \
            pntrc_current_fct.entrysynctime_scaler  \
            ((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
            ( (LSA_UINT32)LTRC_ACT_MODUL_ID << 16) | __LINE__, \
            (LSA_UINT32) (p1),(LSA_UINT32) (p2)) \
            : 0)
        /* The EXT macros require PNTRC_SUPRESS_WARNING_TEXT to surpress warnings for 'msg' unused arg / unreferences formal parameter.
        The ext macros are extended to:
        "{
            LSA_UNUSED_ARG(msg);
            pntrc_current_fct.entryXX ( all parameters );
        }" << the caller adds a ';' here
        If the compile level of the subsystem is below, it is extended to:
        "{
            LSA_UNUSED_ARG(msg);
            0;
        }" << the caller adds a ';' here
        */
    #define PNTRC_TRACE_00_EXT(subsys, idx, level, module, line, msg)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry00  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)) \
			: 0);\
            }

    #define PNTRC_TRACE_01_EXT(subsys, idx, level, module, line, msg, p1)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry01  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1)) \
			: 0);\
            }

    #define PNTRC_TRACE_02_EXT(subsys, idx, level, module, line,  msg, p1, p2)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry02  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2)) \
			: 0);\
            }

    #define PNTRC_TRACE_03_EXT(subsys, idx, level, module, line, msg, p1, p2, p3)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry03  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
		    (( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3)) \
			: 0);\
            }

    #define PNTRC_TRACE_04_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry04  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4)) \
			: 0);\
            }

    #define PNTRC_TRACE_05_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry05  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5)) \
			: 0);\
            }

    #define PNTRC_TRACE_06_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry06  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6)) \
			: 0);\
            }

    #define PNTRC_TRACE_07_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry07  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7)) \
			: 0);\
            }

    #define PNTRC_TRACE_08_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry08  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8)) \
			: 0);\
            }

    #define PNTRC_TRACE_09_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry09  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9)) \
			: 0);\
            }

    #define PNTRC_TRACE_10_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
			CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry10  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9), (LSA_UINT32) (p10)) \
			: 0);\
            }

    #define PNTRC_TRACE_11_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry11  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9), (LSA_UINT32) (p10), (LSA_UINT32) (p11)) \
			: 0);\
            }

    #define PNTRC_TRACE_12_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry12  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9), (LSA_UINT32) (p10), (LSA_UINT32) (p11), (LSA_UINT32) (p12)) \
			: 0);\
            }

    #define PNTRC_TRACE_13_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry13  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9), (LSA_UINT32) (p10), (LSA_UINT32) (p11), (LSA_UINT32) (p12), (LSA_UINT32) (p13)) \
			: 0);\
            }

    #define PNTRC_TRACE_14_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry14  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9), (LSA_UINT32) (p10), (LSA_UINT32) (p11), (LSA_UINT32) (p12), (LSA_UINT32) (p13), (LSA_UINT32) (p14)) \
			: 0);\
            }

    #define PNTRC_TRACE_15_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry15  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9), (LSA_UINT32) (p10), (LSA_UINT32) (p11), (LSA_UINT32) (p12), (LSA_UINT32) (p13), (LSA_UINT32) (p14), (LSA_UINT32) (p15)) \
			: 0);\
            }

    #define PNTRC_TRACE_16_EXT(subsys, idx, level, module, line, msg, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16)\
            { PNTRC_SUPRESS_WARNING_TEXT \
            (\
            CHECK_LEVEL(subsys, idx, level) ? \
			pntrc_current_fct.entry16  \
			((LTRC_SUBSYS_TYPE)(subsys + idx), level, idx, PNTRC_COMPILE_MODULE_ID PNTRC_COMPILE_FILE PNTRC_COMPILE_LINE \
			(( (LSA_UINT32) ((LSA_UINT32) module  << 16) | (LSA_UINT32) line)), \
			(LSA_UINT32) (p1), (LSA_UINT32) (p2), (LSA_UINT32) (p3), (LSA_UINT32) (p4), (LSA_UINT32) (p5), (LSA_UINT32) (p6), (LSA_UINT32) (p7), (LSA_UINT32) (p8), (LSA_UINT32) (p9), (LSA_UINT32) (p10), (LSA_UINT32) (p11), (LSA_UINT32) (p12), (LSA_UINT32) (p13), (LSA_UINT32) (p14), (LSA_UINT32) (p15), (LSA_UINT32) (p16)) \
			: 0);\
            }



/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PNTRC_SUB_H */
