#ifndef BOTTLEINSPECTORIMAGEWIDGETPOLYGONITEM_H
#define BOTTLEINSPECTORIMAGEWIDGETPOLYGONITEM_H

#include <QGraphicsPolygonItem>

class BottleInspectorImageWidgetPolygonHandleItem;


class BottleInspectorImageWidgetPolygonItem : public QGraphicsPolygonItem
{
public:
    BottleInspectorImageWidgetPolygonItem(QPolygonF polygon, QGraphicsItem *parent = 0);

protected:
    QList<BottleInspectorImageWidgetPolygonHandleItem *> _handles;
};

#endif // BOTTLEINSPECTORIMAGEWIDGETPOLYGONITEM_H
