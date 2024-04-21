#include "bgraphicspixmapitem.h"

#include <QtCore>
#include <QGraphicsPixmapItem>


BGraphicsPixmapItem::BGraphicsPixmapItem() :
    BGraphicsItem()
{
}

QGraphicsItem * BGraphicsPixmapItem::toGraphicsItem() const
{
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap());
    item->setToolTip(toolTip());

    return item;
}
