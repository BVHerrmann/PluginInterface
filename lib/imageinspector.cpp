#include "imageinspector.h"

ImageInspector::ImageInspector(const cv::Mat &image)
{
    // store image
    _image = image;
}

ImageInspector::~ImageInspector()
{

}
