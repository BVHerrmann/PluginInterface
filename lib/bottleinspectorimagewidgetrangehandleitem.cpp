#include "bottleinspectorimagewidgetrangehandleitem.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>


BottleInspectorImageWidgetRangeHandleItem::BottleInspectorImageWidgetRangeHandleItem(RangeHandleType handleType, BottleInspectorImageWidgetRangeItem *parent) :
    QGraphicsRectItem(-15, -15, 31, 31, parent)
{
    _handleType = handleType;

    setBrush(QBrush(Qt::lightGray));
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    updatePos();
}

void BottleInspectorImageWidgetRangeHandleItem::updatePos()
{
    BottleInspectorImageWidgetRangeItem *parent = qgraphicsitem_cast<BottleInspectorImageWidgetRangeItem *>(parentItem());

    if (parent) {
        setFlag(ItemSendsGeometryChanges, false);

        if (parent->rangeType() == RangeKeypoints) {
            switch(_handleType) {
            case ResizeHandleTop:
                setPos(parent->boundingRect().topRight());
                break;
            case ResizeHandleBottom:
                setPos(parent->boundingRect().bottomRight());
                break;
            case MoveHandle:
                setPos(parent->boundingRect().right(), parent->boundingRect().top() + parent->boundingRect().height() / 2 - 1);
                break;
            }
        } else {
            switch(_handleType) {
            case ResizeHandleTop:
                setPos(parent->boundingRect().topLeft());
                break;
            case ResizeHandleBottom:
                setPos(parent->boundingRect().bottomLeft());
                break;
            case MoveHandle:
                setPos(parent->boundingRect().left(), parent->boundingRect().top() + parent->boundingRect().height() / 2 - 1);
                break;
            }
        }

        setFlag(ItemSendsGeometryChanges, true);
    }
}

QVariant BottleInspectorImageWidgetRangeHandleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    BottleInspectorImageWidgetRangeItem *parent = qgraphicsitem_cast<BottleInspectorImageWidgetRangeItem *>(parentItem());

    if (parent) {
        if (change == ItemPositionChange && scene()) {
            // value is the new position.
            QPointF newPos = value.toPointF();

            qreal offset = newPos.y() - this->pos().y();
            QRectF allowedRect = parent->allowedRect();

            // make sure pos is in bounding rect of bottle
            if (_handleType != ResizeHandleBottom && parent->sceneBoundingRect().y() + offset < allowedRect.y()) {
                newPos.setY(this->pos().y() + (allowedRect.y() - parent->sceneBoundingRect().y()));
            } else if (_handleType != ResizeHandleTop && parent->sceneBoundingRect().bottom() + offset > allowedRect.bottom()) {
                newPos.setY(this->pos().y() + (allowedRect.bottom() - parent->sceneBoundingRect().bottom()));
            }

            // make sure there is no flip occuring
            if (_handleType == ResizeHandleTop && newPos.y() > parent->sceneBoundingRect().bottom()) {
                newPos.setY(parent->sceneBoundingRect().bottom());
            } else if (_handleType == ResizeHandleBottom && newPos.y() < parent->sceneBoundingRect().top()) {
                newPos.setY(parent->sceneBoundingRect().top());
            }

            newPos.setX(this->x());

            return newPos;

        } else if (change == ItemPositionHasChanged) {
            QPointF pos = value.toPointF();
            QRectF rect = parent->rect();

            if (parent->rangeType() == RangeKeypoints) {
                switch(_handleType) {
                case ResizeHandleTop:
                    rect.setTopRight(pos);
                    break;
                case ResizeHandleBottom:
                    rect.setBottomRight(pos);
                    break;
                case MoveHandle:
                    rect.moveTop(pos.y() - parent->boundingRect().height() / 2 - 1);
                    break;
                }
            } else {
                switch(_handleType) {
                case ResizeHandleTop:
                    rect.setTopLeft(pos);
                    break;
                case ResizeHandleBottom:
                    rect.setBottomLeft(pos);
                    break;
                case MoveHandle:
                    rect.moveTop(pos.y() - parent->boundingRect().height() / 2 - 1);
                    break;
                }
            }

            parent->setRect(rect);
        }
    }

    return QGraphicsRectItem::itemChange(change, value);
}
