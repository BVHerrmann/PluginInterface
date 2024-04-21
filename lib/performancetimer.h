//
//  PerformanceTimer.hpp
//  Inspector
//
//  Created by Jendrik Bertram on 03.06.20.
//

#ifndef PEROFRMANCETIMER_H
#define PEROFRMANCETIMER_H

#include <chrono>

#include <QtCore>


class PerformanceTimer {
    
public:
    explicit PerformanceTimer(QString title = QString());
    ~PerformanceTimer();
    
    void stop();
    
    void intermediate(QString title = QString());
    
private:
    bool _stop_called = false;
    
    std::vector<std::pair<std::chrono::high_resolution_clock::time_point, QString>> timepoints;
};

#endif /* PEROFRMANCETIMER_H */
