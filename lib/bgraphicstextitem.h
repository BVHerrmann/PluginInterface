#ifndef BGRAPHICSTEXTITEM_H
#define BGRAPHICSTEXTITEM_H

#include "bgraphicsitem.h"

#include <QLineF>


class BGraphicsTextItem : public BGraphicsItem
{
public:
    explicit BGraphicsTextItem();

    explicit BGraphicsTextItem(const QString &text) : BGraphicsItem() {
        setText(text);
    }

    QString text() const { return _text; }
    void setText(const QString &text) { _text = text; }

    QPointF pos() const { return _pos; }
    void setPos(const QPointF &pos) { _pos = pos; }

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QString _text;

    QPointF _pos;
};

#endif // BGRAPHICSTEXTITEM_H
