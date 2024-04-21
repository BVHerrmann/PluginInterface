#ifndef BGRAPHICSGRIDITEM_H
#define BGRAPHICSGRIDITEM_H

#include "bgraphicsitem.h"


class BGraphicsGridItem : public BGraphicsItem
{
public:
    explicit BGraphicsGridItem();
    
    explicit BGraphicsGridItem(const QList<QList<QPointF> > &grid, const std::vector<double> &values) : BGraphicsItem() {
        setGrid(grid);
        setValues(values);
    }

    QList<QList<QPointF> > grid() const { return _grid; }
    void setGrid(const QList<QList<QPointF> > &grid) { _grid = grid; }

    std::vector<double> values() const { return _values; }
    void setValues(const std::vector<double> &values) { _values = values; }

    QGraphicsItem *toGraphicsItem() const override;

protected:
    QList<QList<QPointF> > _grid;
    std::vector<double> _values;
};

#endif // BGRAPHICSGRIDITEM_H
