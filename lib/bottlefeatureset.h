#ifndef BOTTLEFEATURESET_H
#define BOTTLEFEATURESET_H

#include <vector>

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

using namespace boost;

#include "eigen_support.h"

#include "bottlesampledistance.h"
#include "featureset.h"


class BottleFeatureSet : public FeatureSet
{
public:
    explicit BottleFeatureSet(std::vector<double> heights, std::vector<Eigen::Vector3d> profile, std::vector<Eigen::Vector3d> keypoints);
    virtual ~BottleFeatureSet();

    std::vector<double> heights() const { return _heights; }
    void setHeights(std::vector<double> heights) { _heights = heights; }

    std::vector<Eigen::Vector3d> profile() const { return _profile; }
    void setProfile(std::vector<Eigen::Vector3d> profile) { _profile = profile; }

    std::vector<Eigen::Vector3d> keypoints() const { return _keypoints; }
    void setKeypoints(std::vector<Eigen::Vector3d> keypoints) { _keypoints = keypoints; }

    BottleSampleDistance calculateDistanceTo(const BottleFeatureSet *other, icl::interval_set<double> profileRanges, icl::interval_set<double> keypointRanges, double matchDist) const;
    BottleSampleDistance calculateDistanceTo(const BottleFeatureSet *other, icl::interval_set<double> profileRanges, std::vector<cv::Rect> keypointAreas, double matchDist) const;

    bool isValid() const override;

protected:
    std::vector<double> _heights;
    std::vector<Eigen::Vector3d> _profile;
    std::vector<Eigen::Vector3d> _keypoints;

    double calculateSquaredEuclidianDistance(const std::vector<Eigen::Vector3d> &list_one, const std::vector<Eigen::Vector3d> &list_two) const;
    uint matchKeypoints(std::vector<Eigen::Vector3d> query, std::vector<Eigen::Vector3d> base, double matchDist) const;
};

QDataStream &operator<<(QDataStream &stream, const std::shared_ptr<const BottleFeatureSet> featureSet);
QDataStream &operator>>(QDataStream &stream, std::shared_ptr<const BottleFeatureSet> &featureSet);

Q_DECLARE_METATYPE(std::shared_ptr<const BottleFeatureSet>)

#endif // BOTTLEFEATURESET_H
