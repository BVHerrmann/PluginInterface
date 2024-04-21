#include "bottleimageinspector.h"

#include <QtCore>
#include <QBitmap>
#include <QPixmap>

#include "common.h"
#include "types.h"


BottleImageInspector::BottleImageInspector(const cv::Mat &image, const std::vector<cv::Point> roi) :
    ImageInspector(image)
{
    // check if roi is set directly
    _hull = roi;

    // helper
    _base_executed = false;
    _roi_executed = !_hull.empty();
    _center_executed = false;

    // set invalid base/center
    _base = Eigen::Line(Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(0, 0), Eigen::Vector2d(0,0)));
    _center = Eigen::Line(Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(0, 0), Eigen::Vector2d(0,0)));

    // initialize vectors to avoid constant checking/resizing later
    _left_profile.resize(image.rows);
    _right_profile.resize(image.rows);
    _smoothed_left_profile.resize(image.rows);
    _smoothed_right_profile.resize(image.rows);
}

BottleImageInspector::~BottleImageInspector()
{

}

const Eigen::Line BottleImageInspector::base()
{
    if (_base_executed) {
        return _base;
    }
    _base_executed = true;

    START_MEASURE_PERFORMANCE();

    cv::Range left_range(0, _image.cols / 2 - 1);
    cv::Range right_range(_image.cols / 2, _image.cols - 1);

    // get white levels for all columns
    cv::Mat hist;
    cv::reduce(_image.rowRange(_image.rows / 2, _image.rows), hist, 0, cv::REDUCE_SUM, CV_32F); // only use lower half of image

    // get extrema
    double max_val_left = 0.0;
    cv::Point max_loc_left;
    cv::minMaxLoc(hist.colRange(0, hist.cols / 2), nullptr, &max_val_left, nullptr, &max_loc_left);
    int max_idx_left = max_loc_left.x;

    double max_val_right = 0.0;
    cv::Point max_loc_right;
    cv::minMaxLoc(hist.colRange(hist.cols / 2, hist.cols), nullptr, &max_val_right, nullptr, &max_loc_right);
    int max_idx_right = max_loc_right.x + (hist.cols / 2);

    // find all measurements
    for (int i = max_idx_left; i >= 0; i--) {
        float value = hist.at<float>(0, i);
        if (value >= max_val_left * 0.9) {
            left_range.start = i;
        } else
            break;
    }
    for (int i = max_idx_left; i < _image.cols / 2; i++) {
        float value = hist.at<float>(0, i);
        if (value >= max_val_left * 0.9) {
            left_range.end = i;
        } else
            break;
    }

    for (int i = max_idx_right; i >= hist.cols / 2; i--) {
        float value = hist.at<float>(0, i);
        if (value >= max_val_right * 0.9) {
            right_range.start = i;
        } else
            break;
    }
    for (int i = max_idx_right; i < _image.cols; i++) {
        float value = hist.at<float>(0, i);
        if (value >= max_val_right * 0.9) {
            right_range.end = i;
        } else
            break;
    }

    // collect base points in both ranges
    std::vector<cv::Point2f> line_points;
    for (int i = left_range.start; i <= left_range.end; i++) {
        double p = findBestEdge(this->sobel_y().col(i), cv::EDGE_HORIZONTAL, 1.0, cv::EDGE_WHITE_TO_BLACK);
        line_points.push_back(cv::Point2f(i + 0.5, p));
    }
    for (int i = right_range.start; i <= right_range.end; i++) {
        double p = findBestEdge(this->sobel_y().col(i), cv::EDGE_HORIZONTAL, 1.0, cv::EDGE_WHITE_TO_BLACK);
        line_points.push_back(cv::Point2f(i + 0.5, p));
    }

    if (line_points.size() >= 2) {
        // enough points to construct a line found

        // find best line
        cv::Vec4f line_params;
        cv::fitLine(line_points, line_params, cv::DIST_FAIR, 0, 0.01 , 0.01);

        // construct base line
        _base = Eigen::lineFromVec4f(line_params);

    } else {
        qWarning() << "Base not found!";
        _base = Eigen::Line(Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(0, 0), Eigen::Vector2d(0,0)));
    }

#ifdef DEBUG
    qDebug() << "Base is" << _base.normal()(0) << _base.normal()(1) << "with offset" << _base.offset();
#endif
    STOP_MEASURE_PERFORMANCE();

    return _base;
}

const std::vector<cv::Point> BottleImageInspector::bottleHull(const uint border)
{
    if (_roi_executed) {
        return _hull;
    }
    _roi_executed = true;

    // dependent on base
    if (!baseFound()) {
		return _hull;
	}
	
	QLineF base = baseLine();

    START_MEASURE_PERFORMANCE();

    assert(_image.data);
    assert(_image.channels() == 1);

    // median blur image to remove most salt and pepper errors
    cv::Mat median = smoothed();

    // scale magnitude image to range 0..255
    double magnitude_min = 0.0;
    double magnitude_max = 0.0;
    cv::minMaxLoc(magnitude(), &magnitude_min, &magnitude_max);
    double alpha = magnitude_max > std::abs(magnitude_min) ? 255.0 / magnitude_max : 255.0 / std::abs(magnitude_min);
    cv::Mat magnitude_scaled;
    cv::convertScaleAbs(magnitude(), magnitude_scaled, alpha);

    // extract threshold image from magnitudes
    int magnitude_threshold = cv::iterative_threshold(magnitude_scaled.colRange(median.cols / 3, median.cols * 2 / 3));
    cv::Mat threshold_magnitude;
    cv::threshold(magnitude_scaled, threshold_magnitude, magnitude_threshold, 255, cv::THRESH_BINARY);

    // extract threshold image from median
    double best_threshold = cv::iterative_threshold(median.colRange(median.cols / 3, median.cols * 2 / 3));
    cv::Mat threshold;
    cv::threshold(median, threshold, best_threshold, 255, cv::THRESH_BINARY_INV);

    // combine both threshold images
    threshold += threshold_magnitude;

    // make everything below bottle black
    std::vector<cv::Point> baseArea;
    baseArea.push_back(cv::Point(base.p1().x(), base.p1().y() - border));
    baseArea.push_back(cv::Point(base.p2().x(), base.p2().y() - border));
    baseArea.push_back(cv::Point(_image.cols, _image.rows));
    baseArea.push_back(cv::Point(0, _image.rows));
    cv::Mat baseAreaMat = cv::Mat(baseArea);
    cv::fillConvexPoly(threshold, (const cv::Point*)baseAreaMat.data, baseAreaMat.rows * baseAreaMat.cols * baseAreaMat.channels() / 2, 0);

    // dilate bottle contour
    cv::Mat dilated;
    cv::dilate(threshold, dilated, cv::Mat(), cv::Point(-1,-1), border + 2);

    // find contours
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // select best contour
    std::vector<cv::Point> contour;
    std::vector<cv::Point> hull;
    double best_dist = INT_MIN;
    for (size_t i = 0; i < contours.size(); i++) {
        // get convex hull for contour
        std::vector<cv::Point> c_hull;
        cv::convexHull(cv::Mat(contours[i]), c_hull);

        // calculate distance
        double dist = cv::pointPolygonTest(cv::Mat(c_hull), cv::Point2f(_image.cols / 2, MIN(this->baseLine().p1().y(), this->baseLine().p2().y()) - 100), true);

        if (dist > best_dist) {
            best_dist = dist;
            contour = contours[i];
            hull = c_hull;
        }
    }

    if (best_dist < -_image.cols / 8.0) {
        qWarning() << "Contour too far of center, no usable ROI found!";
    } else if (cv::contourArea(cv::Mat(hull)) > _image.rows * _image.cols / 2.0) {
        qWarning() << "Contour too big, no usable ROI found!" << cv::contourArea(cv::Mat(hull)) << "vs" << _image.rows * _image.cols / 2;
    } else if (hull.size() < 4) {
        qWarning() << "Not enough contour points, no usable ROI found!" << hull.size();
    } else {
        // convert found roi
        _hull = hull;
    }

    STOP_MEASURE_PERFORMANCE();

    return _hull;
}

const QIcon BottleImageInspector::icon()
{
    if (!_icon.isNull()) {
        return _icon;
    }

    // get roi image
    QRectF rect = boundingRect();
    cv::Mat roi = _image.rowRange(0, rect.bottom()).colRange(rect.left(), rect.right());

    // floodfill background
    // NOTE: Do not use. This hangs sometimes as off OpenCV 2.4.2 (Bug #2258?)
    // cv::floodFill(roi, cv::Point(0, 0), cv::Scalar(255));
    // cv::floodFill(roi, cv::Point(0, roi.cols - 1), cv::Scalar(255));

    // create image and transperency mask
    QImage iconImage = MatToQImage(roi, true);
    iconImage = iconImage.scaled(256, 256,  Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage mask(iconImage.size(), QImage::Format_Mono);
    for (int x=0; x < iconImage.width(); x++) {
        for (int y=0; y < iconImage.height(); y++) {
            if (qGray(iconImage.pixel(x, y)) > 225) {
                mask.setPixel(x, y, 1);
            } else {
                mask.setPixel(x, y, 0);
            }
        }
    }

    // create icon
    QPixmap pixmap = QPixmap::fromImage(iconImage);
    pixmap.setMask(QPixmap::fromImage(mask));
    _icon = QIcon(pixmap);

    return _icon;
}

void BottleImageInspector::findBottleProfileInRow(const uint row, const double threshold, const double precision, QPointF *pointLeft, QPointF *pointRight)
{
    assert(row < (uint) _image.rows);

    if (!bottleROIFound()) {
        qWarning() << "Bottle ROI not found, can not execute" << FUNCTION_NAME << "!";
        return;
    }

    QRect bounding_rect = boundingRect().toRect();

    double dist_left = cv::pointPolygonTest(bottleHull(), cv::Point2f(bounding_rect.left(), row), true);
    double dist_right = cv::pointPolygonTest(bottleHull(), cv::Point2f(bounding_rect.right() + 1.0, row), true);
    int offset = std::ceil(std::abs(dist_left));
    assert(dist_left <= 0.0 && dist_right <= 0.0 && offset >= 0);

    cv::Mat m = magnitude().row(row).colRange(bounding_rect.left() + offset, bounding_rect.right() + std::floor(dist_right));

    double l = findEdge(m, cv::EDGE_VERTICAL, threshold, precision, cv::EDGE_BOTH, cv::EDGE_FIRST);
    double r = findEdge(m, cv::EDGE_VERTICAL, threshold, precision, cv::EDGE_BOTH, cv::EDGE_LAST);

    // calculate point
    const QPointF lp = QPointF(bounding_rect.left() + offset + l, row + 0.5);
    const QPointF rp = QPointF(bounding_rect.left() + offset + r, row + 0.5);

    // store
    _left_profile[row] = lp;
    _right_profile[row] = rp;

    // return
    if (pointLeft)
        *pointLeft = lp;
    if (pointRight)
        *pointRight = rp;
}

const QPointF BottleImageInspector::leftProfileInRow(const uint row, const double threshold, const double precision)
{
    assert(row < (uint) _image.rows);

    if (row < (uint) _image.rows) {
        QPointF point = _left_profile.at(row);

        if (point.isNull()) {
            findBottleProfileInRow(row, threshold, precision, &point, nullptr);
        }

        return point;
    } else {
        return QPointF();
    }
}

const QPointF BottleImageInspector::rightProfileInRow(const uint row, const double threshold, const double precision)
{
    assert(row < (uint) _image.rows);

    if (row < (uint) _image.rows) {
        QPointF point = _right_profile.at(row);

        if (point.isNull()) {
            findBottleProfileInRow(row, threshold, precision, nullptr, &point);
        }

        return point;
    } else {
        return QPointF();
    }
}

const cv::Mat BottleImageInspector::profileSmoothKernel(const int k_size)
{
    if (_profileSmoothKernel.empty()) {
       _profileSmoothKernel = cv::getGaussianKernel(k_size, 0);
    }

    return _profileSmoothKernel;
}

void BottleImageInspector::findSmoothedBottleProfileInRow(const uint row, const int k_size, const double threshold, const double precision, QPointF *pointLeft, QPointF *pointRight)
{
    assert(row < (uint) _image.rows);
    assert(k_size % 2 == 1);

    // get gaussian kernel
    const cv::Mat k = profileSmoothKernel(k_size);

    int start_row = row - std::floor((float) k_size / 2);
    int end_row = row + std::floor((float) k_size / 2);

    // apply kernel
    double left_value = 0;
    double right_value = 0;
    int k_row = 0;
    for (int i = start_row; i <= end_row; i++) {
        // get valid row
        int r = (i > 0) ? (i < _image.rows ? i : _image.rows - 1) : 0;
        left_value += (leftProfileInRow(r, threshold, precision).x() * k.at<double>(k_row, 0));
        right_value += (rightProfileInRow(r, threshold, precision).x() * k.at<double>(k_row, 0));
        k_row++;
    }

    // calculate point
    const QPointF lp = QPointF(left_value, row + 0.5);
    const QPointF rp = QPointF(right_value, row + 0.5);

    // store smoothed value
    _smoothed_left_profile[row] = lp;
    _smoothed_right_profile[row] = rp;

    // return
    if (pointLeft)
        *pointLeft = lp;
    if (pointRight)
        *pointRight = rp;
}

const QPointF BottleImageInspector::smoothedLeftProfileInRow(const uint row, const int k_size, const double threshold, const double precision)
{
    assert(row < (uint) _image.rows);

    if (row < (uint) _image.rows) {
        QPointF point = _smoothed_left_profile.at(row);

        if (point.isNull()) {
            findSmoothedBottleProfileInRow(row, k_size, threshold, precision, &point, nullptr);
        }

        return point;
    } else {
        return QPointF();
    }
}

const QPointF BottleImageInspector::smoothedRightProfileInRow(const uint row, const int k_size, const double threshold, const double precision)
{
    assert(row < (uint) _image.rows);

    if (row < (uint) _image.rows) {
        QPointF point = _smoothed_right_profile.at(row);

        if (point.isNull()) {
            findSmoothedBottleProfileInRow(row, k_size, threshold, precision, nullptr, &point);
        }

        return point;
    } else {
        return QPointF();
    }
}

const Eigen::Line BottleImageInspector::bottleCenter(const double orientationThreshold, const double orientationDiffThreshold, const int stretchThreshold)
{
    if (_center_executed) {
        return _center;
    }
    _center_executed = true;
    
    if (!bottleROIFound()) {
        qWarning() << "Bottle ROI not found, can not execute" << FUNCTION_NAME << "!";
        Eigen::ParametrizedLine<double, 2> cline = Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(_image.cols / 2, 0.0), Eigen::Vector2d(0.0, 1.0));
        _center = Eigen::Line(cline);
        return _center;
    }

    QRect bounding_rect = boundingRect().toRect();
    cv::Mat sobelx = sobel_x();
    cv::Mat sobely = sobel_y();

    START_MEASURE_PERFORMANCE();

    double dir_limit_l = radians(orientationThreshold);
    double dir_limit_u = radians(180.0 - orientationThreshold);
    double dir_limit_diff = radians(orientationDiffThreshold);

    uint start_pos = std::max(bounding_rect.top(), 0);
    uint end_pos = std::min(bounding_rect.bottom(), _image.rows - 1);

    int cur_stretch = 0;
    for (uint i = start_pos; i < end_pos; i++) {
        // determine left and right edge positions
        QPointF l = smoothedLeftProfileInRow(i);
        QPointF r = smoothedRightProfileInRow(i);

        // get direction of edges
        double l_dir = std::atan2(sobelx.at<float>(l.y(), l.x()), sobely.at<float>(l.y(), l.x()));
        double r_dir = std::atan2(sobelx.at<float>(r.y(), r.x()), sobely.at<float>(r.y(), r.x()));

        if (l_dir < -dir_limit_l &&  l_dir > -dir_limit_u
                && r_dir > dir_limit_l && r_dir < dir_limit_u
                && std::fabs(l_dir + r_dir) < dir_limit_diff) {
            // add potentially valid edge point
            cur_stretch++;
            if (cur_stretch > stretchThreshold) {
                double c = (l.x() + r.x()) / 2;
                _center_profile << QPointF(c, l.y());
            }
        } else {
            // delete last points, if necessary
            if (cur_stretch > stretchThreshold) {
                int del_points = cur_stretch > 3 * stretchThreshold ? stretchThreshold : cur_stretch - stretchThreshold;
                _center_profile.remove(_center_profile.size() - del_points, del_points);
            }
            cur_stretch = 0;
        }
    }

    // delete last points if necessary
    if (cur_stretch > stretchThreshold) {
        int del_points = cur_stretch > 3 * stretchThreshold ? stretchThreshold : cur_stretch - stretchThreshold;
        _center_profile.remove(_center_profile.size() - del_points, del_points);
    }

    if (_center_profile.size() >= 2) {
        // found enough points for center

        std::vector<cv::Point2f> center_points;
        for (const QPointF &p : _center_profile) {
            center_points.push_back(cv::Point2f(p.x(), p.y()));
        }

        // find best line
        cv::Vec4f line_params;
        cv::fitLine(cv::Mat(center_points), line_params, cv::DIST_L2, 0, 0.01 , 0.01);

        // construct center line
        _center = Eigen::lineFromVec4f(line_params);
    } else {
        qWarning() << "Center not found! Using default.";

        // construct default center line
        Eigen::ParametrizedLine<double, 2> pline = Eigen::ParametrizedLine<double, 2>(Eigen::Vector2d(boundingRect().center().x(), this->boundingRect().bottom()), Eigen::Vector2d(0.0, 1.0));
        _center = Eigen::Line(pline);
    }

    STOP_MEASURE_PERFORMANCE();

#ifdef DEBUG
    qDebug() << "Center is" << _center.normal()(0) << _center.normal()(1) << "with offset" << _center.offset();
#endif

    return _center;
}

const QPolygonF BottleImageInspector::topProfile()
{
    if (!bottleROIFound()) {
        qWarning() << "Bottle ROI not found, can not execute" << FUNCTION_NAME << "!";
        return _top_profile;
    }

    START_MEASURE_PERFORMANCE();

    // TODO: half of rect should be more than enough
    for (int i = boundingRect().left(); i <= boundingRect().right(); i++) {
        findBottleTopInColumn(i);
    }

    STOP_MEASURE_PERFORMANCE();

    return _top_profile;
}

const QPointF BottleImageInspector::findBottleTopInColumn(const uint col, const double threshold, const double precision)
{
    if (col > (uint)_image.cols) {
        qWarning() << "Column" << col << "out of image range, can not execute" << FUNCTION_NAME << "!";
        return QPointF();
    }

    // hit test
    if (_top_profile.size() > (int) col && !_top_profile[col].isNull())
        return _top_profile[col];

    if (!bottleROIFound()) {
        qWarning() << "Bottle ROI not found, can not execute" << FUNCTION_NAME << "!";
        return QPointF();
    }

    QRect bounding_rect = boundingRect().toRect();

    if (col < (uint)bounding_rect.left() || col > (uint)bounding_rect.right()) {
//        qWarning() << "Column" << col << "out of bounding rect, can not execute" << FUNCTION_NAME << "!";
        return QPointF();
    }

    int offset = 0;
    for (int i=0; i < 10; i++) {
        double dist = cv::pointPolygonTest(bottleHull(), cv::Point2f(col, bounding_rect.top() + offset), true);
        if (dist >= 0.0) {
            break;
        } else {
            offset -= std::floor(dist);
        }
        if (bounding_rect.top() + offset >= bounding_rect.bottom()) {
            qWarning() << "Column" << col << "out of bottle hull, can not execute" << FUNCTION_NAME << "!";
            return QPointF();
        }
    }
    assert(offset >= 0);

    cv::Mat magnitude = this->magnitude().col(col).rowRange(bounding_rect.top() + offset, std::min(bounding_rect.top() + offset + (bounding_rect.height() / 2), bounding_rect.bottom()));

    double t = findEdge(magnitude, cv::EDGE_HORIZONTAL, threshold, precision, cv::EDGE_BLACK_TO_WHITE, cv::EDGE_FIRST);

    if (_top_profile.size() < (int) col + 1)
        _top_profile.resize(col + 1);
    _top_profile[col] = QPointF(col + 0.5, bounding_rect.top() + offset + t);

    return _top_profile[col];
}

const cv::Mat BottleImageInspector::bottleMask()
{
    if (!_mask.empty()) {
        return _mask;
    }

    QPolygonF tops = topProfile();

    START_MEASURE_PERFORMANCE();

    // create bootle mask
    _mask = cv::Mat(_image.size(), CV_8UC1);
    _mask = cv::Scalar(0);

    // get bottle top
    double top = boundingRect().bottom();
    for (int i=0; i < tops.size(); i++) {
        QPointF p = tops[i];
        if (!p.isNull() && p.y() < top) {
            top = p.y();
        }
    }

    // get bottle bottom
    // TODO: maybe the higher point needs to be taken here, so the belt is not taken into account
    Eigen::Vector2d bottom_center = base().intersection(bottleCenter());
    double bottom =  bottom_center(1);

    for (int i = std::ceil(top); i < std::floor(bottom); i++) {
        QPointF l = smoothedLeftProfileInRow(i);
        QPointF r = smoothedRightProfileInRow(i);

        if (!l.isNull() && !r.isNull()) {
            cv::line(_mask, cv::Point(l.x(), l.y()), cv::Point(r.x(), r.y()), cv::Scalar(255));
        }
    }

    if (false) {
        // use distance transform on mask? should not be necessary
        cv::Mat dist_trans;
        cv::distanceTransform(_mask, dist_trans, cv::DIST_L2, 3);

        double max_val = 0.0;
        cv::minMaxLoc(dist_trans, nullptr, &max_val, nullptr, nullptr);

        dist_trans.convertTo(_mask, _mask.type(), 255.0 / max_val);
    }

    STOP_MEASURE_PERFORMANCE();

    return _mask;
}

const cv::Mat BottleImageInspector::dialatedBottleMask()
{
    if (!_dialatedMask.empty()) {
        return _dialatedMask;
    }

    cv::Mat m = bottleMask();

    START_MEASURE_PERFORMANCE();

    // dialate
    cv::dilate(m, _dialatedMask, cv::Mat(), cv::Point(-1,-1), 5);

    // make everything below bottle black
    std::vector<cv::Point> baseArea;
    baseArea.push_back(cv::Point(baseLine().p1().x(), baseLine().p1().y()));
    baseArea.push_back(cv::Point(baseLine().p2().x(), baseLine().p2().y()));
    baseArea.push_back(cv::Point(_image.cols, _image.rows));
    baseArea.push_back(cv::Point(0, _image.rows));
    cv::Mat baseAreaMat = cv::Mat(baseArea);
    cv::fillConvexPoly(_dialatedMask, (const cv::Point*)baseAreaMat.data, baseAreaMat.rows * baseAreaMat.cols * baseAreaMat.channels() / 2, 0);

    STOP_MEASURE_PERFORMANCE();

    return _dialatedMask;
}

const cv::Mat BottleImageInspector::equalizedBottleMask()
{
    if (!_equalizedMask.empty()) {
        return _equalizedMask;
    }

    cv::Mat m = bottleMask();

    START_MEASURE_PERFORMANCE();

    _image.copyTo(_equalizedMask);

    cv::Mat hist;
    int histSize = 256;
    float range[] = { 0, 256 } ;
    const float *histRange = { range };

    cv::calcHist(&_equalizedMask, 1, 0, m, hist, 1, &histSize, &histRange, true, false);

    float cdf = 0;
    float cdf_min = 0;
    float cnt_pix = 0;
    for (int i = 0; i < histSize; i++) {
        cdf += hist.at<float>(0, i);
        hist.at<float>(0, i) = cdf;
        if (cdf_min == 0 && cdf > 0)
            cdf_min = cdf;
        cnt_pix = cdf;
    }

    cdf_min = (cdf_min * 255 / cnt_pix);

    for (int r = 0; r < m.rows; r++) {
        for (int c = 0; c < m.cols; c++) {
            if (m.at<uint8_t>(r, c) > 0) {
                uint8_t v = _equalizedMask.at<uint8_t>(r, c);
                uint8_t x = std::round(((hist.at<float>(0, v) - cdf_min) / (cnt_pix - cdf_min) * 255));
                _equalizedMask.at<uint8_t>(r, c) = x;
            }
        }
    }

    cv::Mat tmp;
    cv::medianBlur(_equalizedMask, tmp, 3);
    _equalizedMask = tmp;

    STOP_MEASURE_PERFORMANCE();

    return _equalizedMask;
}

const cv::Mat BottleImageInspector::contrastBottleMask()
{
    if (!_contrastMask.empty()) {
        return _contrastMask;
    }

    cv::Mat m = bottleMask();

    START_MEASURE_PERFORMANCE();

    _image.copyTo(_contrastMask);

    cv::Mat hist;
    int histSize = 256;
    float range[] = { 0, 256 } ;
    const float *histRange = { range };

    cv::calcHist(&_contrastMask, 1, 0, m, hist, 1, &histSize, &histRange, true, false);

    int cnt = 0;
    for (int i = 0; i < histSize; i++) {
        cnt += hist.at<float>(0, i);
    }

    double min_bin = (double) cnt / 256.0 / 10.0;

    int start_pos = 0;
    int stop_pos = 255;
    for (int i = 0; i < histSize; i++) {
        if (hist.at<float>(0, i) > min_bin) {
            start_pos = i - 1;
            break;
        }
    }
    for (int i = 0; i < histSize; i++) {
        if (hist.at<float>(0, histSize - i) > min_bin) {
            stop_pos = histSize - i + 1;
            break;
        }
    }

    double f = 255.0 / (stop_pos - start_pos);

    for (int r = 0; r < m.rows; r++) {
        for (int c = 0; c < m.cols; c++) {
            if (m.at<uint8_t>(r, c) > 0) {
                uint8_t v = _contrastMask.at<uint8_t>(r, c);
                uint8_t x = 0;
                if (v <= start_pos) {
                    x = 0;
                } else if (v >= stop_pos) {
                    x = 255;
                } else {
                    x = std::round(f * (v - start_pos));
                }

                _contrastMask.at<uint8_t>(r, c) = x;
            }
        }
    }

    cv::Mat tmp;
    cv::medianBlur(_contrastMask, tmp, 3);
    _contrastMask = tmp;

    STOP_MEASURE_PERFORMANCE();

    return _contrastMask;
}

const cv::Mat BottleImageInspector::keypointMask(int edge)
{
    if (!_keypointMask.empty()) {
        return _keypointMask;
    }

    bottleHull();

    START_MEASURE_PERFORMANCE();

    if (edge > 0) {
        // get mask and errode
        cv::Mat inner_mask;
        cv::erode(bottleMask(), inner_mask, cv::Mat(), cv::Point(-1,-1), edge);

        _keypointMask = inner_mask;

    } else {
        /*
         * Results for the part below look ok, but only a way too small area is selected for dark bottles
         */
        // get roi
        QRectF br = boundingRect();
        cv::Rect r = cv::Rect(br.x(), br.y(), br.width(), br.height());

        // get mask and errode
        cv::Mat inner_mask;
        cv::erode(bottleMask()(r), inner_mask, cv::Mat(), cv::Point(-1,-1), 7);

        // create image of bottle with black background
        int background_color = 50;
        cv::Mat bottle_black_background = cv::Mat(inner_mask.size(), _image.type(), cv::Scalar(background_color));
        _image(r).copyTo(bottle_black_background, inner_mask);

        // blur image
        cv::Mat blurred;
        cv::blur(bottle_black_background, blurred, cv::Size(5, 5));

        // flood fill background. this will also flood dark edges of bottle.
        // cv::floodFill(blurred, cv::Point(0, 0), cv::Scalar(0), 0, 255, 0, cv::FLOODFILL_FIXED_RANGE); // somehow FLOODFILL_FIXED_RANGE hangs in OpenCV 3.0
        cv::floodFill(blurred, cv::Point(0, 0), cv::Scalar(0), 0, 255);

        // apply strong filter
        // !!!: this is slow!
        cv::medianBlur(blurred, blurred, 55);

        // flood fill inner part of bottle
        cv::Point center = cv::Point(br.width() / 2, br.height() / 3 * 2);
        cv::floodFill(blurred, center, cv::Scalar(255), 0, 2, 255, 8);
        
        // get mask itself
        cv::Mat blurred_threshold;
        cv::threshold(blurred, blurred_threshold, 170, 255, cv::THRESH_BINARY);
        //    cv::threshold(blurred, blt, 110, 255, cv::THRESH_BINARY);
        //    cv::adaptiveThreshold(image, blt, 255.0, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 99, 25);
        //    cv::adaptiveThreshold(blurred, blt, 255.0, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 99, 45);
        //    cv::adaptiveThreshold(blurred, blt, 255.0, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 99, 5);

        // combine with standard mask
        cv::Mat keypoint_mask;
        cv::bitwise_and(inner_mask, blurred_threshold, keypoint_mask);

        // erode
        cv::erode(keypoint_mask, keypoint_mask, cv::Mat(), cv::Point(-1,-1), 5);

        _keypointMask = cv::Mat(_image.size(), _image.type(), cv::Scalar(0));
        keypoint_mask.copyTo(_keypointMask(r));
    }

    STOP_MEASURE_PERFORMANCE();

    return _keypointMask;
}

const cv::Mat BottleImageInspector::keypointEdges(int sensitivity)
{
    if (!_keypointEdges.empty()) {
        return _keypointEdges;
    }

    cv::Mat kpMask = keypointMask();

    cv::Mat mask;
    cv::erode(bottleMask(), mask, cv::Mat(), cv::Point(-1,-1), 5);

    START_MEASURE_PERFORMANCE();

    // default values for sensitivity == 0
    double low_level = 0.25;
    double high_level = 0.5;

    // set parameters for sensitivity
    if (sensitivity <= -3) {
        low_level = 0.4;
        high_level = 0.8;
    } else if (sensitivity == -2) {
        low_level = 0.35;
        high_level = 0.7;
    } else if (sensitivity == -1) {
        low_level = 0.3;
        high_level = 0.6;
    } else if (sensitivity == 0) {
        // default
    } else if (sensitivity == 1) {
        low_level = 0.2;
        high_level = 0.4;
    } else if (sensitivity == 2) {
        low_level = 0.175;
        high_level = 0.4;
    } else if (sensitivity >= 3) {
        low_level = 0.15;
        high_level = 0.4;
    }

    // get roi
    QRectF br = boundingRect();
    cv::Rect rect = cv::Rect(br.x(), br.y(), br.width(), br.height());

    // create new image with bottle only
    cv::Mat bottle_image = cv::Mat(_image.size(), _image.type(), cv::Scalar(0));
    _image.copyTo(bottle_image, bottleMask());

    // dynamically find edge parameters from histogram
    int maskSize = cv::countNonZero(mask);
    int lowThres = 0;
    int highThres = 0;
    cv::Mat hist;
    int histSize = 256;
    float range[] = { 0, 256 } ;
    const float *histRange = { range };
    cv::calcHist(&bottle_image, 1, 0, mask, hist, 1, &histSize, &histRange, true, false);
    int cnt = 0;
    for (int i = 0; i < histSize; i++) {
        cnt += hist.at<float>(0, i);
        double level = (double)cnt / maskSize;
        if (lowThres == 0 && level >= low_level) {
            lowThres = i;
        } else if (highThres == 0 && level >= high_level) {
            highThres = i;
            break;
        }
    }

    // sanitize threshold
//    highThres = std::min(highThres, 200);
//    lowThres = std::min(lowThres, std::round(highThres / 3.0));

    // extract edges in image
    cv::Mat edges = cv::Mat(bottle_image.size(), bottle_image.type(), cv::Scalar(0));
    cv::Canny(bottle_image(rect), edges(rect), lowThres, highThres, 3, true);

    // get only the edges in the mask
    cv::bitwise_and(kpMask, edges, _keypointEdges);

    STOP_MEASURE_PERFORMANCE();

    return _keypointEdges;
}

const std::vector<cv::KeyPoint> BottleImageInspector::keypoints(int sensitivity)
{
    if (!_keypoints.empty()) {
        return _keypoints;
    }

    // default values for sensitivity == 0
    int kernel = 4;
    int overlap = 2;
    int threshold = 6;

    double thres = 5.0;
    uint min_found = 3;

    // set parameters for sensitivity
    if (sensitivity <= -3) {
        kernel = 4;
        overlap = 2;
        threshold = 8;

        thres = 5.0;
        min_found = 5;
    } else if (sensitivity == -2) {
        kernel = 4;
        overlap = 2;
        threshold = 8;

        thres = 5.0;
        min_found = 4;
    } else if (sensitivity == -1) {
        kernel = 4;
        overlap = 2;
        threshold = 7;

        thres = 5.0;
        min_found = 4;
    } else if (sensitivity == 0) {
        // default
    } else if (sensitivity == 1) {
        kernel = 3;
        overlap = 1;
        threshold = 3;

        thres = 5.0;
        min_found = 8;
    } else if (sensitivity == 2) {
        kernel = 3;
        overlap = 1;
        threshold = 2;

        thres = 5.0;
        min_found = 7;
    } else if (sensitivity >= 3) {
        kernel = 3;
        overlap = 1;
        threshold = 2;

        thres = 5.0;
        min_found = 6;
    }

    cv::Mat kpEdges = keypointEdges(sensitivity);

    START_MEASURE_PERFORMANCE();

    // get roi
    QRectF br = boundingRect();
    cv::Rect rect = cv::Rect(br.x(), br.y(), br.width(), br.height());

    // create keypoints
    std::vector<cv::KeyPoint> keypoints;

    int step = kernel - overlap;
    for (int r=rect.y; r < (rect.y + rect.height - kernel); r += step) {
        for (int c=rect.x; c < (rect.x + rect.width - kernel); c += step) {
            // cv::moments is way too slow for this specific use case
            double m00 = 0;
            double m10 = 0;
            double m01 = 0;
            for (int x=0; x < kernel; x++) {
                for (int y=0; y < kernel; y++) {
                    if (kpEdges.at<uint8_t>(r + y, c + x) > 0) {
                        m00++;
                        m10 += x;
                        m01 += y;
                    }
                }
            }

            if (m00 >= threshold) {
                double off_r = m10 / m00;
                double off_c = m01 / m00;

                cv::KeyPoint k = cv::KeyPoint(c + off_c, r + off_r, kernel, -1, m00);    // orientation/angle could be determined as well, but so far not necessary
                keypoints.push_back(k);
            }
        }
    }

    INTERMEDIATE_MEASURE_PERFORMANCE();

    // sort keypoints (so binary search, lower and upper bound can be used later)
    std::sort(keypoints.begin(), keypoints.end(), cv::KeypointSortByY);

    INTERMEDIATE_MEASURE_PERFORMANCE();

    for (auto p = keypoints.begin(); p != keypoints.end(); ++p) {
        cv::KeyPoint pl = cv::KeyPoint(p->pt.x, p->pt.y - thres, kernel);
        auto lower = std::lower_bound(keypoints.begin(), keypoints.end(), pl, cv::KeypointSortByY);

        uint found = 0;

        // now search for actual close keypoint (below threshold)
        auto q = lower;
        while (q != keypoints.end()) {
            if (q->pt.y > p->pt.y + thres)
                break;

            if (q != p) {
                double d = std::sqrt(std::pow(q->pt.x - p->pt.x, 2) + std::pow(q->pt.y - p->pt.y, 2));
                if (d <= thres) {
                    found++;
                    if (found == min_found)
                        break;
                }
            }
            ++q;
        }

        if (found == min_found) {
            // found a match, so use it
            _keypoints.push_back(*p);
        }
    }

    // add dummy keypoint in case nothing was found
    if (_keypoints.empty()) {
        _keypoints.push_back(cv::KeyPoint());
    }

    INTERMEDIATE_MEASURE_PERFORMANCE();

    STOP_MEASURE_PERFORMANCE();

    return _keypoints;
}
