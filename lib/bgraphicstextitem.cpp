#include "bgraphicstextitem.h"

#include <QGraphicsTextItem>


BGraphicsTextItem::BGraphicsTextItem() :
    BGraphicsItem()
{
}

QGraphicsItem * BGraphicsTextItem::toGraphicsItem() const
{
    QGraphicsTextItem *item = new QGraphicsTextItem(text());
    item->setDefaultTextColor(pen().color());
    item->setPos(pos());
    item->setToolTip(toolTip());

    return item;
}
