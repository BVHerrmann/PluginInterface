#include "contentboarditem.h"

#include <QtWidgets>


ContentBoardItem::ContentBoardItem(QWidget *parent) : QWidget(parent)
{

}

void ContentBoardItem::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
