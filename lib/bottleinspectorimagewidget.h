#ifndef BOTTLEINSPECTORIMAGEWIDGET_H
#define BOTTLEINSPECTORIMAGEWIDGET_H

#include "imagewidget.h"

#ifndef Q_OS_WIN
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#endif
#ifndef Q_MOC_RUN
#include <boost/icl/interval_set.hpp>
#endif
#ifndef Q_OS_WIN
#pragma clang diagnostic pop
#endif

#include "bottleprocessingresult.h"


class BottleInspectorImageWidget : public ImageWidget
{
    Q_OBJECT
public:
    explicit BottleInspectorImageWidget(QWidget *parent = 0);

    std::shared_ptr<const BottleProcessingResult> result() const { return std::dynamic_pointer_cast<const BottleProcessingResult>(_result); }

    boost::icl::interval_set<double> profileRanges() const { return _profile_ranges; }
    boost::icl::interval_set<double> keypointRanges() const { return _keypoint_ranges; }
    void setProfileRanges(boost::icl::interval_set<double> ranges) { _profile_ranges = ranges; }
    void setKeypointRanges(boost::icl::interval_set<double> ranges) { _keypoint_ranges = ranges; }

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void parameterUpdated();

public slots:
    void displayProcessingResult(std::shared_ptr<const ProcessingResult> result);

protected:
    boost::icl::interval_set<double> _profile_ranges;
    boost::icl::interval_set<double> _keypoint_ranges;
    QList<QPolygonF> _keypoint_areas;

    QPoint _dragStart;
    ImageAction _action;
};

#endif // BOTTLEINSPECTORIMAGEWIDGET_H
