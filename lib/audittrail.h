#ifndef AUDITTRAIL_H
#define AUDITTRAIL_H

#include <QtCore>

const char * const kAuditTrail = "auditTrail";

static const QEvent::Type AuditTrailEventType = (QEvent::Type)63525;

class AuditTrailEvent : public QEvent
{
public:
    AuditTrailEvent(const QString &message);
    const QString message() const;
    
private:
    QString _message;
};

class AuditTrail
{
public:
    virtual ~AuditTrail() {}
    
    static void message(const QString &message) {
        QCoreApplication::postEvent(qApp, new AuditTrailEvent(message));
    }
};

#endif // AUDITTRAIL_H
