#include "bottleinspectorimagewidgetradialareahandleitem.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>


BottleInspectorImageWidgetRadialAreaHandleItem::BottleInspectorImageWidgetRadialAreaHandleItem(RadialAreaHandleType handleType, BottleInspectorImageWidgetRadialAreaItem *parent) :
    QGraphicsRectItem(-15, -15, 31, 31, parent)
{
    _handleType = handleType;

    setBrush(QBrush(Qt::lightGray));
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    updatePos();
}

void BottleInspectorImageWidgetRadialAreaHandleItem::updatePos()
{
    BottleInspectorImageWidgetRadialAreaItem *parent = qgraphicsitem_cast<BottleInspectorImageWidgetRadialAreaItem *>(parentItem());
    if (parent) {
        setFlag(ItemSendsGeometryChanges, false);

        switch(_handleType) {
        case ResizeHandleTop:
            setPos(parent->topLeft());
            break;

        case ResizeHandleBottom:
            setPos(parent->bottomLeft());
            break;

        case MoveHandle:
            setPos(parent->centerRight());
            break;
        }

        setFlag(ItemSendsGeometryChanges, true);
    }
}

QVariant BottleInspectorImageWidgetRadialAreaHandleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    BottleInspectorImageWidgetRadialAreaItem *parent = qgraphicsitem_cast<BottleInspectorImageWidgetRadialAreaItem *>(parentItem());

    if (parent) {
        if (change == ItemPositionChange && scene()) {
            // value is the new position.
            QPointF newPos = value.toPointF();
            QRectF allowedRect = parent->inspector()->boundingRect();

            // y
            switch(_handleType) {
            case ResizeHandleBottom:
            case ResizeHandleTop:
                if (newPos.y() < allowedRect.top()) {
                    newPos.setY(allowedRect.top());
                } else if (newPos.y() > allowedRect.bottom()) {
                    newPos.setY(allowedRect.bottom());
                }

//                // make sure there is no flip occuring
//                if (_handleType == ResizeHandleTop && newPos.y() > parent->sceneBoundingRect().bottom()) {
//                    newPos.setY(parent->sceneBoundingRect().bottom());
//                } else if (_handleType == ResizeHandleBottom && newPos.y() < parent->sceneBoundingRect().top()) {
//                    newPos.setY(parent->sceneBoundingRect().top());
//                }

                break;

            case MoveHandle:
                if (newPos.y() < allowedRect.top()) {
                    newPos.setY(allowedRect.top());
                } else if (newPos.y() > allowedRect.bottom()) {
                    newPos.setY(allowedRect.bottom());
                }

                break;
            }

            // x
            switch(_handleType) {
            case ResizeHandleBottom:
            case ResizeHandleTop:
            {
                Eigen::Vector3d pw = parent->inspector()->pixelToWorld(newPos);
                double radius = parent->inspector()->diameterAt(pw(1)) / 2.0;
                Eigen::Vector2d p = parent->inspector()->worldToPixel(radius, pw(1), 0);

                newPos = QPointF(p(0), p(1));
            }
                break;

            case MoveHandle:
            {
                Eigen::Vector3d pw = parent->inspector()->pixelToWorld(newPos);
                double radius = parent->inspector()->diameterAt(pw(1)) / 2.0;
                Eigen::Vector2d p = parent->inspector()->worldToPixel(-radius, pw(1), 0);

                newPos = QPointF(p(0), p(1));
            }
                break;
            }

            return newPos;

        } else if (change == ItemPositionHasChanged) {
            QPointF pos = value.toPointF();

            switch(_handleType) {
            case ResizeHandleTop:
                parent->setTopLeft(pos);
                break;

            case ResizeHandleBottom:
                parent->setBottomLeft(pos);
                break;

            case MoveHandle:
                parent->setCenterRight(pos);
                break;
            }
        }
    }

    return QGraphicsRectItem::itemChange(change, value);
}
