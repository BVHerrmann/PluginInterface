#include "imagewidgetareaitem.h"

#include "imagewidgetareahandleitem.h"


ImageWidgetAreaItem::ImageWidgetAreaItem(QRectF allowedRect, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, w, h, parent)
{
    _allowedRect = allowedRect;

    // create handles
    _handleTopLeft = new ImageWidgetAreaHandleItem(ResizeHandleTopLeft, this);
    _handleTopRight = new ImageWidgetAreaHandleItem(ResizeHandleTopRight, this);
    _handleBottomLeft = new ImageWidgetAreaHandleItem(ResizeHandleBottomLeft, this);
    _handleBottomRight = new ImageWidgetAreaHandleItem(ResizeHandleBottomRight, this);
}

void ImageWidgetAreaItem::setRect(const QRectF &rect)
{
    QGraphicsRectItem::setRect(rect);

    _handleTopLeft->updatePos();
    _handleTopRight->updatePos();
    _handleBottomLeft->updatePos();
    _handleBottomRight->updatePos();
}
