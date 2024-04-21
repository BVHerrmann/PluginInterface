#ifndef BOTTLEIMAGEINSPECTORIMAGEWIDGET_H
#define BOTTLEIMAGEINSPECTORIMAGEWIDGET_H

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

#include "bottleimageprocessingresult.h"


class BottleImageInspectorImageWidget : public ImageWidget
{
    Q_OBJECT
public:
    explicit BottleImageInspectorImageWidget(QWidget *parent = 0);

    double minSideSize() const { return _minSideSize; }
    void setMinSideSize(double size) { _minSideSize = size; }

    std::shared_ptr<const BottleImageProcessingResult> result() const { return std::dynamic_pointer_cast<const BottleImageProcessingResult>(_result); }

    boost::icl::interval_set<double> profileRanges() const { return _profile_ranges; }
    std::vector<cv::Rect> keypointAreas() const { return _keypoint_areas; }
    void setProfileRanges(boost::icl::interval_set<double> ranges) { _profile_ranges = ranges; }
    void setKeypointAreas(std::vector<cv::Rect> areas) { _keypoint_areas = areas; }

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void parameterUpdated();

public slots:
    void displayProcessingResult(std::shared_ptr<const ProcessingResult> result);

protected:
    double _minSideSize;

    boost::icl::interval_set<double> _profile_ranges;
    std::vector<cv::Rect> _keypoint_areas;

    QPoint _dragStart;
    ImageAction _action;
};

#endif // BOTTLEIMAGEINSPECTORIMAGEWIDGET_H
