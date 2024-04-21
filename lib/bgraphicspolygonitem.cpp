#include "bgraphicspolygonitem.h"

#include <QGraphicsPixmapItem>


BGraphicsPolygonItem::BGraphicsPolygonItem() :
    BAbstractGraphicsShapeItem()
{
}

void BGraphicsPolygonItem::moveBy(qreal dx, qreal dy)
{
    _polygon.translate(dx, dy);
}

QGraphicsItem * BGraphicsPolygonItem::toGraphicsItem() const
{
    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(polygon());
    item->setPen(pen());
    item->setBrush(brush());
    item->setToolTip(toolTip());

    return item;
}
