#ifndef BGRAPHICSPATHITEM_H
#define BGRAPHICSPATHITEM_H

#include "babstractgraphicsshapeitem.h"

#include <QPainterPath>


class BGraphicsPathItem : public BAbstractGraphicsShapeItem
{
public:
    explicit BGraphicsPathItem();
    
    explicit BGraphicsPathItem(const QPainterPath &path) : BAbstractGraphicsShapeItem() {
        setPath(path);
    }

    explicit BGraphicsPathItem(const QPolygonF &polygon) : BAbstractGraphicsShapeItem() {
        setPath(polygon);
    }

    QPainterPath path() const { return _path; }
    void setPath(const QPainterPath &path) { _path = path; }
    void setPath(const QPolygonF &polygon);

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QPainterPath _path;
};

#endif // BGRAPHICSPATHITEM_H
