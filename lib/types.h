#ifndef TYPES_H
#define TYPES_H

#include <QImage>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


/** \brief Convert a QImage to cv::Mat
 *
 * If copy is false, then no data will be copied.  However, due to
 * different memory sharing techniques you must keep the original
 * QImage around as long as you use the cv::Mat, otherwise the
 * data underneath might be freed and becomes invalid.
 *
 * \param im            Image to convert
 * \param copy          Copy the data or not
 */
inline cv::Mat QImageToMat(const QImage &im, bool copy = false)
{
    if (im.isNull())
        return cv::Mat();

    if (im.format() == QImage::Format_Indexed8) {
        cv::Mat cim(im.height(), im.width(), CV_8U,
                    (void*)im.bits(),
                    im.bytesPerLine());

        return copy ? cim.clone() : cim;
    }
    else if (im.format() ==  QImage::Format_RGB888) {
        cv::Mat cim(im.height(), im.width(), CV_8UC3,
                    (void*)im.bits(),
                    im.bytesPerLine());

        return copy ? cim.clone() : cim;
    } else {
        assert (false);
    }

    return cv::Mat();
}

/** \brief Convert a cv::Mat to QImage
 *
 * If copy is false, then no data will be copied.  However, due to
 * different memory sharing techniques you must keep the original
 * cv::Mat around as long as you use the QImage, otherwise the
 * data underneath might be freed and becomes invalid.
 *
 * \param im            Image to convert
 * \param copy          Copy the data or not
 */
inline QImage MatToQImage(const cv::Mat &im, bool copy = false)
{
    if (im.empty())
        return QImage();

    cv::Mat scaled;
    if (im.depth() == CV_8U) {
        scaled = im;
    }
    else {
        // find range of values
        double min = 0.0;
        double max = 0.0;
        cv::minMaxLoc(im, &min, &max);

        // scale image correctly
        double alpha = max > std::abs(min) ? 255.0 / max : 255.0 / std::abs(min);
        cv::convertScaleAbs(im, scaled, alpha);
    }

    // convert
    if (scaled.channels() == 1) {
		QImage qim(scaled.data, scaled.cols, scaled.rows, (int)scaled.step, QImage::Format_Grayscale8);
        return copy ? qim.copy() : qim;
    }
    else if (scaled.channels() == 3) {
        QImage qim(scaled.data, scaled.cols, scaled.rows, (int)scaled.step, QImage::Format_RGB888);
        return copy ? qim.copy() : qim;
    }

    return QImage();
}

inline cv::Mat loadImage(QString fileName)
{
    cv::Mat image = cv::imread(qPrintable(fileName), cv::IMREAD_GRAYSCALE);
    if (image.data == nullptr) {
        qDebug() << "using QImage";
        QImage qimage(fileName);
        if (qimage.isNull()) {
            qDebug() << "Image " << fileName << "Could not be opened!";
            return image;
        }
        qimage = qimage.convertToFormat(QImage::Format_RGB888);
        cv::Mat timage = QImageToMat(qimage);
        cv::cvtColor(timage, image, cv::COLOR_RGB2GRAY);
    }

    assert(image.data);

    return image;
}

#endif // TYPES_H
