#ifndef BGRAPHICSPOLYGONITEM_H
#define BGRAPHICSPOLYGONITEM_H

#include "babstractgraphicsshapeitem.h"

#include <QPolygonF>

#include <opencv2/core.hpp>


class BGraphicsPolygonItem : public BAbstractGraphicsShapeItem
{
public:
    explicit BGraphicsPolygonItem();
    
    explicit BGraphicsPolygonItem(const QPolygonF &polygon) : BAbstractGraphicsShapeItem() {
        setPolygon(polygon);
    }

    explicit BGraphicsPolygonItem(const std::vector<cv::Point> &polygon) : BAbstractGraphicsShapeItem() {
        for (auto it = polygon.cbegin(); it != polygon.end(); ++it) {
            _polygon << QPointF((*it).x, (*it).y);
        }
    }

    QPolygonF polygon() const { return _polygon; }
    void setPolygon(const QPolygonF &polygon) { _polygon = polygon; }

    void moveBy(qreal dx, qreal dy);

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QPolygonF _polygon;
};

#endif // BGRAPHICSPOLYGONITEM_H
