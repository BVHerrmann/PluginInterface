#include "bottleprocessingresult.h"

BottleProcessingResult::BottleProcessingResult(const int cameraId, const cv::Mat &image) :
    BottleImageProcessingResult(cameraId, image)
{

}

BottleProcessingResult::~BottleProcessingResult()
{

}
