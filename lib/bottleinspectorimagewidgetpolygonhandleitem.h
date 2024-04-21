#ifndef BOTTLEINSPECTORIMAGEWIDGETPOLYGONHANDLEITEM_H
#define BOTTLEINSPECTORIMAGEWIDGETPOLYGONHANDLEITEM_H

#include <QGraphicsRectItem>

#include "bottleinspectorimagewidgetpolygonitem.h"

class BottleInspectorImageWidgetPolygonHandleItem : public QGraphicsRectItem
{
public:
    BottleInspectorImageWidgetPolygonHandleItem(int pos, BottleInspectorImageWidgetPolygonItem *parent);

    void updatePos();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    int _pos;
};

#endif // BOTTLEINSPECTORIMAGEWIDGETPOLYGONHANDLEITEM_H
