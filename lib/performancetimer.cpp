//
//  PerformanceTimer.cpp
//  Inspector
//
//  Created by Jendrik Bertram on 03.06.20.
//

#include "performancetimer.h"

PerformanceTimer::PerformanceTimer(QString title) {
    auto start_time = std::chrono::high_resolution_clock::now();
    timepoints.push_back(std::make_pair(start_time, title));
}

PerformanceTimer::~PerformanceTimer() {
    stop();
}

void PerformanceTimer::stop() {
    if (_stop_called) {
        return;
    } else {
        _stop_called = true;
    }
    
    auto stop_time = std::chrono::high_resolution_clock::now();
    auto prev_time = timepoints.front().first;
    auto total = stop_time - timepoints.front().first;
    
    if (!timepoints.front().second.isNull()) {
        qDebug() << "Timing for" << timepoints.front().second;
    }
    
    for (int i=1; i < timepoints.size(); ++i) {
        auto time_point = timepoints[i];
        
        double percentage = 100.0 * (time_point.first - prev_time).count() / total.count();
        if (time_point.second.isNull()) {
            qDebug() << qPrintable(QString("Finished intermediate %1 in %2ms (%3%)").arg(i).arg(std::chrono::duration_cast<std::chrono::microseconds>(time_point.first - prev_time).count() * 0.001, 0, 'f', 2).arg(percentage, 0, 'f', 2));
        } else {
            qDebug() << qPrintable(QString("Finished intermediate \"%1\" in %2ms (%3%)").arg(time_point.second).arg(std::chrono::duration_cast<std::chrono::microseconds>(time_point.first - prev_time).count() * 0.001, 0, 'f', 2).arg(percentage, 0, 'f', 2));
        }
        
        prev_time = time_point.first;
    }
    
    if (timepoints.front().second.isNull()) {
        qDebug() << qPrintable(QString("Finished in %1ms").arg(std::chrono::duration_cast<std::chrono::microseconds>(total).count() * 0.001, 0, 'f', 2));
    } else {
        qDebug() << qPrintable(QString("Finished \"%1\" in %2ms").arg(timepoints.front().second).arg(std::chrono::duration_cast<std::chrono::microseconds>(total).count() * 0.001, 0, 'f', 2));
    }
}

void PerformanceTimer::intermediate(QString title) {
    auto intermediate_time = std::chrono::high_resolution_clock::now();
    timepoints.push_back(std::make_pair(intermediate_time, title));
}
