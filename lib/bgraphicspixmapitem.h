#ifndef BGRAPHICSPIXMAPITEM_H
#define BGRAPHICSPIXMAPITEM_H

#include "bgraphicsitem.h"

#include <QPixmap>


class BGraphicsPixmapItem : public BGraphicsItem
{
public:
    explicit BGraphicsPixmapItem();
    
    explicit BGraphicsPixmapItem(const QPixmap &pixmap) : BGraphicsItem() {
        setPixmap(pixmap);
    }

    explicit BGraphicsPixmapItem(const QImage &image) : BGraphicsItem() {
        setImage(image);
    }

    QPixmap pixmap() const { return _pixmap; }
    void setPixmap(const QPixmap &pixmap) { _pixmap = pixmap; }
    void setImage(const QImage &image) { _pixmap = QPixmap::fromImage(image); }

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QPixmap _pixmap;
};

#endif // BGRAPHICSPIXMAPITEM_H
