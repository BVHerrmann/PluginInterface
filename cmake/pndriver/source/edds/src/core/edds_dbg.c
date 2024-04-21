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
/*  F i l e               &F: edds_dbg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDS-Debugging support functions                 */
/*                          (system dependend)                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/


/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID  4
#define EDDS_MODULE_ID    LTRC_ACT_MODUL_ID /* EDDS_MODULE_ID_DBG */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "edds_int.h"            /* internal header */


EDDS_FILE_SYSTEM_EXTENSION(EDDS_MODULE_ID)

LSA_VOID EDDS_INTERNAL_PERFORMANCE_STATISTICS_INIT(EDDS_DDB_TYPE * const pDDB)
{
    EDDS_INTERNAL_STATISTICS_INIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsAll);
    EDDS_INTERNAL_STATISTICS_INIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsCyclic);
    EDDS_INTERNAL_STATISTICS_INIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsIntermediate);
    EDDS_INTERNAL_STATISTICS_INIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsRecurringTask);
    EDDS_INTERNAL_STATISTICS_INIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsPhyEvent);
    EDDS_INTERNAL_STATISTICS_INIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsHandleRequest);
#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
    EDDS_INTERNAL_STATISTICS_JITTER_INIT(pDDB->PerformanceStatistics.cylceJitter); //lint !e569 TH remove warning for debug statistics (not relevant)
#endif
}

LSA_VOID EDDS_INTERNAL_PERFORMANCE_STATISTICS_DEINIT(EDDS_DDB_TYPE * const pDDB)
{
    EDDS_INTERNAL_STATISTICS_DEINIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsAll);
    EDDS_INTERNAL_STATISTICS_DEINIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsCyclic);
    EDDS_INTERNAL_STATISTICS_DEINIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsIntermediate);
    EDDS_INTERNAL_STATISTICS_DEINIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsRecurringTask);
    EDDS_INTERNAL_STATISTICS_DEINIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsPhyEvent);
    EDDS_INTERNAL_STATISTICS_DEINIT(pDDB->hSysDev,pDDB->PerformanceStatistics.callsHandleRequest);
#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
    EDDS_INTERNAL_STATISTICS_JITTER_DEINIT(pDDB->PerformanceStatistics.cylceJitter); //lint !e713 !e732 !e776 TH remove warning for debug statistics (not relevant)
#endif
}

#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
#define EDDS_DBG_TRACE_TIME_CALC(timestat) \
    min =  (LSA_UINT32)timestat.min;\
    max =  (LSA_UINT32)timestat.max;\
    if(timestat.sumcount)\
    {\
        average = (LSA_UINT32)(timestat.timesum / timestat.sumcount);\
    }\
    else\
    {\
        average = 0;\
    }\
    count = (LSA_UINT32)timestat.sumcount;
#endif

LSA_VOID EDDS_INTERNAL_PERFORMANCE_STATISTICS_TRACE(EDDS_DDB_TYPE const * const pDDB)
{
#if defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT)
    LSA_UINT32 min;
    LSA_UINT32 max;
    LSA_UINT32 average;
    LSA_UINT32 count;
    LSA_UINT32 TraceIdx = pDDB->pGlob->TraceIdx;
    EDDS_INTERNAL_PERFORMANCE_STATS_TYPE const * const ps = &pDDB->PerformanceStatistics;

    EDDS_DBG_TRACE_TIME_CALC(ps->callsAll);
    EDDS_SYSTEM_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: processing time of all edds_scheduler calls: min/max/average in nanoseconds (quantity): %d/%d/%d (%d)",
                         min,
                         max,
                         average,
                         count);
    EDDS_DBG_TRACE_TIME_CALC(ps->callsCyclic);
    EDDS_SYSTEM_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: processing time of cyclic edds_scheduler calls: min/max/average in nanoseconds (quantity): %d/%d/%d (%d)",
                         min,
                         max,
                         average,
                         count);
    EDDS_DBG_TRACE_TIME_CALC(ps->callsIntermediate);
    EDDS_SYSTEM_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: processing time of intermediate edds_scheduler calls: min/max/average in nanoseconds (quantity): %d/%d/%d (%d)",
                         min,
                         max,
                         average,
                         count);
    EDDS_DBG_TRACE_TIME_CALC(ps->callsRecurringTask);
    EDDS_SYSTEM_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: processing time of recurring task calls: min/max/average in nanoseconds (quantity): %d/%d/%d (%d)",
                         min,
                         max,
                         average,
                         count);
    EDDS_DBG_TRACE_TIME_CALC(ps->callsPhyEvent);
    EDDS_SYSTEM_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: processing time of phy event calls: min/max/average in nanoseconds (quantity): %d/%d/%d (%d)",
                         min,
                         max,
                         average,
                         count);
    EDDS_DBG_TRACE_TIME_CALC(ps->callsHandleRequest);
    EDDS_SYSTEM_TRACE_04(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: processing time of handle request calls: min/max/average in nanoseconds (quantity): %d/%d/%d (%d)",
                         min,
                         max,
                         average,
                         count);
    {
        LSA_INT32 min; //lint !e578 TH wanted behaviour
        LSA_INT32 max; //lint !e578 TH wanted behaviour
        LSA_INT32 averageP;
        LSA_INT32 averageN;
        LSA_UINT32 countP;
        LSA_UINT32 countN;
        LSA_UINT32 bouncing;
        LSA_UINT32 skipping;

        min =  ps->cylceJitter.min;
        max =  ps->cylceJitter.max;
        if(ps->cylceJitter.sumcountP)
        {
            averageP = (LSA_INT32)(ps->cylceJitter.jittersumP / ps->cylceJitter.sumcountP);
        }
        else
        {
            averageP = 0;
        }
        if(ps->cylceJitter.sumcountN)
        {
            averageN = -((LSA_INT32)(ps->cylceJitter.jittersumN / ps->cylceJitter.sumcountN));
        }
        else
        {
            averageN = 0;
        }
        countP = ps->cylceJitter.sumcountP;
        countN = ps->cylceJitter.sumcountN;
        bouncing = ps->cylceJitter.bouncing;
        skipping = ps->cylceJitter.skipping;

        EDDS_SYSTEM_TRACE_08(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                             "PerfStats: relative jitter of cylce: min/max/-average/+average in nanoseconds (-/+ quantities); skips/bounces: %i/%i/%i/%i (%d/%d); %d/%d",
                             min,
                             max,
                             averageN,
                             averageP,
                             countN,
                             countP,
                             skipping,
                             bouncing);
    }
#else   /* defined(EDDS_CFG_DEBUG_RUNTIME_MEASUREMENT) */
    LSA_UINT32 TraceIdx = pDDB->pGlob->TraceIdx;
    #if (EDDS_CFG_TRACE_MODE != 0)
    EDDS_INTERNAL_PERFORMANCE_STATS_TYPE const * const ps = &pDDB->PerformanceStatistics;
    #endif
    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: number of all edds_scheduler calls: %d",
                         (LSA_UINT32)ps->callsAll);
    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: number of cyclic edds_scheduler calls: %d",
                         (LSA_UINT32)ps->callsCyclic);
    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: number of intermediate edds_scheduler calls: %d",
                         (LSA_UINT32)ps->callsIntermediate);
    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: number of recurring task calls: %d",
                         (LSA_UINT32)ps->callsRecurringTask);
    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: number of phy event calls: %d",
                         (LSA_UINT32)ps->callsPhyEvent);
    EDDS_SYSTEM_TRACE_01(TraceIdx,LSA_TRACE_LEVEL_NOTE_HIGH,
                         "PerfStats: number of handle request calls: %d",
                         (LSA_UINT32)ps->callsHandleRequest);
#endif
    LSA_UNUSED_ARG(TraceIdx);   /* probably unused... */
}
