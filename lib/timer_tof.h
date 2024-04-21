#ifndef TIMER_TOF_H
#define TIMER_TOF_H

#include <QElapsedTimer>

class Timer_TOF
{
public:
    Timer_TOF(int64_t timeout = 0);

    bool update(bool input);
    bool state() const;

private:
    int64_t _timeout;

    QElapsedTimer _timer;
};

#endif // TIMER_TOF_H
