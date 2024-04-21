#include "bottlethreadinspector.h"

#include <QElapsedTimer>

#include <opencv2/features2d.hpp>

#ifdef Q_OS_WIN
#pragma warning(push)
#pragma warning(disable: 4100)
#endif
#ifndef Q_MOC_RUN
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#endif
#ifdef Q_OS_WIN
#pragma warning(pop)
#endif
using namespace boost::accumulators;

#include "common.h"
#include "eigen_support.h"
#include "opencv_support.h"


BottleThreadInspector::BottleThreadInspector(const cv::Mat &image, QObject *parent) :
    QObject(parent)
{
    _image = image;
}

void BottleThreadInspector::findThread()
{
    ENSURE_ONCE();
    START_MEASURE_PERFORMANCE();

    // get edges
    cv::Mat sobel_x;
    cv::Mat sobel_y;
    cv::Mat magnitude;
    cv::Sobel(_image, sobel_x, CV_32F, 1, 0, cv::FILTER_SCHARR);
    cv::Sobel(_image, sobel_y, CV_32F, 0, 1, cv::FILTER_SCHARR);
    cv::magnitude(sobel_x, sobel_y, magnitude);

    // find centers in image
    std::vector<double> centers;
    accumulator_set<double, stats<tag::mean, tag::variance > > acc;
    for (int row = 0; row < magnitude.rows; row++) {
        cv::Mat m = magnitude.row(row);
        double l = findEdge(m, cv::EDGE_VERTICAL, 0.25, 0.1, cv::EDGE_BOTH, cv::EDGE_FIRST);
        double r = findEdge(m, cv::EDGE_VERTICAL, 0.25, 0.1, cv::EDGE_BOTH, cv::EDGE_LAST);
        double c = (r - l) / 2.0 + l;

        acc(c);
        centers.push_back(c);
    }

    // remove outliners
    double min_c = mean(acc) - (2 * std::sqrt(variance(acc)));
    double max_c = mean(acc) + (2 * std::sqrt(variance(acc)));
    std::vector<cv::Point2f> center_points;
    for (size_t i=0; i < centers.size(); i++) {
        double v = centers[i];
        if (v >= min_c && v <= max_c) {
            center_points.push_back(cv::Point2f(i, v));
        }
    }

    // fit line over thread center
    cv::Vec4f line_params;
    cv::fitLine(cv::Mat(center_points), line_params, cv::DIST_FAIR, 0, 0.01 , 0.01);
    Eigen::Line center = Eigen::lineFromVec4f(line_params);

    // convert to eigen and get rotation
    Eigen::Line topLine = Eigen::Line(Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(0, 0), Eigen::Vector2d::UnitY()));
    Eigen::Vector2d interTop = center.intersection(topLine);

    Eigen::Vector2d  p1 = center.normal() * 1000 + interTop;
    Eigen::Vector2d  p2 = center.normal().unitOrthogonal() * -1000 + interTop;

    cv::Point2f srcTri[3];
    cv::Point2f dstTri[3];

    srcTri[0] = cv::Point2f(interTop.y(), interTop.x());
    srcTri[1] = cv::Point2f(p1.y(), p1.x());
    srcTri[2] = cv::Point2f(p2.y(), p2.x());

    dstTri[0] = cv::Point2f(100, 0);
    dstTri[1] = cv::Point2f(1000 + 100, 0);
    dstTri[2] = cv::Point2f(100, 1000);

    // get transformation
    _warp = cv::getAffineTransform(srcTri, dstTri);
    _warp_inv = cv::getAffineTransform(dstTri, srcTri);

    // apply transformation
    cv::warpAffine(_image, _corrected, _warp, _image.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);

    STOP_MEASURE_PERFORMANCE();
}

void BottleThreadInspector::findEdges()
{
    ENSURE_ONCE();
    START_MEASURE_PERFORMANCE();

    // get edges
    cv::Mat sobel_x;
    cv::Sobel(_corrected, sobel_x, CV_32F, 1, 0, cv::FILTER_SCHARR);

    cv::Mat left = sobel_x.colRange(0, sobel_x.cols / 2);
    cv::Mat right = sobel_x.colRange(sobel_x.cols / 2, sobel_x.cols);

    std::vector<double> radius_left;
    std::vector<double> radius_right;

    for (int row=0; row < sobel_x.rows; row++) {
        double l = sobel_x.cols / 2 - cv::findEdge(left.row(row), cv::EDGE_VERTICAL, 0.25, 0.1, cv::EDGE_WHITE_TO_BLACK, cv::EDGE_FIRST);
        double r = sobel_x.cols / 2 - (sobel_x.cols / 2 - cv::findEdge(right.row(row), cv::EDGE_VERTICAL, 0.25, 0.1, cv::EDGE_BLACK_TO_WHITE, cv::EDGE_LAST));

        radius_left.push_back(l);
        radius_right.push_back(r);
    }

    _radius_left = radius_left;
    _radius_right = radius_right;

    STOP_MEASURE_PERFORMANCE();
}

void BottleThreadInspector::findTop()
{
    ENSURE_ONCE();
    START_MEASURE_PERFORMANCE();

    accumulator_set<double, stats<tag::median > > acc_width;

    // collect all widths
    for (size_t i=0; i<_radius_left.size(); i++) {
        acc_width(_radius_left[i] + _radius_right[i]);
    }

    // get edges
    cv::Mat sobel_y;
    cv::Sobel(_corrected, sobel_y, CV_32F, 0, 1, cv::FILTER_SCHARR);

    // use 20% of width to find top
    double top_width = median(acc_width) * 0.20;

    accumulator_set<double, stats<tag::mean > > acc_top;

    int top_start_col = std::round((sobel_y.cols / 2) - (top_width / 2));
    int top_stop_col = std::round((sobel_y.cols / 2) + (top_width / 2));

    for (int col = top_start_col; col <= top_stop_col; col++) {
        double t = cv::findEdge(sobel_y.col(col), cv::EDGE_HORIZONTAL, 0.25, 0.1, cv::EDGE_WHITE_TO_BLACK, cv::EDGE_FIRST);
        acc_top(t);
    }

    _top = mean(acc_top);

    STOP_MEASURE_PERFORMANCE();
}

void BottleThreadInspector::findInnerRadius()
{
    ENSURE_ONCE();
    START_MEASURE_PERFORMANCE();

    // get add radius and sort
    std::vector<double> all_radius;
    for (size_t i = std::ceil(_top); i < _radius_left.size(); i++) {
        all_radius.push_back(_radius_left[i]);
        all_radius.push_back(_radius_right[i]);
    }
    std::sort(all_radius.begin(), all_radius.end());

    // clean sorted
    std::vector<double> cleaned;
    double last = all_radius.at(all_radius.size() - 1);
    for (size_t i=0; i < all_radius.size(); i++) {
        double v = all_radius[i];

        // ignore first if not at least 4 items within 0.5px
        if (all_radius.size() - i > 4 && all_radius[i + 3] > all_radius[i] + 0.5)
            continue;

        // ignore last 3px (outer edge of thread, etc)
        if (v >= last - 3.0)
            break;

        cleaned.push_back(v);
    }

    // now try to find first biggest area within 1.0 px?
    uint prev_cnt = 0;
    double start = 0;
    for (size_t i = 0; i < cleaned.size(); i++) {
        // get size of this block
        uint this_cnt = 0;
        for (size_t n=i; n < cleaned.size(); n++) {
            if (cleaned[n] <= cleaned[i] + 1.0) {
                this_cnt++;
            } else {
                break;
            }
        }

        // this block bigger?
        if (this_cnt > prev_cnt) {
            prev_cnt = this_cnt;
            start = cleaned[i];
        } else {
            break;
        }

        // skip items that are equal
        while (cleaned[i] == start && i < cleaned.size()) {
            i++;
        }
        i--;
    }

    // find average
    accumulator_set<double, stats<tag::mean > > acc;
    for (size_t i=0; i < cleaned.size(); i++) {
        double v = cleaned[i];

        if (v < start) {
            continue;
        } else if (v <= start + 1.0) {
            acc(v);
        } else {
            break;
        }
    }

    qDebug() << "average inner width" << mean(acc);
    _inner_radius = mean(acc);

    STOP_MEASURE_PERFORMANCE();
}

void BottleThreadInspector::findThreadType()
{
    ENSURE_ONCE();

    findThread();
    findEdges();
    findTop();
    findInnerRadius();

    START_MEASURE_PERFORMANCE();

//    accumulator_set<double, stats<tag::mean, tag::median, tag::variance, tag::skewness, tag::kurtosis, tag::moment<1>, tag::moment<2>, tag::moment<3>, tag::moment<4>, tag::moment<5>, tag::moment<6> > > acc_diff;
//    accumulator_set<double, stats<tag::mean, tag::median, tag::variance, tag::skewness, tag::kurtosis, tag::moment<1>, tag::moment<2>, tag::moment<3>, tag::moment<4>, tag::moment<5>, tag::moment<6> > > acc_dev;
    accumulator_set<double, stats<tag::moment<4> > > acc_diff;
    accumulator_set<double, stats<tag::variance > > acc_dev;

    // now check for thread, cork or crown cap
    double prev = 0.0;
    bool pSet = false;
    for (size_t i=0; i < _radius_left.size(); i++) {
        double l = _radius_left[i];
        double r = _radius_right[i];

        if (l >= _inner_radius || r >= _inner_radius) {
            if (std::abs(l - r) > 0.5) {
                acc_diff(l - r);
            }

            if (pSet) {
                acc_dev(l + r - prev);
            }

            prev = l + r;
            pSet = true;
        }
    }

//    qDebug() << "mean     median   variance stdv     skewness  kurosis   m1       m2       m3";
//    qDebug() << mean(acc_diff) << median(acc_diff) << variance(acc_diff) << sqrt(variance(acc_diff)) << skewness(acc_diff) << kurtosis(acc_diff) << moment<1>(acc_diff) << moment<2>(acc_diff) << moment<3>(acc_diff) << moment<4>(acc_diff) << moment<5>(acc_diff) << moment<6>(acc_diff);
//    qDebug() << mean(acc2) << median(acc2) << variance(acc2) << sqrt(variance(acc2)) << skewness(acc2) << kurtosis(acc2) << moment<1>(acc2) << moment<2>(acc2) << moment<3>(acc2) << moment<4>(acc2) << moment<5>(acc2) << moment<6>(acc2);
//    qDebug() << mean(acc_dev) << median(acc_dev) << variance(acc_dev) << sqrt(variance(acc_dev)) << skewness(acc_dev) << kurtosis(acc_dev) << moment<1>(acc_dev) << moment<2>(acc_dev) << moment<3>(acc_dev) << moment<4>(acc_dev) << moment<5>(acc_dev) << moment<6>(acc_dev);

//    qDebug() << "m4" << moment<4>(acc_diff);

    if (moment<4>(acc_diff) > 20.0) {
        qDebug() << "thread";
        _thread_type = ThreadBottle;
    } else if (variance(acc_dev) > 0.2) {
        qDebug() << "crown";
        _thread_type = CrownBottle;
    } else {
        qDebug() << "cork";
        _thread_type = CorkBottle;
    }

    STOP_MEASURE_PERFORMANCE();
}

void BottleThreadInspector::findThreads()
{
    ENSURE_ONCE();

    findThreadType();

    START_MEASURE_PERFORMANCE();

    if (_thread_type != ThreadBottle) {
        qDebug() << "Bottle has no thread!";
        return;
    }

    int scaleFactor = 25;
    int smoothSize = 51;

    // set everything above top to distance of 0
    std::vector<double> cleaned_left(_radius_left.size());
    std::vector<double> cleaned_right(_radius_right.size());
    for (size_t i=0; i < _radius_left.size(); i++) {
        if (i <= std::ceil(_top)) {
            cleaned_left[i] = 0;
            cleaned_right[i] = 0;
        } else {
            cleaned_left[i] = _radius_left[i];
            cleaned_right[i] = _radius_right[i];
        }
    }

    cv::Mat l = cv::Mat(cleaned_left);
    cv::Mat r = cv::Mat(cleaned_right);

    // scale
    cv::Mat lr;
    cv::Mat rr;
    cv::resize(l, lr, cv::Size(1, l.rows * scaleFactor), 0, 0, cv::INTER_LINEAR);
    cv::resize(r, rr, cv::Size(1, r.rows * scaleFactor), 0, 0, cv::INTER_LINEAR);

    // blur
    cv::Mat lb;
    cv::Mat rb;
    cv::GaussianBlur(lr, lb, cv::Size(1, smoothSize), 0);
    cv::GaussianBlur(rr, rb, cv::Size(1, smoothSize), 0);


    // get location where to start looking for threads
    int start_pos = std::ceil(_top);
    for (int i=std::ceil(_top * scaleFactor); i < lr.rows; i++) {
        double l = lb.at<double>(i);
        double r = rb.at<double>(i);

        if (l >= _inner_radius && r >= _inner_radius) {
            start_pos = i;
            break;
        }
    }


    double prev = 0.0;
    bool highFound = false;

    for (int i = start_pos; i < lr.rows; i++) {
        double n = lb.at<double>(i, 0);
        if (n >= prev) {
            highFound = true;
        } else if (highFound) {
            if ((lb.at<double>(i - 1) - rb.at<double>(i - 1)) > 2.0) {
                double pos = (i - 1) / (double)scaleFactor - _top;
                double magnitude = lb.at<double>(i - 1) - _inner_radius;
                qDebug() << "## thread left at" << pos << magnitude;

                double px = (_corrected.cols / 2.0) - lb.at<double>(i - 1);
                double py = (i - 1) / (double)scaleFactor;

                double m11 = _warp_inv.at<double>(0, 0);
                double m12 = _warp_inv.at<double>(0, 1);
                double m13 = _warp_inv.at<double>(0, 2);
                double m21 = _warp_inv.at<double>(1, 0);
                double m22 = _warp_inv.at<double>(1, 1);
                double m23 = _warp_inv.at<double>(1, 2);

                double pmx = (m11 * px) + (m12 * py) + m13;
                double pmy = (m21 * px) + (m22 * py) + m23;

                BottleThread thread = {Left, pos, magnitude, cv::Point2f(pmx, pmy), cv::Point2d(px, py)};
                _threads << thread;
            }

            highFound = false;
        }

        prev = n;
    }

    prev = 0.0;
    highFound = false;

    for (int i = start_pos; i < rr.rows; i++) {
        double n = rb.at<double>(i, 0);
        if (n >= prev) {
            highFound = true;
        } else if (highFound) {
            if ((lb.at<double>(i - 1) - rb.at<double>(i - 1)) < -2.0) {
                double pos = (i - 1) / (double)scaleFactor - _top;
                double magnitude = rb.at<double>(i - 1) - _inner_radius;

                double px = (_corrected.cols / 2.0) + rb.at<double>(i - 1);
                double py = (i - 1) / (double)scaleFactor;

                double m11 = _warp_inv.at<double>(0, 0);
                double m12 = _warp_inv.at<double>(0, 1);
                double m13 = _warp_inv.at<double>(0, 2);
                double m21 = _warp_inv.at<double>(1, 0);
                double m22 = _warp_inv.at<double>(1, 1);
                double m23 = _warp_inv.at<double>(1, 2);

                double pmx = (m11 * px) + (m12 * py) + m13;
                double pmy = (m21 * px) + (m22 * py) + m23;

                BottleThread thread = {Left, pos, magnitude, cv::Point2f(pmx, pmy), cv::Point2d(px, py)};
                _threads << thread;
            }

            highFound = false;
        }

        prev = n;
    }

    STOP_MEASURE_PERFORMANCE();
}
