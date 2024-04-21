#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>
#include <QHash>
#include <QVariant>


class Plugin : public QObject
{
    Q_OBJECT
public:
    explicit Plugin(QObject *parent = 0);
    
    // CommunicationInterface
    virtual void setValue(const QString &name, const QVariant &value) { (void)name; (void)value; assert(false); }
    virtual void setValues(const QHash<QString, QVariant> &values) { (void)values; assert(false); }

signals:
    
public slots:
    // CommunicationInterface
    void setValue(const QObject * sender, const QString &name, const QVariant &value) { if (sender != this) setValue(name, value); }
    void setValues(const QObject * sender, const QHash<QString, QVariant> &values) { if (sender != this) setValues(values); }
};

#endif // PLUGIN_H
