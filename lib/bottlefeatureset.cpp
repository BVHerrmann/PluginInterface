#include "bottlefeatureset.h"

#include "common.h"


BottleFeatureSet::BottleFeatureSet(std::vector<double> heights, std::vector<Eigen::Vector3d> profile, std::vector<Eigen::Vector3d> keypoints)
{
    _heights = heights;
    _profile = profile;
    _keypoints = keypoints;
}

BottleFeatureSet::~BottleFeatureSet()
{

}

bool BottleFeatureSet::isValid() const
{
    return _heights.size() && _profile.size();
}

BottleSampleDistance BottleFeatureSet::calculateDistanceTo(const BottleFeatureSet *other, icl::interval_set<double> profileRanges, icl::interval_set<double> keypointRanges, double matchDist) const
{
    // filter profile
    std::vector<Eigen::Vector3d> p_filtered;
    std::vector<Eigen::Vector3d> po_filtered;
    if (profileRanges.empty()) {
        p_filtered = _profile;
        po_filtered = other->profile();
    } else {
        const std::vector<Eigen::Vector3d> p = other->profile();

        for (auto i = _profile.begin(); i != _profile.end(); ++i) {
            if (icl::contains(profileRanges, (*i)(1)))
                p_filtered.push_back(*i);
        }
        for (auto i = p.begin(); i != p.end(); ++i) {
            if (icl::contains(profileRanges, (*i)(1)))
                po_filtered.push_back(*i);
        }
    }

    double profileDistance = calculateSquaredEuclidianDistance(p_filtered, po_filtered);

    // filter both sets of keypoints
    std::vector<Eigen::Vector3d> k_filtered;
    std::vector<Eigen::Vector3d> ko_filtered;
    if (!keypointRanges.empty()) {
        const std::vector<Eigen::Vector3d> k = other->keypoints();

        for (auto i = _keypoints.begin(); i != _keypoints.end(); ++i) {
            if (icl::contains(keypointRanges, (*i)(1)))
                k_filtered.push_back(*i);
        }
        for (auto i = k.begin(); i != k.end(); ++i) {
            if (icl::contains(keypointRanges, (*i)(1)))
                ko_filtered.push_back(*i);
        }
    }

    // match keypoints new -> old
    uint no_inliners = matchKeypoints(k_filtered, ko_filtered, matchDist);

    // match keypoints old -> new
    uint on_inliners = matchKeypoints(ko_filtered, k_filtered, matchDist);
    double keypointMatchRatio = (k_filtered.size() || ko_filtered.size()) ? (double) (no_inliners + on_inliners) / (k_filtered.size() + ko_filtered.size()) : 1.0;

    // make sure almost empty areas count as empty!
    double length = icl::length(keypointRanges);
    if (k_filtered.size() < length && ko_filtered.size() < length) {
        keypointMatchRatio = 1.0;
    } else if ((k_filtered.size() + ko_filtered.size()) < 2 * length) {
        keypointMatchRatio = std::max(0.9, keypointMatchRatio);
    } else if ((k_filtered.size() + ko_filtered.size()) < 4 * length) {
        keypointMatchRatio = std::max(0.7, keypointMatchRatio);
    }

#ifdef MATCH_DETAILS
    qDebug() << "profileDistance:" << profileDistance;
    qDebug() << "keypointMatchRatio" << keypointMatchRatio;
    qDebug() << "new->old:" << (double) no_inliners / keypoints().size() << no_inliners << keypoints().size();
    qDebug() << "old->new:" << (double) on_inliners / other->keypoints().size() << on_inliners << other->keypoints().size();
    qDebug() << "combined:" << (double) (no_inliners + on_inliners) / (keypoints().size() + other->keypoints().size());
    qDebug() << "new->old:" << (double) no_inliners / k_filtered.size() << no_inliners << k_filtered.size();
    qDebug() << "old->new:" << (double) on_inliners / ko_filtered.size() << on_inliners << ko_filtered.size();
    qDebug() << "combined:" << (double) (no_inliners + on_inliners) / (k_filtered.size() + ko_filtered.size());
#endif

    BottleSampleDistance result;
//    result.reference_angle = 0.0;
    result.combined = 0.0;
    result.profile = profileDistance;
    result.keypoints = keypointMatchRatio;

    return result;
}

BottleSampleDistance BottleFeatureSet::calculateDistanceTo(const BottleFeatureSet *other, icl::interval_set<double> profileRanges, std::vector<cv::Rect> keypointAreas, double matchDist) const
{
    // filter profile
    std::vector<Eigen::Vector3d> p_filtered;
    std::vector<Eigen::Vector3d> po_filtered;
    if (profileRanges.empty()) {
        p_filtered = _profile;
        po_filtered = other->profile();
    } else {
        const std::vector<Eigen::Vector3d> p = other->profile();

        for (auto i = _profile.begin(); i != _profile.end(); ++i) {
            if (icl::contains(profileRanges, (*i)(1)))
                p_filtered.push_back(*i);
        }
        for (auto i = p.begin(); i != p.end(); ++i) {
            if (icl::contains(profileRanges, (*i)(1)))
                po_filtered.push_back(*i);
        }
    }

    double profileDistance = calculateSquaredEuclidianDistance(p_filtered, po_filtered);

    // filter both sets of keypoints
    std::vector<Eigen::Vector3d> k_filtered;
    std::vector<Eigen::Vector3d> ko_filtered;
    if (!keypointAreas.empty()) {
        const std::vector<Eigen::Vector3d> k = other->keypoints();

        for (auto i = _keypoints.begin(); i != _keypoints.end(); ++i) {
            cv::Point p((*i)(0), (*i)(1));
            for (auto r = keypointAreas.begin(); r != keypointAreas.end(); ++r) {
                if (r->contains(p)) {
                    k_filtered.push_back(*i);
                    break;
                }
            }
        }
        for (auto i = k.begin(); i != k.end(); ++i) {
            cv::Point p((*i)(0), (*i)(1));
            for (auto r = keypointAreas.begin(); r != keypointAreas.end(); ++r) {
                if (r->contains(p)) {
                    ko_filtered.push_back(*i);
                    break;
                }
            }
        }
    }

    // match keypoints new -> old
    uint no_inliners = matchKeypoints(k_filtered, ko_filtered, matchDist);

    // match keypoints old -> new
    uint on_inliners = matchKeypoints(ko_filtered, k_filtered, matchDist);
    double keypointMatchRatio = (k_filtered.size() || ko_filtered.size()) ? (double) (no_inliners + on_inliners) / (k_filtered.size() + ko_filtered.size()) : 1.0;

    // make sure almost empty areas count as empty!
    double length = 0;
    for (auto r = keypointAreas.begin(); r != keypointAreas.end(); ++r) {
        length += (r->size().height / 15.0);
    }

    if (k_filtered.size() < length && ko_filtered.size() < length) {
        keypointMatchRatio = 1.0;
    } else if ((k_filtered.size() + ko_filtered.size()) < 2 * length) {
        keypointMatchRatio = std::max(0.9, keypointMatchRatio);
    } else if ((k_filtered.size() + ko_filtered.size()) < 4 * length) {
        keypointMatchRatio = std::max(0.7, keypointMatchRatio);
    }

#ifdef MATCH_DETAILS
    qDebug() << "profileDistance:" << profileDistance;
    qDebug() << "keypointMatchRatio" << keypointMatchRatio;
//    qDebug() << "new->old:" << (double) no_inliners / keypoints().size() << no_inliners << keypoints().size();
//    qDebug() << "old->new:" << (double) on_inliners / other->keypoints().size() << on_inliners << other->keypoints().size();
//    qDebug() << "combined:" << (double) (no_inliners + on_inliners) / (keypoints().size() + other->keypoints().size());
    qDebug() << "new->old:" << (double) no_inliners / k_filtered.size() << no_inliners << k_filtered.size();
    qDebug() << "old->new:" << (double) on_inliners / ko_filtered.size() << on_inliners << ko_filtered.size();
    qDebug() << "combined:" << (double) (no_inliners + on_inliners) / (k_filtered.size() + ko_filtered.size());
#endif

    BottleSampleDistance result;
//    result.reference_angle = 0.0;
    result.combined = 0.0;
    result.profile = profileDistance;
    result.keypoints = keypointMatchRatio;

    return result;
}

double BottleFeatureSet::calculateSquaredEuclidianDistance(const std::vector<Eigen::Vector3d> &list_one, const std::vector<Eigen::Vector3d> &list_two) const
{
    size_t n = list_one.size() <= list_two.size() ? list_one.size() : list_two.size();

    // map lists to eigen matrix
    Eigen::Map<Eigen::Matrix3Xd> l1((double *)list_one.data(), 3, n);
    Eigen::Map<Eigen::Matrix3Xd> l2((double *)list_two.data(), 3, n);

    return (l1 - l2).squaredNorm();
}

uint BottleFeatureSet::matchKeypoints(std::vector<Eigen::Vector3d> query, std::vector<Eigen::Vector3d> base, double matchDist) const
{
    // check preconditions
    if (base.empty() || query.empty()) {
        return 0;
    }

    uint inliners = 0;

    std::vector<Eigen::Vector3d>::iterator lower = base.begin();

    for (size_t i=0; i < query.size(); i++) {
        Eigen::Vector3d q = query[i];

        // elements are sorted, so we can only move up and keep the lower bound
        Eigen::Vector3d ql = Eigen::Vector3d(q(0), q(1) - matchDist, q(2));
        lower = std::lower_bound(lower, base.end(), ql, Eigen::Vector3dSortByY);

        // now search for actual close keypoint (below threshold)
        std::vector<Eigen::Vector3d>::iterator it = lower;
        while (it != base.end()) {
            Eigen::Vector3d p = *it;

            // too high. no match. stop.
            if (p(1) > q(1) + matchDist) {
                break;
            }

            // too far left. continue.
            if (p(0) < q(0) - matchDist) {
                ++it;
                continue;
            }

            // too far right. continue.
            if (p(0) > q(0) + matchDist) {
                ++it;
                continue;
            }

            double d = std::sqrt(std::pow(q(0) - p(0), 2) + std::pow(q(1) - p(1), 2)); // z-axis is always 0, so it can be omitted right now for performance reasons

            if (d <= matchDist) {
                inliners++;
                break;
            }

            ++it;
        }
    }

    return inliners;
}

QDataStream &operator<<(QDataStream &stream, const std::shared_ptr<const BottleFeatureSet> featureSet)
{
    std::vector<double> heights = featureSet->heights();
    std::vector<Eigen::Vector3d> profile = featureSet->profile();
    std::vector<Eigen::Vector3d> keypoints = featureSet->keypoints();

    stream << QVector<double>(heights.begin(), heights.end());
    stream << QVector<Eigen::Vector3d>(profile.begin(), profile.end());
    stream << QVector<Eigen::Vector3d>(keypoints.begin(), keypoints.end());
    
    return stream;
}

QDataStream &operator>>(QDataStream &stream, std::shared_ptr<const BottleFeatureSet> &featureSet)
{
    QVector<double> heights;
    QVector<Eigen::Vector3d> profile;
    QVector<Eigen::Vector3d> keypoints;

    stream >> heights;
    stream >> profile;
    stream >> keypoints;
    
    featureSet = std::shared_ptr<const BottleFeatureSet>(new BottleFeatureSet(std::vector<double>(heights.begin(), heights.end()), std::vector<Eigen::Vector3d>(profile.begin(), profile.end()), std::vector<Eigen::Vector3d>(keypoints.begin(), keypoints.end())));

    return stream;
}
