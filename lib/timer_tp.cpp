#include "timer_tp.h"

Timer_TP::Timer_TP(int64_t timeout)
{
    _timeout = timeout;
    _timer.invalidate();
}

bool Timer_TP::update(bool input)
{
    bool result = false;

    if (_timer.isValid()) {
        result = !_timer.hasExpired(_timeout);
    } else if (input) {
        _timer.start();
        result = true;
    } else {
        _timer.invalidate();
    }

    return result;
}
