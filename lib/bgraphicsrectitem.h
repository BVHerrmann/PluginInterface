#ifndef BGRAPHICSRECTITEM_H
#define BGRAPHICSRECTITEM_H

#include "babstractgraphicsshapeitem.h"


class BGraphicsRectItem : public BAbstractGraphicsShapeItem
{
public:
    explicit BGraphicsRectItem();
    
    explicit BGraphicsRectItem(const QRectF &rect) : BAbstractGraphicsShapeItem() {
        setRect(rect);
    }

    explicit BGraphicsRectItem(const QRect &rect) : BAbstractGraphicsShapeItem() {
        setRect(rect);
    }

    explicit BGraphicsRectItem(double x, double y, double width, double height) : BAbstractGraphicsShapeItem() {
        setRect(x, y, width, height);
    }

    QRectF rect() const { return _rect; }
    void setRect(const QRectF &rect) { _rect = rect; }
    void setRect(const QRect &rect) { _rect = QRectF(rect); }
    void setRect(double x, double y, double width, double height) { _rect = QRectF(x, y, width, height); }

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QRectF _rect;
};

#endif // BGRAPHICSRECTITEM_H
