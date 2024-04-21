#ifndef EDGE_TRIG_H
#define EDGE_TRIG_H

class Edge_TRIG
{
public:
    Edge_TRIG();

    void update(bool input);

    bool high();
    bool low();

    bool flank();
    bool risingEdge();
    bool fallingEdge();

private:
    bool _initialized;
    bool _current;
    bool _previous;
};

#endif // EDGE_TRIG_H
