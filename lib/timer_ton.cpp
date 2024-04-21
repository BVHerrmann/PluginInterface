#include "timer_ton.h"

Timer_TON::Timer_TON(int64_t timeout)
{
    _timeout = timeout;
    _timer.invalidate();
}

bool Timer_TON::update(bool input)
{
    bool result = false;

    if (input && _timer.isValid()) {
        result = _timer.hasExpired(_timeout);
    } else if (input) {
        _timer.start();
    } else {
        _timer.invalidate();
    }

    return result;
}
