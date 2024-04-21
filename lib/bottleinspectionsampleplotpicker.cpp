#include "bottleinspectionsampleplotpicker.h"

#include <QtCore>


BottleInspectionSamplePlotPicker::BottleInspectionSamplePlotPicker(QWidget *parent) :
    QwtPlotPicker(parent)
{

}

BottleInspectionSamplePlotPicker::BottleInspectionSamplePlotPicker(std::function<void(BottleSampleDistance&)> combineFunction, QWidget *parent) :
    QwtPlotPicker(parent)
{
    _combineFunction = combineFunction;
}

QwtText BottleInspectionSamplePlotPicker::trackerTextF( const QPointF &pos ) const
{
    QString text;

    if (_combineFunction) {
        BottleSampleDistance distance = {std::shared_ptr<const BottleInspectionSample>(), 0.0, 0.0, pos.x(), 1.0 - (pos.y() / 100.0)};
        _combineFunction(distance);
        text = QString("%1, %2 (%3)").arg(pos.x(), 0, 'f', 4).arg(pos.y(), 0, 'f', 4).arg(distance.combined, 0, 'f', 4);
    } else {
        text = QString("%1, %2").arg(pos.x(), 0, 'f', 4).arg(pos.y(), 0, 'f', 4);
    }

    return QwtText(text);
}
