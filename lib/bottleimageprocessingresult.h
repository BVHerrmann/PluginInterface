#ifndef BOTTLEIMAGEPROCESSINGRESULT_H
#define BOTTLEIMAGEPROCESSINGRESULT_H

#include "processingresult.h"

#include "bottleimageinspector.h"


class BottleImageProcessingResult : public ProcessingResult
{
public:
    BottleImageProcessingResult(const int cameraId, const cv::Mat &image);
    virtual ~BottleImageProcessingResult();

    std::shared_ptr<BottleImageInspector> inspector() const { return std::dynamic_pointer_cast<BottleImageInspector>(_inspector); }
};

#endif // BOTTLEIMAGEPROCESSINGRESULT_H
