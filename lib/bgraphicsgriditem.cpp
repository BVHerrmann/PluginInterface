#include "bgraphicsgriditem.h"

#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>


BGraphicsGridItem::BGraphicsGridItem() :
    BGraphicsItem()
{
}

QGraphicsItem * BGraphicsGridItem::toGraphicsItem() const
{
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    group->setToolTip(toolTip());

    if (_grid.size() && _grid[0].size()) {
        int shadow_rows = _grid.size() - 1;
        int shadow_cols = _grid[0].size() - 1;

        // shadows
        for (int r = 0; r < shadow_rows; r++) {
            QList<QPointF> grid_row_top = _grid[r];
            QList<QPointF> grid_row_bottom = _grid[r + 1];

            for (int c = 0; c < shadow_cols; c++) {
                double value = _values[r * shadow_cols + c];

                QPolygonF polygon;
                polygon << grid_row_top[c] << grid_row_top[c + 1] << grid_row_bottom[c + 1] << grid_row_bottom[c] << grid_row_top[c];

                QGraphicsPolygonItem *pg = new QGraphicsPolygonItem(polygon);
                pg->setToolTip(QString::number(value));
                pg->setPen(QPen(Qt::NoPen));
                pg->setBrush(QBrush(QColor(pen().color().red(), pen().color().green(), pen().color().blue(), 255 * value / 2.0)));
                group->addToGroup(pg);
            }
        }

        // outline
        QPainterPath path;
        int outline_rows = _grid.size();
        int outline_cols = _grid[0].size();

        // horizontal lines
        for (int r=0; r < outline_rows; r++) {
            QList<QPointF> row = _grid[r];
            if (row.size() < outline_cols)
                outline_cols = row.size();

            for (int c=0; c < outline_cols; c++) {
                c ? path.lineTo(row[c]) : path.moveTo(row[c]);
            }
        }

        // vertical lines
        for (int c=0; c < outline_cols; c++) {
            for (int r=0; r < outline_rows; r++) {
                r ? path.lineTo(_grid[r][c]) : path.moveTo(_grid[r][c]);
            }
        }

        QGraphicsPathItem *pathItem  = new QGraphicsPathItem(path);
        pathItem->setPen(pen());
        group->addToGroup(pathItem);
    }

    return group;
}
