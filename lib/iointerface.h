#ifndef IOINTERFACE_H
#define IOINTERFACE_H

#include <QtCore>


class IOInterface : public QObject
{
    Q_OBJECT
public:
    explicit IOInterface();
    virtual ~IOInterface();
    
    virtual void writeOutputs() = 0;
};

#endif // IOINTERFACE_H
