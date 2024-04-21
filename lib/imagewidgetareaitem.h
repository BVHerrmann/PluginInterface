#ifndef IMAGEWIDGETAREAITEM_H
#define IMAGEWIDGETAREAITEM_H

#include <QGraphicsRectItem>

class ImageWidgetAreaHandleItem;


class ImageWidgetAreaItem : public QGraphicsRectItem
{
public:
    explicit ImageWidgetAreaItem(QRectF allowedRect, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

    void setRect(const QRectF &rect);

    QRectF allowedRect() const { return _allowedRect; }
    void setAllowedRect(QRectF value) { _allowedRect = value; }

protected:
    QRectF _allowedRect;

    ImageWidgetAreaHandleItem *_handleTopLeft;
    ImageWidgetAreaHandleItem *_handleTopRight;
    ImageWidgetAreaHandleItem *_handleBottomLeft;
    ImageWidgetAreaHandleItem *_handleBottomRight;
};

#endif // IMAGEWIDGETAREAITEM_H
