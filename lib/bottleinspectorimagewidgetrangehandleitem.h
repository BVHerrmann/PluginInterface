#ifndef BOTTLEINSPECTORIMAGEWIDGETRANGEHANDLEITEM_H
#define BOTTLEINSPECTORIMAGEWIDGETRANGEHANDLEITEM_H


#include <QGraphicsRectItem>

#include "bottleinspectorimagewidgetrangeitem.h"


typedef enum {
    ResizeHandleTop,
    ResizeHandleBottom,
    MoveHandle
} RangeHandleType;


class BottleInspectorImageWidgetRangeHandleItem : public QGraphicsRectItem
{
public:
    explicit BottleInspectorImageWidgetRangeHandleItem(RangeHandleType handleType, BottleInspectorImageWidgetRangeItem *parent = 0);

    void updatePos();

    RangeHandleType handleType() const { return _handleType; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    RangeHandleType _handleType;
};

#endif // BOTTLEINSPECTORIMAGEWIDGETRANGEHANDLEITEM_H
