#ifndef BOTTLEINSPECTOR_H
#define BOTTLEINSPECTOR_H

#include <QObject>
#include <QLineF>
#include <QPainterPath>
#include <QPolygonF>
#include <QRectF>

#include "bottleimageinspector.h"
#include "bottlethreadinspector.h"


class BottleInspector : public BottleImageInspector
{

public:
    explicit BottleInspector(const cv::Mat &image);
    virtual ~BottleInspector();

    // optimal base
    inline double optimalBase() { return (_image.rows / 2) + (radians(8.0) / wHeight() * _image.rows / 2); }
    const inline QLineF optimalBaseLine() { return QLineF(0, optimalBase(), _image.cols, optimalBase()); }
    const QRect baseBoundingRect() { double roi_height = g() * std::asin(radians(1.0)); return QRect(0, std::round(optimalBase() - (roi_height / 2.0)), _image.cols, std::round(roi_height)); }

    // top
    const std::vector<double> bottleHeights() { findBottleHeights(); return _bottle_heights; }
    double bottleHeight() { findBottleHeight(); return _bottle_height; }

    // thread
//    inline const QRect threadROI() { findThreadROI(); return _threadRoi; }
//    inline const cv::Mat threadROIImage() { findThreadROI(); return _threadRoiImage; }
//    inline const cv::Mat thread() { findThread(); return _threadInspector->image(); }
//    inline BottleThreadType threadType() { findThread(); return _threadInspector->threadType(); }
//    QList<BottleThread> threads();

    // bottle profile features
    const std::tuple<std::vector<Eigen::Vector3d>, std::vector<Eigen::Vector3d>> extractProfile(double start, double height, double precision = 1.0, std::vector<Eigen::Vector2d> *left_profile = nullptr, std::vector<Eigen::Vector2d> *right_profile = nullptr);
    double diameterAt(const double position);

    // bootle key points
    const std::vector<Eigen::Vector3d> keypointsWorld();
    const std::vector<Eigen::Vector3d> keypointsFASTWorld();

    // camera parameters
    inline double f() { return 16.0; }  // in mm
    inline double sensorWidth() { return 3.6; }
    inline double sensorHeight() { return 4.8; }
    inline QSizeF sensorSize() { return QSizeF(sensorWidth(), sensorHeight()); }    // in mm
    inline double wWidth() { return std::atan2(sensorWidth(), 2 * f()); }
    inline double wHeight() { return std::atan2(sensorHeight(), 2 * f()); }
    inline QSizeF w() { return QSizeF(wWidth(), wHeight()); }   // w in radians
    inline double g() { return ((_image.cols / 2 / tan(wWidth())) + (_image.rows / 2 / tan(wHeight()))) / 2; }   // in px

    // transformation matrices
    inline Eigen::Matrix3d rotationMatrix() { Eigen::Matrix3d r; r = Eigen::AngleAxisd(_camera_rotation[0], Eigen::Vector3d::UnitX()) * Eigen::AngleAxisd(_camera_rotation[1], Eigen::Vector3d::UnitY()) * Eigen::AngleAxisd(_camera_rotation[2], Eigen::Vector3d::UnitZ()); return r; }
    inline Eigen::Transform3d cameraTransform() { ensureCameraTransfrom(); Eigen::Transform3d t = Eigen::Transform3d::Identity(); t.rotate(rotationMatrix()).translate(_camera_position); return t; }

    // world <-> camera
    inline Eigen::Vector3d worldToCamera(const Eigen::Vector3d v) { return cameraTransform() * v; }
    inline Eigen::Vector3d cameraToWorld(const Eigen::Vector3d v) { return cameraTransform().inverse() * v; }

    // camera <-> pixel
    inline Eigen::Vector2d cameraToPixel(const Eigen::Vector3d v) { Eigen::Vector3d p; p = v * (g() / v(2)); return Eigen::Vector2d(_image.cols / 2 - p(0), _image.rows / 2 - p(1)); }
    inline Eigen::Vector3d pixelToCamera(const double x, const double y) {
        // construct world-z-plane in camera coordinates
        Eigen::Plane zplane = Eigen::Plane::Through(Eigen::Vector3d(0, 0, 0), Eigen::Vector3d::UnitX(), Eigen::Vector3d::UnitY());
        zplane = zplane.transform(cameraTransform());
        Eigen::Vector3d n = zplane.normal();
        Eigen::Vector3d v = Eigen::Vector3d(-x + (_image.cols / 2), -y + (_image.rows / 2), g());   // get position on image plane coordinates from pixel
        double f = -zplane.offset() / n.dot(v);     // calculate scaling factor
        return v * f;   // return scaled position
    }
    inline Eigen::Vector3d pixelToCamera(const Eigen::Vector2d &p) { return pixelToCamera(p(0), p(1)); }
    inline Eigen::Vector3d pixelToCamera(const QPointF &p) { return pixelToCamera(p.x(), p.y()); }

    // world <-> pixel
    inline Eigen::Vector2d worldToPixel(const double x, const double y, const double z) { return cameraToPixel(worldToCamera(Eigen::Vector3d(x, y, z))); }
    inline Eigen::Vector2d worldToPixel(const Eigen::Vector3d v) { return cameraToPixel(worldToCamera(v)); }
    inline Eigen::Vector3d pixelToWorld(const double x, const double y) { return cameraTransform().inverse() * pixelToCamera(x, y); }
    inline Eigen::Vector3d pixelToWorld(const cv::Point2f p) { return pixelToWorld(p.x, p.y); }
    inline Eigen::Vector3d pixelToWorld(const Eigen::Vector2d &p) { return pixelToWorld(p(0), p(1)); }
    inline Eigen::Vector3d pixelToWorld(const QPointF &p) { return pixelToWorld(p.x(), p.y()); }

    // helper
    inline Eigen::Line lineAtX(const double x);
    inline Eigen::Line lineAtY(const double y);

private:
    // camera
    void calculateCameraTransfrom();
    void optimizeCameraTransform(const uint iterations = 15, const double epsilon = 0.00001);
    inline void ensureCameraTransfrom() { if(!_calculate_camera_transform_valid) { calculateCameraTransfrom(); _calculate_camera_transform_valid = true; optimizeCameraTransform(); } }
    bool _calculate_camera_transform_valid;
    bool _optimize_camera_transform_run;
    Eigen::Vector3d _camera_position;   // position of camera in relation to optimal origin of bottle in (mm/px ?)
    Eigen::Vector3d _camera_rotation;   // rotation of camera in radians

    // bottle top
    void findBottleHeights(const double radius = 10.5);
    std::vector<double> _bottle_heights;
    void findBottleHeight();
    double _bottle_height;

    // thread
    void findThreadROI();
    inline bool threadROIFound() { findThreadROI(); return !_threadRoi.isEmpty(); }
    QRect _threadRoi;
    cv::Mat _threadRoiImage;

    void findThread();
    BottleThreadInspector *_threadInspector;

    // keypoints
    std::vector<Eigen::Vector3d> _keypointsWorld;

    // bottle thread
//    void findBottleThread();
};

#endif // BOTTLEINSPECTOR_H
