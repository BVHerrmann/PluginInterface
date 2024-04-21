#ifndef BOTTLETHREADINSPECTOR_H
#define BOTTLETHREADINSPECTOR_H

#include <QObject>

#include <opencv2/core.hpp>

typedef enum {
    CorkBottle,
    CrownBottle,
    ThreadBottle
} BottleThreadType;

typedef enum {
    Left,
    Right
} BottleThreadSide;

struct BottleThread {
    BottleThreadSide side;
    double offset;
    double magnitude;
    cv::Point2d position;
    cv::Point2d correctedPosition;
};

class BottleThreadInspector : public QObject
{
    Q_OBJECT
public:
    explicit BottleThreadInspector(const cv::Mat &image, QObject *parent = 0);

    const cv::Mat& image() { findThread(); return _corrected; }

    BottleThreadType threadType() { findThreadType(); return _thread_type; }

    QList<BottleThread> threads() { findThreads(); return _threads; }

private:
    cv::Mat _image;

    void findThread();
    cv::Mat _warp;
    cv::Mat _warp_inv;
    cv::Mat _corrected;

    void findEdges();
    std::vector<double> _radius_left;
    std::vector<double> _radius_right;

    void findTop();
    double _top;

    void findInnerRadius();
    double _inner_radius;

    void findThreadType();
    BottleThreadType _thread_type;

    void findThreads();
    QList<BottleThread> _threads;
};

#endif // BOTTLETHREADINSPECTOR_H
