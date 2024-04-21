#include "bottleinspector.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#ifdef Q_OS_WIN
#pragma warning(push)
#pragma warning(disable: 4100)
#endif
#ifndef Q_MOC_RUN
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#endif
#ifdef Q_OS_WIN
#pragma warning(pop)
#endif
using namespace boost::accumulators;

#include "common.h"
#include "opencv_support.h"
#include "types.h"


BottleInspector::BottleInspector(const cv::Mat &image) :
    BottleImageInspector(image)
{
    _threadInspector = nullptr;

    // initialize helper
    _bottle_height = -1;

    // initialize camera
    _calculate_camera_transform_valid = false;
    _optimize_camera_transform_run = false;
    _camera_position = Eigen::Vector3d(0.0, 0.0, 1265.0);
    _camera_rotation = Eigen::Vector3d(radians(8.0), 0.0, 0.0);
}

BottleInspector::~BottleInspector()
{
    if (_threadInspector)
        delete _threadInspector;
}

void BottleInspector::calculateCameraTransfrom()
{
    // dependent on base and center
    //    if (!bottleROIFound() || !bottleCenterFound())
    //        return;

    const Eigen::Line center = bottleCenter();

    // rotate around z-axis to fit lean
    double a = std::acos(center.normal().unitOrthogonal().dot(Eigen::Vector2d::UnitY()));
    if (a > M_PI_2)
        a -= M_PI;
    _camera_rotation(2) = a;

    // rotate around y-axis to fit center
    Eigen::Line v_middle = Eigen::Line(Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(_image.cols / 2, _image.rows / 2), Eigen::Vector2d::UnitX()));
    Eigen::Vector2d inter = center.intersection(v_middle);
    double r = std::atan2(_image.cols / 2 - inter(0), g());
    _camera_rotation(1) = r;

    // rotate around x-axis to fit base
    double base_dist = base().absDistance(Eigen::Vector2d(_image.cols / 2, _image.rows / 2));
    _camera_rotation(0) = std::atan2(base_dist, g());
}

void BottleInspector::optimizeCameraTransform(const uint iterations, const double epsilon)
{
    if (_optimize_camera_transform_run) {
        return;
    }

    // dependent on base and center
    if (!bottleROIFound() || !bottleCenterFound())
        return;

    // rough camera transform should be available
    ensureCameraTransfrom();

    const Eigen::Line center = bottleCenter();

    // maybe do this?
    //    _camera_rotation(0) = 8.0;

    START_MEASURE_PERFORMANCE();

    for (uint i=1; i <= iterations; i++) {
        bool transform_converged = true;

        // change height to fit base
        double base_dist = base().signedDistance(worldToPixel(Eigen::Vector3d(0, 0, 0)));
        if (std::fabs(base_dist) > epsilon) {
            Eigen::Vector2d prj = base().projection(worldToPixel(Eigen::Vector3d(0, 0, 0)));
            Eigen::Vector3d v = pixelToWorld(QPointF(prj(0), prj(1)));
            _camera_position[1] += v(1);
            transform_converged = false;
        }

        // rotate around z-axis to fit lean
        Eigen::Vector2d origin = worldToPixel(Eigen::Vector3d(0, 0, 0));
        Eigen::Vector2d top = worldToPixel(Eigen::Vector3d(0, 100, 0));
        double dot = center.normal().unitOrthogonal().dot((top - origin).normalized());
        if ((float) dot <= (float) 1.0 && (float) dot >= (float)  -1.0) {
            double a = std::acos(dot);
            if(a > epsilon && a < M_PI - epsilon) {
                Eigen::Vector2d off = center.projection(top) - top;

                if (a <= M_PI_2 && a >= -M_PI_2) {
                    off(0) < 0.0 ? _camera_rotation(2) -= a : _camera_rotation(2) += a;
                } else {
                    a > M_PI_2 ? a -= M_PI : a += M_PI;
                    off(0) < 0.0 ? _camera_rotation(2) += a : _camera_rotation(2) -= a;
                }

                transform_converged = false;
            }
        }

        // rotate around y-axis to fit center
        Eigen::Line h_center = Eigen::Line(Eigen::ParametrizedLine<double, 2>::Through(worldToPixel(Eigen::Vector3d(0, 0, 0)), worldToPixel(Eigen::Vector3d(0, 100, 0))));
        Eigen::Vector2d center = h_center.intersection(base());
        Eigen::Vector2d inter = bottleCenter().intersection(base());

        double d = std::sqrt((center - inter).array().pow(2).sum());
        if (d > epsilon) {
            double r = std::atan2(d, g());

            if ((center - inter)(0) < 0.0) {
                _camera_rotation(1) -= r;
            } else {
                _camera_rotation(1) += r;
            }

            transform_converged = false;
        }

        // early exit condition
        if (transform_converged) {
            break;
        }
    }

    _optimize_camera_transform_run = true;

    STOP_MEASURE_PERFORMANCE();
}

void BottleInspector::findBottleHeight()
{
    std::vector<double> heights = bottleHeights();
    std::sort(heights.begin(), heights.end());

    if (heights.size())
        _bottle_height = heights[heights.size() / 2];
}

void BottleInspector::findBottleHeights(const double radius)
{
    std::vector<double> heights;

    if (!bottleROIFound()) {
        qWarning() << "Bottle ROI not found, can not execute findBottleTop()!";
        return;
    }
    if (!bottleCenterFound()) {
        qWarning() << "Bottle Center not found, can not execute findBottleTop()!";
        return;
    }

    START_MEASURE_PERFORMANCE();

    Eigen::Line border_left = lineAtX(-radius);
    Eigen::Line border_right = lineAtX(radius);

    QRect bounding_rect = this->boundingRect().toRect();

    QLineF intersection_left = Eigen::intersectLineWithRect(border_left, bounding_rect);
    QLineF intersection_right = Eigen::intersectLineWithRect(border_right, bounding_rect);

    int start_col = std::floor(std::min(intersection_right.p1().x(), intersection_right.p2().x()));
    int stop_col = std::ceil(std::max(intersection_left.p1().x(), intersection_left.p2().x()));

    QList<accumulator_set<double, stats<tag::mean > > > acc;

    for (int i=start_col; i < stop_col; i++) {
        QPointF p = findBottleTopInColumn(i);
        Eigen::Vector3d w = pixelToWorld(p);

        // create ParametrizedLine
        Eigen::ParametrizedLine<double, 3> c = Eigen::ParametrizedLine<double, 3>(Eigen::Vector3d(0, 0, 0), Eigen::Vector3d::UnitY());

        int distance = std::round(c.distance(w));
        if (distance <= radius) {
            while (acc.size() <= distance)
                acc << accumulator_set<double, stats<tag::mean > >();

            acc[distance](w(1));
        }
    }

    heights.resize(acc.size());

    for (int i=0; i<acc.size(); i++) {
        heights[i] = mean(acc[i]);
    }

    _bottle_heights = heights;

    STOP_MEASURE_PERFORMANCE();
}
/*
void BottleInspector::findThreadROI()
{
    ENSURE_ONCE();

    findBottleCenter();
    findBottleHeight();
    ensureCameraTransfrom();

    double height = bottleHeight();
    Eigen::Vector2d centerTop = worldToPixel(0, height + 2.5, 0);

    Eigen::Vector2d centerBottom = worldToPixel(0, height - 22.5, 0);

    _threadRoi = QRect(centerTop(0) - 100, centerTop(1), 201, centerBottom(1) - centerTop(1));
    _threadRoiImage = _image.colRange(_threadRoi.left(), _threadRoi.right()).rowRange(_threadRoi.top(), _threadRoi.bottom());
}
*/
/*
void BottleInspector::findThread()
{
    if (!_threadInspector) {
        _threadInspector = new BottleThreadInspector(threadROIImage());
    }
}
*/
/*
QList<BottleThread> BottleInspector::threads()
{
    findThread();

    QList<BottleThread> result;

    QList<BottleThread> threads = _threadInspector->threads();

    for (const BottleThread &thread : threads) {
        thread.position.x = thread.position.x + _threadRoi.left();
        thread.position.y = thread.position.y + _threadRoi.top();
        result << thread;
    }

    return result;
}
*/
// TODO: improve to take parts of pixel into account. Will also make it work for precision < 1.0
const std::tuple<std::vector<Eigen::Vector3d>, std::vector<Eigen::Vector3d>> BottleInspector::extractProfile(double start, double height, double precision, std::vector<Eigen::Vector2d> *left_profile, std::vector<Eigen::Vector2d> *right_profile)
{
    // dependent and center
    if (!bottleCenterFound()) {
        qWarning() << "Bottle Center not found, can not execute extractProfile()!";
        return std::make_tuple(std::vector<Eigen::Vector3d>(), std::vector<Eigen::Vector3d>());
    }
    
    std::vector<Eigen::Vector3d> samples_left;
    std::vector<Eigen::Vector3d> samples_right;

    QRectF bounding_rect = boundingRect();
    Eigen::Line left_border = Eigen::Line(Eigen::ParametrizedLine<double, 2>::Through(Eigen::Vector2d(bounding_rect.topLeft().x(), bounding_rect.topLeft().y()), Eigen::Vector2d(bounding_rect.bottomLeft().x(), bounding_rect.bottomLeft().y())));
    Eigen::Line right_border = Eigen::Line(Eigen::ParametrizedLine<double, 2>::Through(Eigen::Vector2d(bounding_rect.topRight().x(), bounding_rect.topRight().y()), Eigen::Vector2d(bounding_rect.bottomRight().x(), bounding_rect.bottomRight().y())));

    Eigen::Line center = bottleCenter();

    Eigen::Line lu;
    Eigen::Line lo = this->lineAtY(start);
    for (double h = start; h < start + height; h += precision) {
        // update lines
        lu = lo;
        lo = this->lineAtY(h + precision);
        Eigen::Line lc = this->lineAtY(h + (precision * 0.5));
        
        // get area
        double top_left = std::clamp(std::min(center.intersection(lo)(1), left_border.intersection(lo)(1)), 0.0, _image.rows - 1.0);
        double bottom_left = std::clamp(std::max(center.intersection(lu)(1), left_border.intersection(lu)(1)), 0.0, _image.rows - 1.0);
        double top_right = std::clamp(std::min(center.intersection(lo)(1), right_border.intersection(lo)(1)), 0.0, _image.rows - 1.0);
        double bottom_right = std::clamp(std::max(center.intersection(lu)(1), right_border.intersection(lu)(1)), 0.0, _image.rows - 1.0);

        double left_x = 0;
        for (int i = std::floor(bottom_left) - 1; i >= std::ceil(top_left); i--) {
            left_x += smoothedLeftProfileInRow(i).x();
        }
        Eigen::Vector2d pl = intersectLineWithX(lc, left_x / (std::floor(bottom_left) - std::ceil(top_left)));
        Eigen::Vector3d plw = pixelToWorld(pl);
        samples_left.push_back(plw);

        double right_x = 0;
        for (int i = std::floor(bottom_right) - 1; i >= std::ceil(top_right); i--) {
            right_x += smoothedRightProfileInRow(i).x();
        }
        Eigen::Vector2d pr = intersectLineWithX(lc, right_x / (std::floor(bottom_right) - std::ceil(top_right)));
        Eigen::Vector3d prw = pixelToWorld(pr);
        samples_right.push_back(prw);

        // update paths
        if (left_profile)
            left_profile->push_back(pl);
        if (right_profile)
            right_profile->push_back(pr);
    }
    
    return std::make_tuple(samples_left, samples_right);
}

double BottleInspector::diameterAt(const double position)
{
    double diameter = 0;

    const auto& [profile_left, profile_right] = extractProfile(position - 0.5, 1.0, 1.0);

    if (profile_left.size() == 1 && profile_right.size() == 1) {
        diameter = (profile_left[0] - profile_right[0]).norm();
    }

    return diameter;
}

inline Eigen::Line BottleInspector::lineAtX(const double x)
{
    Eigen::Vector2d p1 = worldToPixel(x, -100, 0);
    Eigen::Vector2d p2 = worldToPixel(x, 100, 0);

    // create ParametrizedLine
    Eigen::ParametrizedLine<double, 2> pline = Eigen::ParametrizedLine<double, 2>::Through(p1, p2);

    // convert to Line
    return Eigen::Line(pline);
}

inline Eigen::Line BottleInspector::lineAtY(const double y)
{
    Eigen::Vector2d p1 = worldToPixel(-100, y, 0);
    Eigen::Vector2d p2 = worldToPixel(100, y, 0);

    // create ParametrizedLine
    Eigen::ParametrizedLine<double, 2> pline = Eigen::ParametrizedLine<double, 2>::Through(p1, p2);

    // convert to Line
    return Eigen::Line(pline);
}

const std::vector<Eigen::Vector3d> BottleInspector::keypointsWorld()
{
    if (!_keypointsWorld.empty()) {
        return _keypointsWorld;
    }

    std::vector<cv::KeyPoint> kp = keypoints();

    START_MEASURE_PERFORMANCE();

    std::vector<Eigen::Vector3d> keypoints;

    for (auto keypoint = kp.begin(); keypoint != kp.end(); ++keypoint) {
        if (keypoint->pt.x != 0 && keypoint->pt.y != 0) {
            keypoints.push_back(pixelToWorld(keypoint->pt));
        }
    }

    // sort keypoints (so binary search, lower and upper bound can be used later). this is necessary as correct sorting is not guaranteed after coordinate transformation
    std::sort(keypoints.begin(), keypoints.end(), Eigen::Vector3dSortByY);

    _keypointsWorld = keypoints;

    STOP_MEASURE_PERFORMANCE();

    return _keypointsWorld;
}

const std::vector<Eigen::Vector3d> BottleInspector::keypointsFASTWorld()
{
    if (!_keypointsWorld.empty()) {
        return _keypointsWorld;
    }

    findBottleHeight();

    cv::Mat m = bottleMask();

    int height = std::round(bottleHeight());

    START_MEASURE_PERFORMANCE();

//    contrast();

    // extract keypoints
    std::vector<cv::KeyPoint> fast;
    cv::FAST(_image, fast, 30.0, false);    // uses FAST 9_16, which should be the best
//    cv::FAST(_contrast, fast, 30.0, false);
//    cv::FAST(_image, fast, 7, false);    // uses FAST 9_16, which should be the best
//    cv::FAST(eql, fast, 10.0, false);    // uses FAST 9_16, which should be the best
//    cv::FAST(_contrast, fast, 10.0, true);

    INTERMEDIATE_MEASURE_PERFORMANCE();

    // get valid keypoints
    std::vector<cv::Point2f> validKeypoints;
    std::vector<uint8_t> sortedRatings;
    for (size_t i=0; i < fast.size(); i++) {
        cv::KeyPoint p = fast[i];

        uint8_t rating = m.at<uint8_t>(p.pt.y, p.pt.x);
        if (rating > 0) {
            validKeypoints.push_back(p.pt);
            sortedRatings.push_back(rating);
        }
    }
    std::sort(sortedRatings.begin(), sortedRatings.end());

    // TODO: handle if too few points found
	uint8_t cuttoff = 0;
    if ((int) sortedRatings.size() > height) {
        cuttoff = sortedRatings[sortedRatings.size() - height];
	}

    std::vector<Eigen::Vector3d> keypoints;
    for (size_t i=0; i < validKeypoints.size(); i++) {
        cv::Point2f p = validKeypoints[i];
        if (m.at<uint8_t>(p.y, p.x) >= cuttoff) {
            Eigen::Vector3d pw = pixelToWorld(p);
            keypoints.push_back(pw);
        }
    }

    INTERMEDIATE_MEASURE_PERFORMANCE();

    // sort keypoints (so binary search, lower and upper bound can be used later)
    std::sort(keypoints.begin(), keypoints.end(), Eigen::Vector3dSortByY);

    double thres = 5.0;
    uint min_found = 5;

    for (size_t i=0; i < keypoints.size(); i++) {
        Eigen::Vector3d p = keypoints[i];

        Eigen::Vector3d pl = Eigen::Vector3d(p(0), p(1) - thres, p(2));
        std::vector<Eigen::Vector3d>::iterator lower = std::lower_bound(keypoints.begin(), keypoints.end(), pl, Eigen::Vector3dSortByY);

        uint found = 0;

        // now search for actual close keypoint (below threshold)
        std::vector<Eigen::Vector3d>::iterator it = lower;
        while (it != keypoints.end()) {
            Eigen::Vector3d q = *it;

            if (q(1) > p(1) + thres)
                break;

            if (q != p) {
                double d = std::sqrt(std::pow(q(0) - p(0), 2) + std::pow(q(1) - p(1), 2) + std::pow(q(2) - p(2), 2));
                if (d <= thres) {
                    found++;
                    if (found == min_found)
                        break;
                }
            }
            ++it;
        }

        if (found == min_found) {
            // found a match, so use it
            _keypointsWorld.push_back(p);
        }
    }

    INTERMEDIATE_MEASURE_PERFORMANCE();

    STOP_MEASURE_PERFORMANCE();

    return _keypointsWorld;
}
