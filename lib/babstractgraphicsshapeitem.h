#ifndef BABSTRACTGRAPHICSSHAPEITEM_H
#define BABSTRACTGRAPHICSSHAPEITEM_H

#include "bgraphicsitem.h"

#include <QBrush>


class BAbstractGraphicsShapeItem : public BGraphicsItem
{
public:
    explicit BAbstractGraphicsShapeItem();
    
    QBrush brush() const { return _brush; }
    void setBrush(const QBrush &brush) { _brush = brush; }

signals:
    
public slots:
    
protected:
    QBrush _brush;
};

#endif // BABSTRACTGRAPHICSSHAPEITEM_H
