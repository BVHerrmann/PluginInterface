
#ifndef EDDS_DBG_H                       /* ----- reinclude-protection ----- */
#define EDDS_DBG_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_dbg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile for debugging                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  05.04.04    JS    some defines changed to prevent compiler warnings.     */
/*  10.08.04    JS    added some macros                                      */
/*  27.01.05    JS    ported to LTRC                                         */
/*  06.07.05    JS    Tracelevel warning added.                              */
/*  19.10.05    JS    LSA_TRACE_LEVEL_WARNING to LSA_TRACE_LEVEL_WARN        */
#endif
/*****************************************************************************/



/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/


/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)

#define EDDS_INTERNAL_STATISTICS_INIT(hSysDev, statistics) { statistics.time = 0;\
    statistics.timesum = 0;\
    statistics.sumcount = 0;\
    statistics.min = ~0;\
    statistics.max = 0;}
#define EDDS_INTERNAL_STATISTIC_ENTER(hSysDev,statistics)  { statistics.time = EDDS_GET_SYSTEM_TIME_NS(hSysDev); }
#define EDDS_INTERNAL_STATISTIC_EXIT(hSysDev,statistics)   { statistics.time = EDDS_GET_SYSTEM_TIME_NS(hSysDev) - statistics.time;\
    if(statistics.min > statistics.time) statistics.min = statistics.time;\
    if(statistics.max < statistics.time) statistics.max = statistics.time;\
    statistics.timesum += statistics.time;\
    ++statistics.sumcount;}
#define EDDS_INTERNAL_STATISTICS_DEINIT(hSysDev, statistics) { statistics.time = ((statistics.sumcount) ? (statistics.timesum / statistics.sumcount) : 0); }

#else  /* defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT) */

#define EDDS_INTERNAL_STATISTICS_INIT(hSysDev, statistics) LSA_UNUSED_ARG(hSysDev);statistics = 0
#define EDDS_INTERNAL_STATISTIC_ENTER(hSysDev,statistics)  LSA_UNUSED_ARG(hSysDev);++statistics
#define EDDS_INTERNAL_STATISTIC_EXIT(hSysDev,statistics)   LSA_UNUSED_ARG(hSysDev);LSA_UNUSED_ARG(statistics)/* statistics += 0*/ /* this is done to prevent "unused argument" warnings */
#define EDDS_INTERNAL_STATISTICS_DEINIT(hSysDev,statistics) EDDS_INTERNAL_STATISTICS_INIT(hSysDev, statistics)

#endif /* defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT) */
/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

#ifndef EDDS_INTERNAL_PERFORMANCE_STATISTICS_INIT
LSA_VOID EDDS_INTERNAL_PERFORMANCE_STATISTICS_INIT(
        EDDS_DDB_TYPE         * const pDDB
);
#endif

#ifndef EDDS_INTERNAL_PERFORMANCE_STATISTICS_DEINIT
LSA_VOID EDDS_INTERNAL_PERFORMANCE_STATISTICS_DEINIT(
        EDDS_DDB_TYPE         * const pDDB
);
#endif

#ifndef EDDS_INTERNAL_PERFORMANCE_STATISTICS_TRACE
LSA_VOID EDDS_INTERNAL_PERFORMANCE_STATISTICS_TRACE(
        EDDS_DDB_TYPE         const * const pDDB
);
#endif


#if defined(EDDS_CFG_FREE_ERROR_IS_FATAL) || defined(EDDS_CFG_DEBUG_ENABLE)
#define EDDS_FREE_CHECK(RetVal) \
{                                                         \
if ( (RetVal) != LSA_RET_OK )                     \
EDDS_FatalError(EDDS_FATAL_ERR_MEM_FREE,  \
EDDS_MODULE_ID,             \
__LINE__);                  \
}
#else
#define EDDS_FREE_CHECK(RetVal) {}
#endif

/*---------------------------------------------------------------------------*/
/* Debugging (enables traces)                                                */
/*                                                                           */
/* meaning see edd_cfg.h                                                    */
/*---------------------------------------------------------------------------*/
#if defined(EDDS_CFG_DEBUG_ENABLE) || defined(_lint) || defined(lint)
#define EDDS_IS_VALID_PTR_HANDLER(ptr)  if(LSA_HOST_PTR_ARE_EQUAL(LSA_NULL,ptr))\
                                        {\
                                            EDDS_FatalError(EDDS_FATAL_ERR_NULL_PTR,EDDS_MODULE_ID,__LINE__);\
                                        }

#define EDDS_ASSERT(Cond_) \
{                                                         \
if( ! (Cond_) )                                         \
{                                                     \
EDDS_FatalError( EDDS_FATAL_ERR_INCONSISTENCE,     \
EDDS_MODULE_ID,                    \
__LINE__);                        \
}                                                     \
}
#else
#define EDDS_IS_VALID_PTR_HANDLER(ptr)                            {}
#define EDDS_ASSERT(Condition) {}
#endif

#define EDDS_IS_VALID_PTR(ptr) EDDS_IS_VALID_PTR_HANDLER(ptr)

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/

#endif  /* of EDDS_DBG_H */
/*****************************************************************************/
/*  end of file EDDS_DBG.H                                                */
/*****************************************************************************/
