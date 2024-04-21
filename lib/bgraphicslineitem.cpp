#include "bgraphicslineitem.h"

#include <QGraphicsLineItem>


BGraphicsLineItem::BGraphicsLineItem() :
    BGraphicsItem()
{
}

QGraphicsItem * BGraphicsLineItem::toGraphicsItem() const
{
    QGraphicsLineItem *item = new QGraphicsLineItem(line());
    item->setPen(pen());
    item->setToolTip(toolTip());

    return item;
}
