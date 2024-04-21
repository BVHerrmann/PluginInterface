#ifndef TIMER_TP_H
#define TIMER_TP_H

#include <QElapsedTimer>

class Timer_TP
{
public:
    Timer_TP(int64_t timeout);

    bool update(bool input);

private:
    int64_t _timeout;

    QElapsedTimer _timer;
};

#endif // TIMER_TP_H
