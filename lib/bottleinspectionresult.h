#ifndef BOTTLEINSPECTIONRESULT_H
#define BOTTLEINSPECTIONRESULT_H

#include "inspectionresult.h"

#include "bottleprocessingresult.h"


class BottleInspectionResult : public InspectionResult
{
public:
    BottleInspectionResult(bool hasCameraLeft, bool hasCameraCenter, bool hasCameraRight);
    virtual ~BottleInspectionResult();

    inline bool hasCameraLeft() const { return _has_camera_left; }
    inline bool hasCameraCenter() const { return _has_camera_center; }
    inline bool hasCameraRight() const { return _has_camera_right; }

    const std::shared_ptr<const BottleProcessingResult> processingResult(const int key) const { return std::dynamic_pointer_cast<const BottleProcessingResult>(InspectionResult::processingResult(key)); }

protected:
    bool _has_camera_left;
    bool _has_camera_center;
    bool _has_camera_right;
};

#endif // BOTTLEINSPECTIONRESULT_H
