#ifndef IMAGEWIDGETAREAHANDLEITEM_H
#define IMAGEWIDGETAREAHANDLEITEM_H

#include <QGraphicsRectItem>

#include "imagewidgetareaitem.h"


typedef enum {
    ResizeHandleTopLeft,
    ResizeHandleTopRight,
    ResizeHandleBottomLeft,
    ResizeHandleBottomRight
} AreaHandleType;


class ImageWidgetAreaHandleItem : public QGraphicsRectItem
{
public:
    explicit ImageWidgetAreaHandleItem(AreaHandleType handleType, ImageWidgetAreaItem *parent = 0);

    void updatePos();

    AreaHandleType handleType() const { return _handleType; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    AreaHandleType _handleType;
};

#endif // IMAGEWIDGETAREAHANDLEITEM_H
