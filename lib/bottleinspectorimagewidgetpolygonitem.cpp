#include "bottleinspectorimagewidgetpolygonitem.h"

#include "bottleinspectorimagewidgetpolygonhandleitem.h"

BottleInspectorImageWidgetPolygonItem::BottleInspectorImageWidgetPolygonItem(QPolygonF polygon, QGraphicsItem *parent) :
    QGraphicsPolygonItem(polygon, parent)
{
//    _rangeType = rangeType;
//    _allowedRect = allowedRect;

//    // create handles
//    _handleTop = new BottleInspectorImageWidgetRangeHandleItem(ResizeHandleTop, this);
//    _handleBottom = new BottleInspectorImageWidgetRangeHandleItem(ResizeHandleBottom, this);
//    _handleCenter = new BottleInspectorImageWidgetRangeHandleItem(MoveHandle, this);


    for (int i=0; i < polygon.size(); i++) {
        _handles << new BottleInspectorImageWidgetPolygonHandleItem(i, this);
    }
}
