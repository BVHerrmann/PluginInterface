#include "timer_tof.h"

Timer_TOF::Timer_TOF(int64_t timeout)
{
    _timeout = timeout;
    _timer.invalidate();
}

bool Timer_TOF::update(bool input)
{
    bool result = false;

    if (input) {
        _timer.start();
        result = true;
    } else if (_timer.isValid()) {
        result = !_timer.hasExpired(_timeout);
    }

    return result;
}

bool Timer_TOF::state() const
{
    return _timer.isValid() && !_timer.hasExpired(_timeout);
}
