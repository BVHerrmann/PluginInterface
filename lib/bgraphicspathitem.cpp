#include "bgraphicspathitem.h"

#include <QtCore>
#include <QGraphicsPathItem>


BGraphicsPathItem::BGraphicsPathItem() :
    BAbstractGraphicsShapeItem()
{
}

void BGraphicsPathItem::setPath(const QPolygonF &polygon)
{
    QPainterPath path;
    QPointF prev;

    for (const QPointF &p : polygon) {
        if (prev.isNull() || std::fabs(prev.y() - p.y()) > 1.0)
            path.moveTo(p);
        else
            path.lineTo(p);
        prev = p;
    }

    // set created path
    _path = path;
}

QGraphicsItem * BGraphicsPathItem::toGraphicsItem() const
{
    QGraphicsPathItem *item = new QGraphicsPathItem(path());
    item->setPen(pen());
    item->setBrush(brush());
    item->setToolTip(toolTip());

    return item;
}
