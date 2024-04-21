#ifndef BSWITCH_H
#define BSWITCH_H

#include <QAbstractButton>
#include <QResizeEvent>

#include "colors.h"


class BSwitch : public QAbstractButton
{
    Q_OBJECT
public:
    BSwitch(QWidget *parent = 0);
    
    void setCheckedColor(const QColor &color) { _checkedColor = color; }
    QColor checkedColor() const { return _checkedColor; }
    
    void setUncheckedColor(const QColor &color) { _uncheckedColor = color; }
    QColor uncheckedColor() const { return _uncheckedColor; }
    
protected:
    virtual void paintEvent (QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    
private:
    QColor _checkedColor = HMIColor::OK;
    QColor _uncheckedColor = HMIColor::Light;
};

#endif // BSWITCH_H
