#ifndef IMAGEINSPECTOR_H
#define IMAGEINSPECTOR_H

#include "processingresult.h"

#include <opencv2/core.hpp>
#include "opencv_support.h"


class ImageInspector
{

public:
    explicit ImageInspector(const cv::Mat &image);
    virtual ~ImageInspector();

    // images
    const cv::Mat image() { return _image; }
    inline const cv::Mat magnitude() { if(!_magnitude.data) { cv::magnitude(_image, _magnitude, _sobel_x, _sobel_y, _smoothed); } return _magnitude; }
    inline const cv::Mat sobel_x() { if(!_sobel_x.data) { cv::magnitude(_image, _magnitude, _sobel_x, _sobel_y, _smoothed); } return _sobel_x; }
    inline const cv::Mat sobel_y() { if(!_sobel_y.data) { cv::magnitude(_image, _magnitude, _sobel_x, _sobel_y, _smoothed); } return _sobel_y; }
    inline const cv::Mat smoothed() { if(!_smoothed.data) { cv::magnitude(_image, _magnitude, _sobel_x, _sobel_y, _smoothed); } return _smoothed; }

protected:
    // images
    cv::Mat _image;

private:
    // usefull helper images
    cv::Mat _magnitude, _sobel_x, _sobel_y, _smoothed;
};

#endif // IMAGEINSPECTOR_H
