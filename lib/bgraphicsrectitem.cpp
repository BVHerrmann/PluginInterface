#include "bgraphicsrectitem.h"

#include <QtCore>
#include <QGraphicsRectItem>


BGraphicsRectItem::BGraphicsRectItem() :
    BAbstractGraphicsShapeItem()
{
}

QGraphicsItem * BGraphicsRectItem::toGraphicsItem() const
{
    QGraphicsRectItem *item = new QGraphicsRectItem(rect());
    item->setPen(pen());
    item->setBrush(brush());
    item->setToolTip(toolTip());

    return item;
}
