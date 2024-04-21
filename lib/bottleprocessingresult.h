#ifndef BOTTLEPROCESSINGRESULT_H
#define BOTTLEPROCESSINGRESULT_H

#include "bottleimageprocessingresult.h"

#include "bottlefeatureset.h"
#include "bottleinspector.h"


class BottleProcessingResult : public BottleImageProcessingResult
{
public:
    BottleProcessingResult(const int cameraId, const cv::Mat &image);
    virtual ~BottleProcessingResult();

    std::shared_ptr<BottleInspector> inspector() const { return std::dynamic_pointer_cast<BottleInspector>(_inspector); }

    std::shared_ptr<const BottleFeatureSet> featureSet() const { return std::dynamic_pointer_cast<const BottleFeatureSet>(_featureSet); }
};

#endif // BOTTLEPROCESSINGRESULT_H
