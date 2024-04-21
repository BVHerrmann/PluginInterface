#ifndef IMAGEWIDGETRADIALAREAHANDLEITEM_H
#define IMAGEWIDGETRADIALAREAHANDLEITEM_H

#include <QGraphicsRectItem>

#include "bottleinspectorimagewidgetradialareaitem.h"

typedef enum {
    ResizeHandleTop,
    ResizeHandleBottom,
    MoveHandle
} RadialAreaHandleType;


class BottleInspectorImageWidgetRadialAreaHandleItem : public QGraphicsRectItem
{
public:
    explicit BottleInspectorImageWidgetRadialAreaHandleItem(RadialAreaHandleType handleType, BottleInspectorImageWidgetRadialAreaItem *parent = 0);

    void updatePos();

    RadialAreaHandleType handleType() const { return _handleType; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    RadialAreaHandleType _handleType;
};

#endif // IMAGEWIDGETRADIALAREAHANDLEITEM_H
