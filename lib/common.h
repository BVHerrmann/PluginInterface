#ifndef COMMON_H
#define COMMON_H

#if defined(Q_OS_WIN)
    #define FUNCTION_NAME __FUNCTION__
#else
    #define FUNCTION_NAME __func__
#endif

#define MEASURE_PERFORMANCE

#ifdef MEASURE_PERFORMANCE
#include "performancetimer.h"
#define START_MEASURE_PERFORMANCE() \
    PerformanceTimer _performance_timer(__FUNCTION__);
#define INTERMEDIATE_MEASURE_PERFORMANCE() \
    _performance_timer.intermediate();
#define INTERMEDIATE_MEASURE_PERFORMANCE2(description) \
    _performance_timer.intermediate(description);
#define STOP_MEASURE_PERFORMANCE() \
    _performance_timer.stop();
#else
#define START_MEASURE_PERFORMANCE()
#define INTERMEDIATE_MEASURE_PERFORMANCE()
#define STOP_MEASURE_PERFORMANCE()
#endif

#define ENSURE_ONCE() bool _run = property(FUNCTION_NAME).toBool(); if (!_run) { setProperty(FUNCTION_NAME, true); } else { return; }

bool isDebuggerAttached();

#endif // COMMON_H
