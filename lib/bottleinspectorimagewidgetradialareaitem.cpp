#include "bottleinspectorimagewidgetradialareaitem.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "bottleinspectorimagewidgetradialareahandleitem.h"
#include "colors.h"


BottleInspectorImageWidgetRadialAreaItem::BottleInspectorImageWidgetRadialAreaItem(std::shared_ptr<BottleInspector> inspector, double roiBottom, double roiTop, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    _inspector = inspector;
    _roiBottom = roiBottom;
    _roiTop = roiTop;

    // create paths
    _patternRangePath = new QGraphicsPathItem(this);
    QPen pen(HMIColor::yellow);
    pen.setWidth(0);
    _patternRangePath->setPen(pen);
    _patternRangeBackPath = new QGraphicsPathItem(this);
    QPen penBack(HMIColor::transparentYellow);
    penBack.setWidth(0);
    _patternRangeBackPath->setPen(penBack);

    // create handles
    _handleTopLeft = new BottleInspectorImageWidgetRadialAreaHandleItem(ResizeHandleTop, this);
    _handleBottomLeft = new BottleInspectorImageWidgetRadialAreaHandleItem(ResizeHandleBottom, this);
    _handleCenterRight = new BottleInspectorImageWidgetRadialAreaHandleItem(MoveHandle, this);

    // draw
    updatePath();
}

QRectF BottleInspectorImageWidgetRadialAreaItem::boundingRect() const
{
    return QRectF();
}

void BottleInspectorImageWidgetRadialAreaItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)painter;
    (void)option;
    (void)widget;
}

void BottleInspectorImageWidgetRadialAreaItem::updatePath()
{
    bool move = true;

    double radiusBottom = inspector()->diameterAt(roiBottom()) / 2.0;
    double radiusTop = inspector()->diameterAt(roiTop()) / 2.0;

    // pattern range back
    QPainterPath patternRangeBackPath;
    move = true;
    for (int a = 180; a <= 360; ++a) {
        double x = radiusBottom * std::cos(radians(a));
        double y = radiusBottom * -std::sin(radians(a));
        Eigen::Vector2d w = inspector()->worldToPixel(x, roiBottom(), y);
        if (move) {
            patternRangeBackPath.moveTo(w(0), w(1));
            move = false;
        } else {
            patternRangeBackPath.lineTo(w(0), w(1));
        }
    }
    move = true;
    for (int a = 180; a <= 360; ++a) {
        double x = radiusTop * std::cos(radians(a));
        double y = radiusTop * -std::sin(radians(a));
        Eigen::Vector2d w = inspector()->worldToPixel(x, roiTop(), y);
        if (move) {
            patternRangeBackPath.moveTo(w(0), w(1));
            move = false;
        } else {
            patternRangeBackPath.lineTo(w(0), w(1));
        }
    }
    _patternRangeBackPath->setPath(patternRangeBackPath);

    // pattern range
    QPainterPath patternRangePath;
    move = true;
    for (int a = 0; a <= 180; ++a) {
        double x = radiusBottom * std::cos(radians(a));
        double y = radiusBottom * -std::sin(radians(a));
        Eigen::Vector2d w = inspector()->worldToPixel(x, roiBottom(), y);
        if (move) {
            patternRangePath.moveTo(w(0), w(1));
            move = false;
        } else {
            patternRangePath.lineTo(w(0), w(1));
        }
    }
    move = true;
    for (int a = 0; a <= 180; ++a) {
        double x = radiusTop * std::cos(radians(a));
        double y = radiusTop * -std::sin(radians(a));
        Eigen::Vector2d w = inspector()->worldToPixel(x, roiTop(), y);
        if (move) {
            patternRangePath.moveTo(w(0), w(1));
            move = false;
        } else {
            patternRangePath.lineTo(w(0), w(1));
        }
    }
    _patternRangePath->setPath(patternRangePath);

    // update handles
    _handleTopLeft->updatePos();
    _handleBottomLeft->updatePos();
    _handleCenterRight->updatePos();
}

QPointF BottleInspectorImageWidgetRadialAreaItem::topLeft()
{
    double radiusTop = inspector()->diameterAt(roiTop()) / 2.0;
    Eigen::Vector2d p = inspector()->worldToPixel(radiusTop, roiTop(), 0);

    return QPointF(p(0), p(1));
}

void BottleInspectorImageWidgetRadialAreaItem::setTopLeft(QPointF &pos)
{
    Eigen::Vector3d p = inspector()->pixelToWorld(pos);

    setROITop(p(1));
    updatePath();
}

QPointF BottleInspectorImageWidgetRadialAreaItem::bottomLeft()
{
    double radiusBottom = inspector()->diameterAt(roiBottom()) / 2.0;
    Eigen::Vector2d p = inspector()->worldToPixel(radiusBottom, roiBottom(), 0);

    return QPointF(p(0), p(1));
}

void BottleInspectorImageWidgetRadialAreaItem::setBottomLeft(QPointF &pos)
{
    Eigen::Vector3d p = inspector()->pixelToWorld(pos);

    setROIBottom(p(1));
    updatePath();
}

QPointF BottleInspectorImageWidgetRadialAreaItem::centerRight()
{
    double radiusCenter = inspector()->diameterAt(roiCenter()) / 2.0;
    Eigen::Vector2d p = inspector()->worldToPixel(-radiusCenter, roiCenter(), 0);

    return QPointF(p(0), p(1));
}

void BottleInspectorImageWidgetRadialAreaItem::setCenterRight(QPointF &pos)
{
    Eigen::Vector3d p = inspector()->pixelToWorld(pos);
    double offset = p(1) - roiCenter();
    double bottom = roiBottom() + offset;
    double top = roiTop() + offset;

    setROIBottom(bottom);
    setROITop(top);
    updatePath();
}
