#include "bottleinspectionsample.h"

#include <QtCore>


BottleInspectionSample::BottleInspectionSample(std::shared_ptr<const BottleFeatureSet> left, std::shared_ptr<const BottleFeatureSet> center, std::shared_ptr<const BottleFeatureSet> right)
{
    _left = left;
    _center = center;
    _right = right;
}

BottleInspectionSample::BottleInspectionSample(std::shared_ptr<const BottleProcessingResult> left, std::shared_ptr<const BottleProcessingResult> center, std::shared_ptr<const BottleProcessingResult> right)
{
    if (left)
        _left = left->featureSet();
    if (center)
        _center = center->featureSet();
    if (right)
        _right = right->featureSet();
}

BottleInspectionSample::~BottleInspectionSample()
{

}

BottleSampleDistance BottleInspectionSample::calculateDistanceTo(std::shared_ptr<const BottleInspectionSample> other, boost::icl::interval_set<double> profileRanges, boost::icl::interval_set<double> keypointRanges, double matchDist, double priorityFactor) const
{
    BottleSampleDistance distance = {other, 0.0, 0.0, 0.0, 0.0};

    uint cnt = 0;

    if (_left && other->left()) {
        BottleSampleDistance other_distance = _left->calculateDistanceTo(other->left().get(), profileRanges, keypointRanges, matchDist);
        distance.height += other_distance.height;
        distance.profile += other_distance.profile;
        distance.keypoints += other_distance.keypoints;
        cnt++;
    }
    if (_center && other->center()) {
        BottleSampleDistance other_distance = _center->calculateDistanceTo(other->center().get(), profileRanges, keypointRanges, matchDist);
        distance.height += other_distance.height;
        distance.profile += other_distance.profile;
        distance.keypoints += other_distance.keypoints;
        cnt++;
    }
    if (_right && other->right()) {
        BottleSampleDistance other_distance = _right->calculateDistanceTo(other->right().get(), profileRanges, keypointRanges, matchDist);
        distance.height += other_distance.height;
        distance.profile += other_distance.profile;
        distance.keypoints += other_distance.keypoints;
        cnt++;
    }

    if (cnt) {
        // normalize keypoint value
        distance.keypoints /= (double)cnt;

        // calculate combined value
        double scaledProfileDistance = distance.profile;
        double scaledKeypointDistance = (1.0 - distance.keypoints) * 100.0;
        if (priorityFactor < 0) {
            scaledProfileDistance *= -priorityFactor;
        } else if (priorityFactor > 0) {
            scaledKeypointDistance *= priorityFactor;
        }
        distance.combined = std::sqrt(std::pow(scaledProfileDistance, 2) + std::pow(scaledKeypointDistance, 2));
    }

    return distance;
}

BottleSampleDistance BottleInspectionSample::calculateDistanceTo(std::shared_ptr<const BottleInspectionSample> other, boost::icl::interval_set<double> profileRanges, std::multimap<CameraPosition, cv::Rect> keypointAreas, double matchDist, std::function<void(BottleSampleDistance&)> combineFunction) const
{
    BottleSampleDistance distance = {other, 0.0, 0.0, 0.0, 0.0};

    uint cnt = 0;

    if (_left && other->left()) {
        std::vector<cv::Rect> keypointAreasLeft;
        auto range = keypointAreas.equal_range(CameraLeft);
        for (auto it = range.first; it != range.second; ++it) {
            keypointAreasLeft.push_back(it->second);
        }

        BottleSampleDistance other_distance = _left->calculateDistanceTo(other->left().get(), profileRanges, keypointAreasLeft, matchDist);
        distance.height += other_distance.height;
        distance.profile += other_distance.profile;
        distance.keypoints += other_distance.keypoints;
        cnt++;
    }
    if (_center && other->center()) {
        std::vector<cv::Rect> keypointAreasCenter;
        auto range = keypointAreas.equal_range(CameraCenter);
        for (auto it = range.first; it != range.second; ++it) {
            keypointAreasCenter.push_back(it->second);
        }

        BottleSampleDistance other_distance = _center->calculateDistanceTo(other->center().get(), profileRanges, keypointAreasCenter, matchDist);
        distance.height += other_distance.height;
        distance.profile += other_distance.profile;
        distance.keypoints += other_distance.keypoints;
        cnt++;
    }
    if (_right && other->right()) {
        std::vector<cv::Rect> keypointAreasRight;
        auto range = keypointAreas.equal_range(CameraRight);
        for (auto it = range.first; it != range.second; ++it) {
            keypointAreasRight.push_back(it->second);
        }

        BottleSampleDistance other_distance = _right->calculateDistanceTo(other->right().get(), profileRanges, keypointAreasRight, matchDist);
        distance.height += other_distance.height;
        distance.profile += other_distance.profile;
        distance.keypoints += other_distance.keypoints;
        cnt++;
    }

    if (cnt) {
        // normalize values
        distance.profile /= cnt;
        distance.keypoints /= (double)cnt;

        if (combineFunction) {
            // apply function to get combined values
            combineFunction(distance);
        } else {
            // use default
            distance.combined = std::sqrt(std::pow(distance.profile, 2) + std::pow((1.0 - distance.keypoints) * 100.0, 2));
        }
    }

    return distance;
}

QDataStream &operator<<(QDataStream &stream, const std::shared_ptr<const BottleInspectionSample> sample)
{
    if (sample->left()) {
        stream << true;
        stream << sample->left();
    } else {
        stream << false;
    }

    if (sample->center()) {
        stream << true;
        stream << sample->center();
    } else {
        stream << false;
    }

    if (sample->right()) {
        stream << true;
        stream << sample->right();
    } else {
        stream << false;
    }

    return stream;
}

QDataStream &operator>>(QDataStream &stream, std::shared_ptr<const BottleInspectionSample> &sample)
{
    bool featureSetExists = false;
    std::shared_ptr<const BottleFeatureSet> left;
    std::shared_ptr<const BottleFeatureSet> center;
    std::shared_ptr<const BottleFeatureSet> right;

    stream >> featureSetExists;
    if (featureSetExists)
        stream >> left;
    stream >> featureSetExists;
    if (featureSetExists)
        stream >> center;
    stream >> featureSetExists;
    if (featureSetExists)
        stream >> right;

    sample = std::make_shared<BottleInspectionSample>(left, center, right);

    return stream;
}
