#include "audittrail.h"

AuditTrailEvent::AuditTrailEvent(const QString &message) : QEvent(AuditTrailEventType)
{
    _message = message;
}

const QString AuditTrailEvent::message() const
{
    return _message;
}
