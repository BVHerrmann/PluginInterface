#include "edge_trig.h"

Edge_TRIG::Edge_TRIG()
{
    _initialized = false;
    _previous = false;
    _current = false;
}

void Edge_TRIG::update(bool input)
{
    _previous = _current;

    if (_initialized) {
        _current = input;
    } else {
        _initialized = true;
        _previous = input;
    }
}

bool Edge_TRIG::high()
{
    return _current;
}

bool Edge_TRIG::low()
{
    return !_current;
}

bool Edge_TRIG::flank()
{
    return _previous != _current;
}

bool Edge_TRIG::risingEdge()
{
    return !_previous && _current;
}

bool Edge_TRIG::fallingEdge()
{
    return _previous && !_current;
}
