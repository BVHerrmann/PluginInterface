#include "bgraphicskeypointitem.h"

#include <QGraphicsEllipseItem>


BGraphicsKeyPointItem::BGraphicsKeyPointItem() :
    BAbstractGraphicsShapeItem()
{
}

QGraphicsItem * BGraphicsKeyPointItem::toGraphicsItem() const
{
    float size = _keypoint.size;
    if (!size)
        size = 1.5;
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem(_keypoint.pt.x - (size / 2.0), _keypoint.pt.y - (size / 2.0), size, size);
    item->setPen(pen());
    item->setBrush(brush());

//    item->setPen(QPen(QColor(HMIColor::red), 0));
//    if (_keypoint.response < 0.1)
//        item->setPen(QPen(QColor(std::min((int) (_keypoint.response * 2), 255), 0, 0, std::min((int) (_keypoint.response * 3), 255)), 0));
//    else
//        item->setPen(QPen(QColor(std::min((int) (_keypoint.response * 1000 * 3), 255), 0, 0, std::min((int) (_keypoint.response * 1000 * 3), 255)), 0));
    item->setToolTip(toolTip());

    return item;
}
