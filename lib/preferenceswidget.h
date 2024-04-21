#ifndef PREFERENCESWIDGET_H
#define PREFERENCESWIDGET_H

#include <QWidget>


const char * const kPropertyKey = "propertyKey";

class PreferencesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PreferencesWidget(QWidget *parent = 0);

    void showEvent(QShowEvent *event);

signals:

public slots:
    void changeValue(bool value);
    void changeValue(int value);
    void changeValue(double value);
    void changeValue(const QString &value);

    void changeIndex(int index);
    
protected:
    void dynamicLoadPreferences(QWidget *widget);

    void changeValue(QObject *sender, QVariant value);

};

#endif // PREFERENCESWIDGET_H
