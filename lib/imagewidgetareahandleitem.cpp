#include "imagewidgetareahandleitem.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>


ImageWidgetAreaHandleItem::ImageWidgetAreaHandleItem(AreaHandleType handleType, ImageWidgetAreaItem *parent) :
    QGraphicsRectItem(-15, -15, 31, 31, parent)
{
    _handleType = handleType;

    setBrush(QBrush(Qt::lightGray));
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    updatePos();
}

void ImageWidgetAreaHandleItem::updatePos()
{
    ImageWidgetAreaItem *parent = qgraphicsitem_cast<ImageWidgetAreaItem *>(parentItem());

    if (parent) {
        setFlag(ItemSendsGeometryChanges, false);

        switch(_handleType) {
        case ResizeHandleTopLeft:
            setPos(parent->boundingRect().topLeft());
            break;

        case ResizeHandleTopRight:
            setPos(parent->boundingRect().topRight());
            break;

        case ResizeHandleBottomLeft:
            setPos(parent->boundingRect().bottomLeft());
            break;

        case ResizeHandleBottomRight:
            setPos(parent->boundingRect().bottomRight());
            break;
        }

        setFlag(ItemSendsGeometryChanges, true);
    }
}

QVariant ImageWidgetAreaHandleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    ImageWidgetAreaItem *parent = qgraphicsitem_cast<ImageWidgetAreaItem *>(parentItem());

    if (parent) {
        if (change == ItemPositionChange && scene()) {
            // value is the new position.
            QPointF newPos = value.toPointF();
            QRectF allowedRect = parent->allowedRect();

            // x
            switch(_handleType) {
            case ResizeHandleTopLeft:
            case ResizeHandleBottomLeft:
                if (newPos.x() > parent->sceneBoundingRect().right()) {
                    newPos.setX(parent->sceneBoundingRect().right());
                } else if (newPos.x() < allowedRect.left()) {
                    newPos.setX(allowedRect.left());
                }

                break;

            case ResizeHandleTopRight:
            case ResizeHandleBottomRight:
                if (newPos.x() < parent->sceneBoundingRect().left()) {
                    newPos.setX(parent->sceneBoundingRect().left());
                } else if (newPos.x() > allowedRect.right()) {
                    newPos.setX(allowedRect.right());
                }

                break;

            }

            // y
            switch(_handleType) {
            case ResizeHandleTopLeft:
            case ResizeHandleTopRight:
                if (newPos.y() > parent->sceneBoundingRect().bottom()) {
                    newPos.setY(parent->sceneBoundingRect().bottom());
                } else if (newPos.y() < allowedRect.top()) {
                    newPos.setY(allowedRect.top());
                }

                break;

            case ResizeHandleBottomLeft:
            case ResizeHandleBottomRight:
                if (newPos.y() < parent->sceneBoundingRect().top()) {
                    newPos.setY(parent->sceneBoundingRect().top());
                } else if (newPos.y() > allowedRect.bottom()) {
                    newPos.setY(allowedRect.bottom());
                }

                break;
            }

            return newPos;

        } else if (change == ItemPositionHasChanged) {
            QPointF pos = value.toPointF();
            QRectF rect = parent->rect();

            switch(_handleType) {
            case ResizeHandleTopLeft:
                rect.setTopLeft(pos);
                break;

            case ResizeHandleTopRight:
                rect.setTopRight(pos);
                break;

            case ResizeHandleBottomLeft:
                rect.setBottomLeft(pos);
                break;

            case ResizeHandleBottomRight:
                rect.setBottomRight(pos);
                break;
            }

            parent->setRect(rect);
        }
    }

    return QGraphicsRectItem::itemChange(change, value);
}
