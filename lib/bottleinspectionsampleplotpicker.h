#ifndef BOTTLEINSPECTIONSAMPLEPLOTPICKER_H
#define BOTTLEINSPECTIONSAMPLEPLOTPICKER_H

#include <functional>

#include <qwt/qwt_plot_picker.h>

#include "bottlesampledistance.h"


class BottleInspectionSamplePlotPicker : public QwtPlotPicker
{
    Q_OBJECT
public:
    explicit BottleInspectionSamplePlotPicker(QWidget *parent);
    explicit BottleInspectionSamplePlotPicker(std::function<void(BottleSampleDistance&)> combineFunction, QWidget *parent);

    void setCombineFunction(std::function<void(BottleSampleDistance&)> combineFunction) { _combineFunction = combineFunction; }

signals:

public slots:

protected:
    virtual QwtText trackerTextF(const QPointF &) const;

private:
    std::function<void(BottleSampleDistance&)> _combineFunction;
};

#endif // BOTTLEINSPECTIONSAMPLEPLOTPICKER_H
