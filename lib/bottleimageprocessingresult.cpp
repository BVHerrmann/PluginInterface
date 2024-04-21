#include "bottleimageprocessingresult.h"

BottleImageProcessingResult::BottleImageProcessingResult(const int cameraId, const cv::Mat &image) :
    ProcessingResult(cameraId, image)
{

}

BottleImageProcessingResult::~BottleImageProcessingResult()
{

}
