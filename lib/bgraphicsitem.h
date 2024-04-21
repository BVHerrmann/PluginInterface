#ifndef BGRAPHICSITEM_H
#define BGRAPHICSITEM_H

#include <QObject>
#include <QPen>
class QGraphicsItem;


class BGraphicsItem
{
public:
    explicit BGraphicsItem();
    virtual ~BGraphicsItem();
    
    QPen pen() const { return _pen; }
    void setPen(const QPen &pen) { _pen = pen; }

    QString toolTip() const { return _toolTip; }
    void setToolTip(const QString &toolTip) { _toolTip = toolTip; }

    virtual QGraphicsItem *toGraphicsItem() const = 0;

protected:
    QPen _pen;
    QString _toolTip;
};

#endif // BGRAPHICSITEM_H
