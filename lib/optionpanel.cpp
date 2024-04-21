#include "optionpanel.h"

#include <QtWidgets>


OptionPanel::OptionPanel(QWidget *parent) : QWidget(parent)
{
    
}

void OptionPanel::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
