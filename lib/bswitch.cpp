#include "bswitch.h"

#include <QPainter>


BSwitch::BSwitch(QWidget *parent) : QAbstractButton(parent)
{
    setMinimumSize(64, 32);
    setCheckable(true);
}

void BSwitch::resizeEvent(QResizeEvent *event)
{
    (void)event;

    update();
}

void BSwitch::paintEvent(QPaintEvent *event)
{
    (void)event;

    qreal width = this->width();
    qreal height = this->height();
    if (width > 2 * height) {
        width = 2 * height;
    } else if (height * 2 > width) {
        height = width / 2;
    }
    qreal margin = height / 10.0;
    qreal radius = height / 2.0 - margin;
    
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    
    painter.setRenderHint(QPainter::Antialiasing);
    if (isEnabled()) {
        if (isChecked()) {
            painter.setBrush(_checkedColor);
        } else {
            painter.setBrush(_uncheckedColor);
        }
    } else {
        painter.setBrush(HMIColor::LightGrey);
    }
    
    painter.drawRoundedRect(QRect(rect().topLeft(), QSize(width, height)), height / 2.0, height / 2.0);
    
    if (isEnabled()) {
        painter.setBrush(HMIColor::DarkGrey);
    } else {
        painter.setBrush(HMIColor::Grey);
    }

    if (isChecked()) {
        painter.drawEllipse(QPointF(width - (height / 2.0), height / 2.0), radius, radius);
    } else {
        painter.drawEllipse(QPointF(height / 2.0, height / 2.0), radius, radius);
    }
}
