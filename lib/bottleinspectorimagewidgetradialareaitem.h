#ifndef IMAGEWIDGETRADIALAREAITEM_H
#define IMAGEWIDGETRADIALAREAITEM_H

#include <QGraphicsItem>

#include "bottleinspector.h"

class BottleInspectorImageWidgetRadialAreaHandleItem;


class BottleInspectorImageWidgetRadialAreaItem : public QGraphicsItem
{
public:
    BottleInspectorImageWidgetRadialAreaItem(std::shared_ptr<BottleInspector> inspector, double roiBottom, double roiTop, QGraphicsItem *parent = 0);

    // QGraphicsItem
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    std::shared_ptr<BottleInspector> inspector() const { return _inspector; }

    inline double roiBottom() const { return _roiBottom; }
    void setROIBottom(double value) { _roiBottom = value; }

    inline double roiTop() const { return _roiTop; }
    void setROITop(double value) { _roiTop = value; }

    inline double roiCenter() const { return (_roiTop - _roiBottom) / 2.0 + _roiBottom; }

    QPointF topLeft();
    void setTopLeft(QPointF &pos);

    QPointF bottomLeft();
    void setBottomLeft(QPointF &pos);

    QPointF centerRight();
    void setCenterRight(QPointF &pos);

protected:
    void updatePath();

    std::shared_ptr<BottleInspector> _inspector;
    double _roiBottom;
    double _roiTop;

    QGraphicsPathItem *_patternRangePath;
    QGraphicsPathItem *_patternRangeBackPath;

    BottleInspectorImageWidgetRadialAreaHandleItem *_handleTopLeft;
    BottleInspectorImageWidgetRadialAreaHandleItem *_handleBottomLeft;
    BottleInspectorImageWidgetRadialAreaHandleItem *_handleCenterRight;
};

#endif // IMAGEWIDGETRADIALAREAITEM_H
