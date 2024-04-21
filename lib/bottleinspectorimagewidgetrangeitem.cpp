#include "bottleinspectorimagewidgetrangeitem.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "bottleinspectorimagewidgetrangehandleitem.h"


BottleInspectorImageWidgetRangeItem::BottleInspectorImageWidgetRangeItem(RangeType rangeType, QRectF allowedRect, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, w, h, parent)
{
    _rangeType = rangeType;
    _allowedRect = allowedRect;

    // create handles
    _handleTop = new BottleInspectorImageWidgetRangeHandleItem(ResizeHandleTop, this);
    _handleBottom = new BottleInspectorImageWidgetRangeHandleItem(ResizeHandleBottom, this);
    _handleCenter = new BottleInspectorImageWidgetRangeHandleItem(MoveHandle, this);
}

void BottleInspectorImageWidgetRangeItem::setRect(const QRectF &rect)
{
    QGraphicsRectItem::setRect(rect);

    _handleTop->updatePos();
    _handleBottom->updatePos();
    _handleCenter->updatePos();
}
