#ifndef BOTTLEINSPECTORIMAGEWIDGETRANGEITEM_H
#define BOTTLEINSPECTORIMAGEWIDGETRANGEITEM_H


#include <QGraphicsRectItem>

class BottleInspectorImageWidgetRangeHandleItem;


typedef enum {
    RangeProfile,
    RangeKeypoints
} RangeType;


class BottleInspectorImageWidgetRangeItem : public QGraphicsRectItem
{
public:
    explicit BottleInspectorImageWidgetRangeItem(RangeType rangeType, QRectF allowedRect, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

    void setRect(const QRectF &rect);

    RangeType rangeType() const { return _rangeType; }
    QRectF allowedRect() const { return _allowedRect; }

protected:
    RangeType _rangeType;
    QRectF _allowedRect;

    BottleInspectorImageWidgetRangeHandleItem *_handleTop;
    BottleInspectorImageWidgetRangeHandleItem *_handleBottom;
    BottleInspectorImageWidgetRangeHandleItem *_handleCenter;
};

#endif // BOTTLEINSPECTORIMAGEWIDGETRANGEITEM_H
