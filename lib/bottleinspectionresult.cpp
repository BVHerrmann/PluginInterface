#include "bottleinspectionresult.h"

BottleInspectionResult::BottleInspectionResult(bool hasCameraLeft, bool hasCameraCenter, bool hasCameraRight) :
    InspectionResult()
{
    _has_camera_left = hasCameraLeft;
    _has_camera_center = hasCameraCenter;
    _has_camera_right = hasCameraRight;
}

BottleInspectionResult::~BottleInspectionResult()
{

}
