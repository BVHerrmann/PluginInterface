#ifndef BGRAPHICSELLIPSEITEM_H
#define BGRAPHICSELLIPSEITEM_H

#include "bgraphicsitem.h"

#include <QRectF>


class BGraphicsEllipseItem : public BGraphicsItem
{
public:
    explicit BGraphicsEllipseItem();

    explicit BGraphicsEllipseItem(const QRectF &rect) : BGraphicsItem() {
        setRect(rect);
    }

    QRectF rect() const { return _rect; }
    void setRect(const QRectF &rect) { _rect = rect; }

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QRectF _rect;

};

#endif // BGRAPHICSELLIPSEITEM_H
