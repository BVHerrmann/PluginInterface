#ifndef TIMER_TON_H
#define TIMER_TON_H

#include <QElapsedTimer>

class Timer_TON
{
public:
    Timer_TON(int64_t timeout);

    bool update(bool input);

private:
    int64_t _timeout;

    QElapsedTimer _timer;
};

#endif // TIMER_TON_H
