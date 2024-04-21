#include "bgraphicsellipseitem.h"

#include <QGraphicsEllipseItem>


BGraphicsEllipseItem::BGraphicsEllipseItem() :
    BGraphicsItem()
{
}

QGraphicsItem * BGraphicsEllipseItem::toGraphicsItem() const
{
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem(rect());
	//item->setStartAngle(0);
	//item->setSpanAngle(90 * 16);
    item->setPen(pen());
    item->setToolTip(toolTip());

    return item;
}
