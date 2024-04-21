#ifndef BGRAPHICSLINEITEM_H
#define BGRAPHICSLINEITEM_H

#include "bgraphicsitem.h"

#include <QLineF>


class BGraphicsLineItem : public BGraphicsItem
{
public:
    explicit BGraphicsLineItem();

    explicit BGraphicsLineItem(const QLineF &line) : BGraphicsItem() {
        setLine(line);
    }

    QLineF line() const { return _line; }
    void setLine(const QLineF &line) { _line = line; }

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QLineF _line;
};

#endif // BGRAPHICSLINEITEM_H
