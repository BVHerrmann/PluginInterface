#ifndef BOTTLEIMAGEINSPECTOR_H
#define BOTTLEIMAGEINSPECTOR_H

#include "imageinspector.h"

#include <QIcon>
#include <QPolygonF>
#include <qnumeric.h>

#include <opencv2/features2d.hpp>

#include "eigen_support.h"

#ifndef radians
    #define radians(x) (M_PI * (x) / 180.0)
#endif
#ifndef degrees
    #define degrees(x) (180.0 * (x) / M_PI)
#endif


class BottleImageInspector : public ImageInspector
{

public:
    explicit BottleImageInspector(const cv::Mat &image, const std::vector<cv::Point> roi = std::vector<cv::Point>());
    virtual ~BottleImageInspector();

    // base
    virtual inline bool baseFound() { return base().offset() != 0; }
    virtual const Eigen::Line base();
    inline const QLineF baseLine() { return baseFound() ? Eigen::intersectLineWithRect(base(), QRect(0, 0, _image.cols, _image.rows)) : QLineF(); }
    inline double basePosition() { return baseFound() ? (baseLine().y1() + baseLine().y2()) / 2 : 0; }

    // bottle roi
    virtual inline bool bottleROIFound() { return !bottleHull().empty(); }
    virtual const std::vector<cv::Point> bottleHull(const uint border = 10);
    inline const QRectF boundingRect() { if(bottleROIFound()) {cv::Rect r = cv::boundingRect(bottleHull()); return QRectF(r.x, r.y, r.width, r.height); } else { return QRectF();} }

    // icon
    const QIcon icon();

    // bottle profile
    const QPolygonF leftProfile() { return _smoothed_left_profile; }
    const QPolygonF rightProfile() { return _smoothed_right_profile; }

    // bottle center
    inline bool bottleCenterFound() { return !qIsNaN(bottleCenter().offset()); }
    virtual const Eigen::Line bottleCenter(const double orientationThreshold = 45.0, const double orientationDiffThreshold = 18.0, const int stretchThreshold = 5);
    inline const QLineF centerLine() { return Eigen::intersectLineWithRect(bottleCenter(), boundingRect().toRect()); }
    const QPolygonF centerProfile() { bottleCenter(); return _center_profile; }

    // bottle top
    const QPolygonF topProfile();

    // masks
    const cv::Mat bottleMask();
    const cv::Mat dialatedBottleMask();
    const cv::Mat equalizedBottleMask();
    const cv::Mat contrastBottleMask();
    const cv::Mat keypointMask(int edge = 0);

    // keypoints
    const cv::Mat keypointEdges(int sensitivity = 0);
    const std::vector<cv::KeyPoint> keypoints(int sensitivity = 0);

protected:
    // bottle profile
    void findBottleProfileInRow(const uint row, const double threshold, const double precision, QPointF *pointLeft = nullptr, QPointF *pointRight = nullptr);
    const QPointF leftProfileInRow(const uint row, const double threshold = 0.2, const double precision = 1.0);
    const QPointF rightProfileInRow(const uint row, const double threshold = 0.2, const double precision = 1.0);
    void findSmoothedBottleProfileInRow(const uint row, const int k_size, const double threshold, const double precision, QPointF *pointLeft = nullptr, QPointF *pointRight = nullptr);
    const QPointF smoothedLeftProfileInRow(const uint row, const int k_size = 7, const double threshold = 0.2, const double precision = 1.0);
    const QPointF smoothedRightProfileInRow(const uint row, const int k_size = 7, const double threshold = 0.2, const double precision = 1.0);

    // bottle top
    const QPointF findBottleTopInColumn(const uint col, const double threshold = 0.2, const double precision = 1.0);

private:
    // base
    bool _base_executed;
    Eigen::Line _base;

    // bottle roi
    bool _roi_executed;
    std::vector<cv::Point> _hull;

    // icon
    QIcon _icon;

    // bottle profile
    inline const cv::Mat profileSmoothKernel(const int k_size);
    QPolygonF _left_profile, _right_profile;
    cv::Mat _profileSmoothKernel;
    QPolygonF _smoothed_left_profile, _smoothed_right_profile;

    // bottle center
    bool _center_executed;
    Eigen::Line _center;
    QPolygonF _center_profile;

    // bottle top
    QPolygonF _top_profile;

    // bottle masks
    cv::Mat _mask;
    cv::Mat _dialatedMask;
    cv::Mat _equalizedMask;
    cv::Mat _contrastMask;
    cv::Mat _keypointMask;

    // keypoints
    cv::Mat _keypointEdges;
    std::vector<cv::KeyPoint> _keypoints;
};

#endif // BOTTLEIMAGEINSPECTOR_H
