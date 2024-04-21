#ifndef OPENCV_SUPPORT_H
#define OPENCV_SUPPORT_H

#include "eigen_support.h"

#include <QDebug>
#include <QPolygon>
#include <QPoint>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>


namespace cv
{

//! edge direction
enum EdgeDirection { EDGE_WHITE_TO_BLACK, EDGE_BOTH, EDGE_BLACK_TO_WHITE };

//! egde position
enum EdgePosition { EDGE_FIRST, EDGE_LAST };

//! egde orientation, direction is always white to black!
enum EdgeOrientation { EDGE_HORIZONTAL, EDGE_VERTICAL, EDGE_UNKNOWN,
                       EDGE_O = 0, EDGE_NO = 1, EDGE_N = 2, EDGE_NW = 3, EDGE_W = 4, EDGE_SW = 5, EDGE_S = 6, EDGE_SO = 7,
                       EDGE_R = 0, EDGE_UR = 1, EDGE_U = 2, EDGE_UL = 3, EDGE_L = 4, EDGE_DL = 5, EDGE_D = 6, EDGE_DR = 7 };

inline bool KeypointSortByY(KeyPoint i, KeyPoint j) { return (i.pt.y < j.pt.y); }

inline void magnitude(const Mat& src, Mat& dst, Mat& sobel_x, Mat& sobel_y, Mat& smoothed, const int ksize = 3)
{
    // smooth
//    cv::GaussianBlur(src, smoothed, Size(ksize, ksize), 0);
    if (ksize > 0) {
        cv::medianBlur(src, smoothed, ksize);
    } else {
        smoothed = src;
    }

    // sobel
    cv::Sobel(smoothed, sobel_x, CV_32F, 1, 0, cv::FILTER_SCHARR);
    cv::Sobel(smoothed, sobel_y, CV_32F, 0, 1, cv::FILTER_SCHARR);

    // magnitude
    cv::magnitude(sobel_x, sobel_y, dst);
}

inline double findBestEdge(const Mat& magnitude, const EdgeOrientation dim, const double precision = 0.05, const EdgeDirection dir = EDGE_BOTH)
{
    assert(dim == EDGE_HORIZONTAL || dim == EDGE_VERTICAL);

    cv::Mat reduced;
    double minVal, maxVal;
    Point minLoc, maxLoc;

    // reduce matrix to vector
    if (dir == EDGE_BOTH)
        cv::reduce(abs(magnitude), reduced, dim == cv::EDGE_HORIZONTAL, cv::REDUCE_SUM, CV_64F);
    else
        cv::reduce(magnitude, reduced, dim == cv::EDGE_HORIZONTAL, cv::REDUCE_SUM, CV_64F);

    // make all vectors have a width of 1
    if (dim == cv::EDGE_VERTICAL)
        cv::transpose(reduced, reduced);

    if (precision != 1.0) {
        // scaling of array necessary
        cv::Mat scaled;
        cv::Size size = cv::Size(1, reduced.size().height / precision);
        cv::resize(reduced, scaled, size, 0, 0, cv::INTER_CUBIC);

        cv::minMaxLoc(scaled, &minVal, &maxVal, &minLoc, &maxLoc);
    } else {
        cv::minMaxLoc(reduced, &minVal, &maxVal, &minLoc, &maxLoc);
    }

    // select result
    double result;
    if ((dir == EDGE_WHITE_TO_BLACK && minVal < 0)) { // || (dir != EDGE_WHITE_TO_BLACK && -minVal > maxVal)) {
        result = minLoc.y * precision;
    } else {
        result = maxLoc.y * precision;
    }

    // add correct offset to go from px to coordinate
    return result + (precision / 2);
}

inline double findEdge2(const Mat& magnitude, const double thresholdVal, const EdgeDirection dir)
{
    // find first position above threshold
    int pos = 0;
    for (int i = 0; i < magnitude.rows - 1; i++) {
        double value = magnitude.at<double>(i, 0);
        double next = magnitude.at<double>(i + 1, 0);
        
        if (dir == EDGE_WHITE_TO_BLACK) {
            if (value < thresholdVal && value < next) {
                pos = i;
                break;
            }
        } else {
            if (value > thresholdVal && value > next) {
                pos = i;
                break;
            }
        }
    }
    
    return pos + 0.5;
}

inline double findEdge(const Mat& magnitude, const EdgeOrientation dim, const double threshold = 0.25, const double precision = 0.05, const EdgeDirection dir = EDGE_BOTH, const EdgePosition edge_pos = EDGE_FIRST)
{
    cv::Mat reduced;
    double minVal, maxVal;

    // reduce matrix to vector
    if (dir == EDGE_BOTH)
        cv::reduce(abs(magnitude), reduced, dim == cv::EDGE_HORIZONTAL, cv::REDUCE_SUM, CV_64F);
    else
        cv::reduce(magnitude, reduced, dim == cv::EDGE_HORIZONTAL, cv::REDUCE_SUM, CV_64F);

    // make all vectors have a width of 1
    if (dim == cv::EDGE_VERTICAL)
        cv::transpose(reduced, reduced);
    
    cv::minMaxLoc(reduced, &minVal, &maxVal);
    
    if (edge_pos == EDGE_LAST) {
        // flip array
        cv::Mat tmp;
        cv::flip(reduced, tmp, 0);
        reduced = tmp;
    }

    // find first position above threshold
    int pos = 0;
    for (int i = 0; i < reduced.rows - 1; i++) {
        double value = reduced.at<double>(i, 0);
        double next = reduced.at<double>(i + 1, 0);

        if (dir == EDGE_WHITE_TO_BLACK) {
            if (value < minVal * threshold && value < next) {
                pos = i;
                break;
            }
        } else {
            if (value > maxVal * threshold && value > next) {
                pos = i;
                break;
            }
        }
    }

    double result;
    if (precision == 1.0) {
        if (edge_pos == EDGE_FIRST) {
            result = pos * precision + (precision / 2);
        } else {
            result = (reduced.rows - pos) * precision - (precision / 2);
        }
    } else {
        // scaling of array necessary
        //cv::Size size = cv::Size(1, reduced.size().height / precision);
        //cv::resize(reduced, scaled, size, 0, 0, cv::INTER_CUBIC);
        
        int offset = 4;
        
        int min = std::max(0, pos - offset);
        int max = std::min(pos + offset, reduced.rows);
        
        cv::Mat area = reduced.rowRange(min, max);
//        qDebug() << "area size" << area.size().width << area.size().height;
//        QString values = "R";
//        for (int i=0; i<area.size().height; ++i) {
//            values.append(QString(" %1").arg(area.at<double>(i, 0)));
//        }
//        qDebug() << values;
        
        cv::Mat scaled;
        cv::Size size = cv::Size(1, (max - min) / precision);
        cv::resize(area, scaled, size, 0, 0, cv::INTER_CUBIC);
//        values = "S";
//        for (int i=0; i<scaled.size().height; ++i) {
//            values.append(QString(" %1").arg(scaled.at<double>(i, 0)));
//        }
//        qDebug() << values;
        /*
        cv::resize(area, scaled, size, 0, 0, cv::INTER_LINEAR_EXACT);
        values = "";
        for (int i=0; i<scaled.size().height; ++i) {
            values.append(QString("%1 ").arg(scaled.at<double>(i, 0)));
        }
        qDebug() << values;
        
        cv::resize(area, scaled, size, 0, 0, cv::INTER_LANCZOS4);
        values = "";
        for (int i=0; i<scaled.size().height; ++i) {
            values.append(QString("%1 ").arg(scaled.at<double>(i, 0)));
        }
        qDebug() << values;
        */
        double thresholdValue = dir == EDGE_WHITE_TO_BLACK ? minVal * threshold : maxVal * threshold;
        double sub_result = findEdge2(scaled, thresholdValue, dir);
        
        if (edge_pos == EDGE_FIRST) {
            //result = pos * precision + (precision / 2);
            //result = min + sub_result;
            result = min + (sub_result * precision);
//
//            qDebug() << "F pos" << pos << "sub" << sub_result << "result" << result << "min" << min << "max" << max;
        } else {
            
            //result = (scaled.rows - pos) * precision - (precision / 2);
            //result = (reduced.rows - pos) - offset + (sub_result * precision) - (precision / 2);
            
            result = reduced.rows - (min + (sub_result * precision));
//
//            qDebug() << "L pos" << pos << "sub" << sub_result << "result" << result << "min" << min << "max" << max;
        }
    }

    return result;
}

inline double findEdgeOld(const Mat& magnitude, const EdgeOrientation dim, const double threshold = 0.25, const double precision = 0.05, const EdgeDirection dir = EDGE_BOTH, const EdgePosition edge_pos = EDGE_FIRST)
{
    cv::Mat reduced;
    double minVal, maxVal;

    // reduce matrix to vector
    if (dir == EDGE_BOTH)
        cv::reduce(abs(magnitude), reduced, dim == cv::EDGE_HORIZONTAL, cv::REDUCE_SUM, CV_64F);
    else
        cv::reduce(magnitude, reduced, dim == cv::EDGE_HORIZONTAL, cv::REDUCE_SUM, CV_64F);

    // make all vectors have a width of 1
    if (dim == cv::EDGE_VERTICAL)
        cv::transpose(reduced, reduced);

    cv::Mat scaled;
    if (precision != 1.0) {
        // scaling of array necessary
        cv::Size size = cv::Size(1, reduced.size().height / precision);
        cv::resize(reduced, scaled, size, 0, 0, cv::INTER_CUBIC);

        cv::minMaxLoc(scaled, &minVal, &maxVal);
    } else {
        scaled = reduced;
        cv::minMaxLoc(reduced, &minVal, &maxVal);
    }

    if (edge_pos == EDGE_LAST) {
        // flip array
        cv::Mat tmp;
        cv::flip(scaled, tmp, 0);
        scaled = tmp;
    }

    // find first position above threshold
    int pos = 0;
    for (int i = 0; i < scaled.rows - 1; i++) {
        double value = scaled.at<double>(i, 0);
        double next = scaled.at<double>(i + 1, 0);

        if (dir == EDGE_WHITE_TO_BLACK) {
            if (value < minVal * threshold && value < next) {
                pos = i;
                break;
            }
        } else {
            if (value > maxVal * threshold && value > next) {
                pos = i;
                break;
            }
        }
    }

    double result;
    if (edge_pos == EDGE_FIRST) {
        result = pos * precision + (precision / 2);
    } else {
        result = (scaled.rows - pos) * precision - (precision / 2);
    }

    return result;
}

inline EdgeOrientation edgeOrientation(double orientation)
{
    if (orientation > M_PI || orientation < -M_PI) {
        orientation = std::fmod(orientation, M_PI);
        qDebug() << "WARNING: radians out of range for edgeOrientation!";
    }

    if (orientation > -M_PI / 8 && orientation <= M_PI / 8) {
        return EDGE_O;
    } else if (orientation > M_PI / 8 && orientation <= M_PI * 3 / 8) {
        return EDGE_SO;
    } else if (orientation > M_PI * 3 / 8 && orientation <= M_PI * 5 / 8) {
        return EDGE_S;
    } else if (orientation > M_PI * 5 / 8 && orientation <= M_PI * 7 / 8) {
        return EDGE_SW;
    } else if ((orientation > M_PI * 7 / 8 || orientation < -M_PI * 7 / 8)) {
        return EDGE_W;
    } else if (orientation > -M_PI * 7 / 8 && orientation <= -M_PI * 5 / 8) {
        return EDGE_NW;
    } else if (orientation > -M_PI * 5 / 8 && orientation <= -M_PI * 3 / 8) {
        return EDGE_N;
    } else if (orientation > -M_PI * 3 / 8 && orientation <= -M_PI / 8) {
        return EDGE_NO;
    }

	return EDGE_UNKNOWN;
}

inline EdgeOrientation edgeOrientation(double soble_x, double soble_y)
{
    return edgeOrientation(std::atan2(soble_y, soble_x));
}

inline void trace_edge_best(QPolygon& contour, const Mat& magnitude, const Mat& sobel_x, const Mat& sobel_y, const QPoint& pixel, const QPoint& previous_pixel)
{
    // check for edges of image or loops
    if (pixel.x() == 0 || pixel.x() == magnitude.cols - 1 || pixel.y() == 0 || pixel.y() == magnitude.rows -1 || contour.contains(pixel))
        return;
    else
        contour.append(pixel);

    // extract chocies for possible next edge positions
    cv::Mat choices = magnitude.colRange(pixel.x() - 1, pixel.x() + 2).rowRange(pixel.y() - 1, pixel.y() + 2).clone();
    choices.at<float>(1, 1) = 0;

    // eliminate choices based on current orientation
    double sobel_x_value = sobel_x.at<float>(pixel.y(), pixel.x());
    double sobel_y_value = sobel_y.at<float>(pixel.y(), pixel.x());
    double orientation = std::atan2(sobel_y_value, sobel_x_value);
    switch (edgeOrientation(orientation)) {
    case EDGE_O:
//        qDebug() << "O";
        choices.at<float>(1, 0) = 0;
        choices.at<float>(1, 2) = 0;
        break;
    case EDGE_NO:
//        qDebug() << "NO";
        choices.at<float>(0, 2) = 0;
        choices.at<float>(2, 0) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(0, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(2, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 0) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 2) = 0;
        break;
    case EDGE_N:
//        qDebug() << "N";
        choices.at<float>(0, 1) = 0;
        choices.at<float>(2, 1) = 0;
        break;
    case EDGE_NW:
//        qDebug() << "NW";
        choices.at<float>(0, 0) = 0;
        choices.at<float>(2, 2) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(2, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(0, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 2) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 0) = 0;
        break;
    case EDGE_W:
//        qDebug() << "W";
        choices.at<float>(1, 0) = 0;
        choices.at<float>(1, 2) = 0;
        break;
    case EDGE_SW:
//        qDebug() << "SW";
        choices.at<float>(0, 2) = 0;
        choices.at<float>(2, 0) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(0, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(2, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 0) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 2) = 0;
        break;
    case EDGE_S:
//        qDebug() << "S";
        choices.at<float>(0, 1) = 0;
        choices.at<float>(2, 1) = 0;
        break;
    case EDGE_SO:
//        qDebug() << "SO";
        choices.at<float>(0, 0) = 0;
        choices.at<float>(2, 2) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(2, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(0, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 2) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 0) = 0;
        break;
    }

    // make sure we don't go back
    if (previous_pixel.x() > pixel.x()) {
        choices.at<float>(0, 2) = 0;
        choices.at<float>(1, 2) = 0;
        choices.at<float>(2, 2) = 0;
    } else if (previous_pixel.x() < pixel.x()) {
        choices.at<float>(0, 0) = 0;
        choices.at<float>(1, 0) = 0;
        choices.at<float>(2, 0) = 0;
    }
    if (previous_pixel.y() > pixel.y()) {
        choices.at<float>(2, 0) = 0;
        choices.at<float>(2, 1) = 0;
        choices.at<float>(2, 2) = 0;
    } else if (previous_pixel.y() < pixel.y()) {
        choices.at<float>(0, 0) = 0;
        choices.at<float>(0, 1) = 0;
        choices.at<float>(0, 2) = 0;
    }

    // eliminate pixel with significantly different orientation
    /*
    for (int r=0; r < choices.rows; r++) {
        for (int c=0; c < choices.cols; c++) {
            double value = choices.at<float>(r, c);
            if (value) {
                double gray_x = sobel_x.at<float>(pixel.y() - 1 + r, pixel.x() - 1 + c);
                double gray_y = sobel_y.at<float>(pixel.y() - 1 + r, pixel.x() - 1 + c);
                double diff = orientation - std::atan2(gray_y, gray_x);
                qDebug() << orientation << std::atan2(gray_y, gray_x) << diff << std::fmod(std::fabs((orientation + M_PI) - (std::atan2(gray_y, gray_x) + M_PI)), M_PI);
                if (std::fmod(std::fabs(orientation - std::atan2(gray_y, gray_x)), M_PI) > M_PI / 9 &&
                        std::fmod(std::fabs(atan2(gray_y, gray_x) - orientation), M_PI) < M_PI * 8 / 9) {
                    choices.at<float>(r, c) = 0;
                    qDebug() << "eliminate";
                }
            }
//            qDebug() << r << c << value;
        }
    }
*/
    // fine best next step
    double maxVal;
    cv::Point maxLoc;
    cv::minMaxLoc(choices, nullptr, &maxVal, nullptr, &maxLoc);

//    for (int r=0; r < choices.rows; r++) {
//        for (int c=0; c < choices.cols; c++) {
//            float value = choices.at<float>(r, c);
//            qDebug() << r << c << value;
//        }
//    }
//    qDebug() << "took" << maxVal;

    // check if there actually is an edge
    if (maxVal > 0)
        trace_edge_best(contour, magnitude, sobel_x, sobel_y, QPoint(pixel.x() -1 + maxLoc.x, pixel.y() -1 + maxLoc.y), pixel);
}

inline void trace_edge(QPolygon& contour, const Mat& magnitude, const Mat& sobel_x, const Mat& sobel_y, const QPoint& pixel, const QPoint& previous_pixel)
{
    // check for edges of image or loops
    if (pixel.x() == 0 || pixel.x() == magnitude.cols - 1 || pixel.y() == 0 || pixel.y() == magnitude.rows -1 || contour.contains(pixel))
        return;
    else
        contour.append(pixel);

    // extract chocies for possible next edge positions
    cv::Mat choices = magnitude.colRange(pixel.x() - 1, pixel.x() + 2).rowRange(pixel.y() - 1, pixel.y() + 2).clone();
    choices.at<float>(1, 1) = 0;

    // eliminate choices based on current orientation
    double sobel_x_value = sobel_x.at<float>(pixel.y(), pixel.x());
    double sobel_y_value = sobel_y.at<float>(pixel.y(), pixel.x());
    double orientation = std::atan2(sobel_y_value, sobel_x_value);
    switch (edgeOrientation(orientation)) {
    case EDGE_O:
//        qDebug() << "O";
        choices.at<float>(1, 0) = 0;
        choices.at<float>(1, 2) = 0;
        break;
    case EDGE_NO:
//        qDebug() << "NO";
        choices.at<float>(0, 2) = 0;
        choices.at<float>(2, 0) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(0, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(2, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 0) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 2) = 0;
        break;
    case EDGE_N:
//        qDebug() << "N";
        choices.at<float>(0, 1) = 0;
        choices.at<float>(2, 1) = 0;
        break;
    case EDGE_NW:
//        qDebug() << "NW";
        choices.at<float>(0, 0) = 0;
        choices.at<float>(2, 2) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(2, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(0, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 2) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 0) = 0;
        break;
    case EDGE_W:
//        qDebug() << "W";
        choices.at<float>(1, 0) = 0;
        choices.at<float>(1, 2) = 0;
        break;
    case EDGE_SW:
//        qDebug() << "SW";
        choices.at<float>(0, 2) = 0;
        choices.at<float>(2, 0) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(0, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(2, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 0) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 2) = 0;
        break;
    case EDGE_S:
//        qDebug() << "S";
        choices.at<float>(0, 1) = 0;
        choices.at<float>(2, 1) = 0;
        break;
    case EDGE_SO:
//        qDebug() << "SO";
        choices.at<float>(0, 0) = 0;
        choices.at<float>(2, 2) = 0;

        if (previous_pixel.x() < pixel.x())
            choices.at<float>(2, 1) = 0;
        else if (previous_pixel.x() > pixel.x())
            choices.at<float>(0, 1) = 0;
        if (previous_pixel.y() < pixel.y())
            choices.at<float>(1, 2) = 0;
        else if (previous_pixel.y() > pixel.y())
            choices.at<float>(1, 0) = 0;
        break;
    }

    // make sure we don't go back
    if (previous_pixel.x() > pixel.x()) {
        choices.at<float>(0, 2) = 0;
        choices.at<float>(1, 2) = 0;
        choices.at<float>(2, 2) = 0;
    } else if (previous_pixel.x() < pixel.x()) {
        choices.at<float>(0, 0) = 0;
        choices.at<float>(1, 0) = 0;
        choices.at<float>(2, 0) = 0;
    }
    if (previous_pixel.y() > pixel.y()) {
        choices.at<float>(2, 0) = 0;
        choices.at<float>(2, 1) = 0;
        choices.at<float>(2, 2) = 0;
    } else if (previous_pixel.y() < pixel.y()) {
        choices.at<float>(0, 0) = 0;
        choices.at<float>(0, 1) = 0;
        choices.at<float>(0, 2) = 0;
    }

    // eliminate pixel with significantly different orientation
    /*
    for (int r=0; r < choices.rows; r++) {
        for (int c=0; c < choices.cols; c++) {
            double value = choices.at<float>(r, c);
            if (value) {
                double gray_x = sobel_x.at<float>(pixel.y() - 1 + r, pixel.x() - 1 + c);
                double gray_y = sobel_y.at<float>(pixel.y() - 1 + r, pixel.x() - 1 + c);
                double diff = orientation - std::atan2(gray_y, gray_x);
                qDebug() << orientation << std::atan2(gray_y, gray_x) << diff << std::fmod(std::fabs((orientation + M_PI) - (std::atan2(gray_y, gray_x) + M_PI)), M_PI);
                if (std::fmod(std::fabs(orientation - std::atan2(gray_y, gray_x)), M_PI) > M_PI / 9 &&
                        std::fmod(std::fabs(std::atan2(gray_y, gray_x) - orientation), M_PI) < M_PI * 8 / 9) {
                    choices.at<float>(r, c) = 0;
                    qDebug() << "eliminate";
                }
            }
//            qDebug() << r << c << value;
        }
    }
*/
    // fine best next step
    double maxVal;
    cv::Point maxLoc;
    cv::minMaxLoc(choices, nullptr, &maxVal, nullptr, &maxLoc);

//    for (int r=0; r < choices.rows; r++) {
//        for (int c=0; c < choices.cols; c++) {
//            float value = choices.at<float>(r, c);
//            qDebug() << r << c << value;
//        }
//    }
//    qDebug() << "took" << maxVal;

    QPoint choice = QPoint(pixel.x() -1 + maxLoc.x, pixel.y() -1 + maxLoc.y);

    double new_sobel_x_value = sobel_x.at<float>(choice.y(), choice.x());
    double new_sobel_y_value = sobel_y.at<float>(choice.y(), choice.x());
    double new_orientation = std::atan2(new_sobel_y_value, new_sobel_x_value);

    int range = 50;
    int edge_range = 5;

    qDebug() << choice;

    // check if there is a better "outer" choice
    if (choice.x() != pixel.x() && choice.y() == pixel.y()) {
        double offset = 0;
        if (new_orientation < 0) {
            cv::Mat m = magnitude.col(choice.x()).rowRange(choice.y() - range, choice.y() + edge_range);
            offset = -range - 0.5 + cv::findEdge(m, cv::EDGE_HORIZONTAL, 0.1, 1.0);
            qDebug() << "o" << new_orientation;
        } else {
            cv::Mat m = magnitude.col(choice.x()).rowRange(choice.y() - edge_range, choice.y() + range);
            offset = -edge_range - 0.5 + cv::findEdge(m, cv::EDGE_HORIZONTAL, 0.1, 1.0, cv::EDGE_BOTH, cv::EDGE_LAST);
            qDebug() << "u" << new_orientation;
        }
        qDebug() << "offset" << offset;
        choice.setY(choice.y() + offset);
    } else if (choice.x() == pixel.x() && choice.y() != pixel.y()) {
        double offset = 0;
        if (new_orientation < -M_PI_2 || new_orientation > M_PI_2) {
            cv::Mat m = magnitude.row(choice.y()).colRange(choice.x() - range, choice.x() + edge_range);
            offset = -range - 0.5 + cv::findEdge(m, cv::EDGE_VERTICAL, 0.1, 1.0);
            qDebug() << "l" << new_orientation;
        } else {
            cv::Mat m = magnitude.row(choice.y()).colRange(choice.x() - edge_range, choice.x() + range);
            offset = -edge_range - 0.5 + cv::findEdge(m, cv::EDGE_VERTICAL, 0.1, 1.0, cv::EDGE_BOTH, cv::EDGE_LAST);
            qDebug() << "r" << new_orientation;
//            for (int c=0; c < m.cols; c++) {
//                qDebug() << c << m.at<float>(0, c);
//            }
        }
        qDebug() << "offset" << offset;
        choice.setX(choice.x() + offset);
    } else if ((choice.x() < pixel.x() && choice.y() < pixel.y())
               || (choice.x() > pixel.x() && choice.y() > pixel.y())) {
        double offset = 0;
        if (new_orientation < -M_PI_4 || new_orientation > 3 * M_PI_4) {
            qDebug() << "ol" << new_orientation;
            cv::Mat m = magnitude.colRange(choice.x() - range, choice.x() + edge_range).rowRange(choice.y() - range, choice.y() + edge_range).diag(0);
            offset = -range - 0.5 + cv::findEdge(m, cv::EDGE_HORIZONTAL, 0.1, 1.0);
        } else {
            qDebug() << "ur" << new_orientation;
            cv::Mat m = magnitude.colRange(choice.x() - edge_range, choice.x() + range).rowRange(choice.y() - edge_range, choice.y() + range).diag(0);
            offset = -edge_range - 0.5 + cv::findEdge(m, cv::EDGE_HORIZONTAL, 0.1, 1.0, cv::EDGE_BOTH, cv::EDGE_LAST);


            for (int r=0; r < m.rows; r++) {
                qDebug() << r << m.at<float>(r, 0);
            }
        }

        qDebug() << "offset" << offset;
        choice.setX(choice.x() + offset);
        choice.setY(choice.y() + offset);
    } else if ((choice.x() < pixel.x() && choice.y() > pixel.y())
               || (choice.x() > pixel.x() && choice.y() < pixel.y())) {
        if (new_orientation < M_PI_4 && new_orientation > -3 * M_PI_4) {
            qDebug() << "or" << new_orientation;
        } else {
            qDebug() << "ul" << new_orientation;
        }
    }

    // check if there actually is an edge
    if (maxVal > 0)
        trace_edge(contour, magnitude, sobel_x, sobel_y, choice, pixel);
}

inline int iterative_threshold(const Mat &image)
{
    // calculate histogram
    int hist_size = 256;
    Mat hist;
    calcHist(&image, 1, 0, Mat(), hist, 1, &hist_size, 0);

    int threshold = 0;
    int threshold_new = 128;

    do {
        double m1 = 0.0;
        double m2 = 0.0;
        int cnt1 = 0;
        int cnt2 = 0;

        threshold = threshold_new;

        for (int i=0; i < threshold; i++) {
            m1 += (hist.at<float>(i) * i);
            cnt1 += hist.at<float>(i);
        }
        if (m1)
            m1 /= cnt1;

        for (int i=threshold; i < hist_size; i++) {
            m2 += (hist.at<float>(i) * i);
            cnt2 += hist.at<float>(i);
        }
        if (m2)
            m2 /= cnt2;

        threshold_new = (m1 + m2) / 2.0;
    } while (threshold_new != threshold);

    return threshold_new;
}

inline std::tuple<cv::Vec4f, std::vector<cv::Point2d>> fitLineRANSAC(const std::vector<cv::Point2d>& points, const double max_distance_from_line = 1.0, const double min_distance_between_points = 1.0)
{
    cv::RNG rng;
    
    std::vector<cv::Point2d> bestInliers;
    for (const auto &p : points) {
        //pick two points
        int idx1 = rng.uniform(0, (int)points.size());
        int idx2 = rng.uniform(0, (int)points.size());
        cv::Point p1 = points[idx1];
        cv::Point p2 = points[idx2];

        // points too close - discard
        if (cv::norm(p1 - p2) < std::max(min_distance_between_points, max_distance_from_line)) {
            continue;
        }
        
        // line equation ->  (y1 - y2)X + (x2 - x1)Y + x1y2 - x2y1 = 0
        double a = p1.y - p2.y;
        double b = p2.x - p1.x;
        double c = (p1.x * p2.y) - (p2.x * p1.y);
        
        // normalize them
        double scale = 1.0 / std::sqrt(std::pow(a, 2) + std::pow(b, 2));
        a *= scale;
        b *= scale;
        c *= scale;

        // count inliers
        std::vector<cv::Point2d> inliers;
        for (const cv::Point2d &p0 : points) {
            double rho = std::abs(a*p0.x + b * p0.y + c);
            if (rho <= max_distance_from_line) {
                inliers.push_back(p0);
            }
        }
        
        if (inliers.size() > bestInliers.size()) {
            bestInliers = inliers;
        }
    }
    
    // calculate line
    cv::Vec4f line;
    if (bestInliers.size()) {
        cv::fitLine(bestInliers, line, cv::DIST_FAIR, 0, 0.01, 0.01);
    }
    
    return std::make_tuple(line, bestInliers);
}

inline std::tuple<cv::Vec4f, std::vector<cv::Point2d>> fitLineRANSAC(const std::vector<cv::Point2d>& points, const double max_distance_from_line, const double min_distance_between_points, const double min_inliners)
{
    const auto& [params, inliners] = cv::fitLineRANSAC(points, max_distance_from_line, min_distance_between_points);
    if (inliners.size() >= min_inliners) {
        return std::make_tuple(params, inliners);
    } else {
        return std::make_tuple(cv::Vec4f(), std::vector<cv::Point2d>());
    }
}

inline Eigen::Line fitLineWithNormal(const std::vector<cv::Point2d> &points, const Eigen::Vector2d &normal)
{
    // using all points!
    
    Eigen::Line best_line;
    double best_dist = UINT_MAX;
    double second_best_dist = UINT_MAX;
    
    cv::Point2d best_point;
    cv::Point2d second_best_point;
    
    // find two best points
    for (const auto &p_source : points) {
        // calculate line for p
        Eigen::ParametrizedLine<double, 2> pline = Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(p_source.x, p_source.y), normal);
        Eigen::Line line = Eigen::Line(pline);
        
        double sum_dist = std::accumulate(points.cbegin(), points.cend(), 0.0, [&line](const double &s, const cv::Point2d &p) {
            const double r = line.absDistance(Eigen::Vector2d(p.x, p.y));
            const double c = 5.0; // 1.345;
            return s + (r < c ? std::pow(r, 2) / 2 : c * (r - (c / 2))); // Huber Distance
        });
        
        if (sum_dist < best_dist) {
            second_best_point = best_point;
            second_best_dist = best_dist;
            
            best_line = line;
            best_dist = sum_dist;
            best_point = p_source;
        }
    }
    
    // randomly select center between two points to try to find better match
    cv::RNG rng;
    for (int i=0; i < (2 * points.size()); ++i) {
        //pick two points
        int idx1 = rng.uniform(0, (int)points.size());
        int idx2 = rng.uniform(0, (int)points.size());
        cv::Point p1 = points[idx1];
        cv::Point p2 = points[idx2];
        
        // calculate line for p
        Eigen::ParametrizedLine<double, 2> pline = Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0), normal);
        Eigen::Line line = Eigen::Line(pline);
        
        double sum_dist = std::accumulate(points.cbegin(), points.cend(), 0.0, [&line](const double &s, const cv::Point2d &p) {
            const double r = line.absDistance(Eigen::Vector2d(p.x, p.y));
            const double c = 5.0; // 1.345;
            return s + (r < c ? std::pow(r, 2) / 2 : c * (r - (c / 2))); // Huber Distance
        });
        
        if (sum_dist < best_dist) {
            second_best_point = best_point;
            second_best_dist = best_dist;
            
            best_line = line;
            best_dist = sum_dist;
            best_point = cv::Point2d((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);
        }
    }
    
    // optimize using these two points
    double orig_dist = best_dist;
    qDebug() << "Initial dist" << best_dist;
    
    int i = 0;
    while (best_dist != second_best_dist && i <= 100) {
        i++;
        cv::Point2d p_center = cv::Point2d((best_point.x + second_best_point.x) / 2.0, (best_point.y + second_best_point.y) / 2.0);
        Eigen::Line line = Eigen::Line(Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(p_center.x, p_center.y), normal));
        double sum_dist = std::accumulate(points.cbegin(), points.cend(), 0.0, [&line](const double &s, const cv::Point2d &p) {
            const double r = line.absDistance(Eigen::Vector2d(p.x, p.y));
            const double c = 5.0; // 1.345;
            return s + (r < c ? std::pow(r, 2) / 2 : c * (r - (c / 2))); // Huber Distance
        });
        
        cv::Point2d p_opposite = cv::Point2d(p_center.x < best_point.x ? best_point.x + std::abs(best_point.x - second_best_point.x) : best_point.x - std::abs(best_point.x - second_best_point.x), p_center.y < best_point.y ? best_point.y + std::abs(best_point.y - second_best_point.y) : best_point.y - std::abs(best_point.y - second_best_point.y));
        Eigen::Line line_opposite = Eigen::Line(Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(p_opposite.x, p_opposite.y), normal));
        double sum_dist_opposite = std::accumulate(points.cbegin(), points.cend(), 0.0, [&line_opposite](const double &s, const cv::Point2d &p) {
            const double r = line_opposite.absDistance(Eigen::Vector2d(p.x, p.y));
            const double c = 5.0; // 1.345;
            return s + (r < c ? std::pow(r, 2) / 2 : c * (r - (c / 2))); // Huber Distance
        });
        
        if (sum_dist < best_dist) {
            second_best_point = best_point;
            second_best_dist = best_dist;
            best_point = p_center;
            best_dist = sum_dist;
        } else if (sum_dist_opposite < best_dist) {
            second_best_point = best_point;
            second_best_dist = best_dist;
            best_point = p_opposite;
            best_dist = sum_dist_opposite;
        } else if (sum_dist_opposite < sum_dist) {
            second_best_point = p_opposite;
            second_best_dist = sum_dist_opposite;
        } else {
            second_best_point = p_center;
            second_best_dist = sum_dist;
        }
    }
    
    qDebug() << "converged dist:" << best_dist << "diff:" << (std::abs(orig_dist - best_dist)) << "after" << i;
    
    return best_line;
}


// QDebug operators

inline QDebug operator<<(QDebug dbg, const Mat &m)
{
    dbg.nospace() << "Mat(" << m.cols << ", " << m.rows << ")";
    return dbg.space();
}

inline QDebug operator<<(QDebug dbg, const Point &p)
{
    dbg.nospace() << "(" << p.x << ", " << p.y << ")";
    return dbg.space();
}

inline QDebug operator<<(QDebug dbg, const Rect &r)
{
    dbg.nospace() << "(" << r.x << ", " << r.y << "," << r.width << "," << r.height << ")";
    return dbg.space();
}

inline QDebug operator<<(QDebug dbg, const Size &s)
{
    dbg.nospace() << "(" << s.width << ", " << s.height << ")";
    return dbg.space();
}

// QDataStream operators

inline QDataStream &operator<<(QDataStream &stream, const Mat &m)
{
    // Write header
    int rows = m.rows;
    int cols = m.cols;
    int type = m.type();
    stream << rows << cols << type;

    // Write data
    int len = rows * cols * (int)m.elemSize();
    stream << len;
    if (len > 0) {
        if (!m.isContinuous()) qFatal("Can't serialize non-continuous matrices.");
        int written = stream.writeRawData((const char*)m.data, len);
        if (written != len) qFatal("Mat serialization failure, expected: %d bytes, wrote: %d bytes.", len, written);
    }
    return stream;
}

inline QDataStream &operator>>(QDataStream &stream, Mat &m)
{
    // Read header
    int rows, cols, type;
    stream >> rows >> cols >> type;
    m.create(rows, cols, type);

    int len;
    stream >> len;
    char *data = (char*) m.data;

    // In certain circumstances, like reading from stdin or sockets, we may not
    // be given all the data we need at once because it isn't available yet.
    // So we loop until it we get it.
    while (len > 0) {
        const int read = stream.readRawData(data, len);
        if (read == -1) qFatal("Mat deserialization failure, exptected %d more bytes.", len);
        data += read;
        len -= read;
    }
    return stream;
}

inline QDataStream &operator<<(QDataStream &stream, const Point &p)
{
    return stream << p.x << p.y;
}

inline QDataStream &operator>>(QDataStream &stream, Point &p)
{
    return stream >> p.x >> p.y;
}

inline QDataStream &operator<<(QDataStream &stream, const Point2f &p)
{
    return stream << p.x << p.y;
}

inline QDataStream &operator>>(QDataStream &stream, Point2f &p)
{
    return stream >> p.x >> p.y;
}

inline QDataStream &operator<<(QDataStream &stream, const Rect &r)
{
    return stream << r.x << r.y << r.width << r.height;
}

inline QDataStream &operator>>(QDataStream &stream, Rect &r)
{
    return stream >> r.x >> r.y >> r.width >> r.height;
}

inline QDataStream &operator<<(QDataStream &stream, const Size &s)
{
    return stream << s.width << s.height;
}

inline QDataStream &operator>>(QDataStream &stream, Size &s)
{
    return stream >> s.width >> s.height;
}

inline QDataStream &operator<<(QDataStream &stream, const KeyPoint &p)
{
    return stream << p.pt << p.size << p.angle << p.response << p.octave << p.class_id;
}

inline QDataStream &operator>>(QDataStream &stream, KeyPoint &p)
{
    return stream >> p.pt >> p.size >> p.angle >> p.response >> p.octave >> p.class_id;
}

}

#endif // OPENCV_SUPPORT_H
