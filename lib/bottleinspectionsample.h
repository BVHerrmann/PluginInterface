#ifndef BOTTLEINSPECTIONSAMPLE_H
#define BOTTLEINSPECTIONSAMPLE_H

#include <QDataStream>

#include "bottlefeatureset.h"
#include "bottleprocessingresult.h"
#include "bottlesampledistance.h"


typedef enum {
    CameraUnknown   = 0,
    CameraLeft      = 1 << 0,
    CameraCenter    = 1 << 1,
    CameraRight     = 1 << 2,
    CameraTop       = 1 << 3,
    CameraThread    = 1 << 4
} CameraPosition;


class BottleInspectionSample
{
public:
    explicit BottleInspectionSample(std::shared_ptr<const BottleFeatureSet> left, std::shared_ptr<const BottleFeatureSet> center, std::shared_ptr<const BottleFeatureSet> right);
    explicit BottleInspectionSample(std::shared_ptr<const BottleProcessingResult> left, std::shared_ptr<const BottleProcessingResult> center, std::shared_ptr<const BottleProcessingResult> right);
    virtual ~BottleInspectionSample();

    BottleSampleDistance calculateDistanceTo(std::shared_ptr<const BottleInspectionSample> other, boost::icl::interval_set<double> profileRanges, boost::icl::interval_set<double> keypointRanges, double matchDist, double priorityFactor = 1000.0) const;
    BottleSampleDistance calculateDistanceTo(std::shared_ptr<const BottleInspectionSample> other, boost::icl::interval_set<double> profileRanges, std::multimap<CameraPosition, cv::Rect> keypointAreas, double matchDist, std::function<void(BottleSampleDistance&)> combineFunction = std::function<void(BottleSampleDistance&)>()) const;

    std::shared_ptr<const BottleFeatureSet> left() const { return _left; }
    void setLeft(std::shared_ptr<const BottleFeatureSet> featureSet) { _left = featureSet; }

    std::shared_ptr<const BottleFeatureSet> center() const { return _center; }
    void setCenter(std::shared_ptr<const BottleFeatureSet> featureSet) { _center = featureSet; }

    std::shared_ptr<const BottleFeatureSet> right() const { return _right; }
    void setRight(std::shared_ptr<const BottleFeatureSet> featureSet) { _right = featureSet; }

protected:
    std::shared_ptr<const BottleFeatureSet> _left;
    std::shared_ptr<const BottleFeatureSet> _center;
    std::shared_ptr<const BottleFeatureSet> _right;
};

QDataStream &operator<<(QDataStream &stream, const std::shared_ptr<const BottleInspectionSample> sample);
QDataStream &operator>>(QDataStream &stream, std::shared_ptr<const BottleInspectionSample> &sample);

#endif // BOTTLEINSPECTIONSAMPLE_H
