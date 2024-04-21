#ifndef EIGEN_SUPPORT_H
#define EIGEN_SUPPORT_H

#ifdef Q_OS_WIN
#pragma warning(push)
#pragma warning(disable: 4522)
#endif
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#ifdef Q_OS_WIN
#pragma warning(pop)
#endif

#include <QDataStream>
#include <QDebug>
#include <QLineF>
#include <QPointF>
#include <QRect>
#include <QVector>

#include <opencv2/core.hpp>


EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector2d)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector3d)


namespace Eigen
{

typedef Transform<double, 3, Eigen::Affine> Transform3d;
typedef Hyperplane<double, 3> Plane;
typedef Hyperplane<double, 2> Line;

inline bool Vector3dSortByY(Eigen::Vector3d i, Eigen::Vector3d j) { return (i(1) < j(1)); }

//! Create Line from origin and direction
inline Line lineFromOriginAndDirection(const Vector2d &origin, const Vector2d &direction)
{
    // create ParametrizedLine
    ParametrizedLine<double, 2> pline = ParametrizedLine<double, 2>(origin, direction);

    // convert to Line
    return Line(pline);
}

//! Create Line from output of cv::fitLine etc.
inline Line lineFromVec4f(const cv::Vec4f &vec)
{
    // create ParametrizedLine
    ParametrizedLine<double, 2> pline = ParametrizedLine<double, 2>(Vector2d(vec[2], vec[3]), Vector2d(vec[0], vec[1]));

    // convert to Line
    return Line(pline);
}

//! Get coordinates of intersection from line and plane with defined x
inline Vector2d intersectLineWithX(const Line &line, double x)
{
    // construct line with x-coordinate
    Line xline = Line(Vector2d(-1, 0), x);

    // return intersection with line
    return xline.intersection(line);
}

//! Get coordinates of intersection from line and plane with defined y
inline Vector2d intersectLineWithY(const Line &line, double y)
{
    // construct line with y-coordinate
    Line yline = Line(Vector2d(0, 1), -y);

    // return intersection with line
    return yline.intersection(line);
}

//! Create QLineF from Line and Rect
inline QLineF intersectLineWithRect(const Line &line, const QRectF &rect)
{
    QLineF output_line;

    double isX = (std::fabs(line.normal().dot(Eigen::Vector2d(1, 0))) < 1e-10);
    double isY = (std::fabs(line.normal().dot(Eigen::Vector2d(0, 1))) < 1e-10);

    // calculate intersections
    Vector2d points[4];
    points[0] = !isX ? intersectLineWithY(line, rect.top()) : Vector2d(INT_MAX, INT_MAX);
    points[1] = !isX ? intersectLineWithY(line, rect.bottom()) : Vector2d(INT_MAX, INT_MAX);
    points[2] = !isY ? intersectLineWithX(line, rect.left()) : Vector2d(INT_MAX, INT_MAX);
    points[3] = !isY ? intersectLineWithX(line, rect.right()) : Vector2d(INT_MAX, INT_MAX);

    for (int i=0; i<3; i++) {
        QPointF p1 = QPointF(std::round(points[i][0]), std::round(points[i][1]));

        if (rect.contains(p1)) {
            for (int j=i+1; j<4; j++) {
                QPointF p2 = QPointF(std::round(points[j][0]), std::round(points[j][1]));

                if (rect.contains(p2)) {

                    output_line = QLineF(points[i][0], points[i][1], points[j][0], points[j][1]);
                    break;
                }
            }
            break;
        }
    }

    return output_line;
}

inline QLineF intersectLineWithRect(const Line &line, const QRect &rect)
{
    return intersectLineWithRect(line, QRectF(rect));
}

inline QPointF pointFromVector(const Vector2d &vec)
{
    return QPointF(vec(0), vec(1));
}

}

template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline QDebug operator<< (QDebug d, const Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols > &mat)
{
    int r = mat.rows();
    int c = mat.cols();

    d << "Eigen::Matrix(" << r << ", " << c << ") [";
    for (int i=0; i<r; i++) {
        d << "[";
        for (int j=0; j<c; j++) {
            d << mat(i, j);
            if (j<c-1)
                d << ", ";
        }
        d << "]";
    }
    d << "]";

    return d;
}

template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline QDataStream &operator<<(QDataStream &stream, const Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols > &mat)
{
    int r = mat.rows();
    int c = mat.cols();
    stream << r << c;

    _Scalar *data = new _Scalar[r*c];
    for (int i=0; i<r; i++)
        for (int j=0; j<c; j++)
            data[i*c+j] = mat(i, j);
    int bytes = r*c*sizeof(_Scalar);
    int bytes_written = stream.writeRawData((const char*)data, bytes);
    if (bytes != bytes_written) qFatal("EigenUtils.h operator<< failure.");

    delete[] data;
    return stream;
}

template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline QDataStream &operator>>(QDataStream &stream, Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols > &mat)
{
    int r, c;
    stream >> r >> c;
    mat.resize(r, c);

    _Scalar *data = new _Scalar[r*c];
    int bytes = r*c*sizeof(_Scalar);
    int bytes_read = stream.readRawData((char*)data, bytes);
    if (bytes != bytes_read) qFatal("EigenUtils.h operator>> failure.");
    for (int i=0; i<r; i++)
        for (int j=0; j<c; j++)
            mat(i, j) = data[i*c+j];

    delete[] data;
    return stream;
}

#endif // EIGEN_SUPPORT_H
