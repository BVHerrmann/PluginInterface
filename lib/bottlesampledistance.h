#ifndef BOTTLESAMPLEDISTANCE_H
#define BOTTLESAMPLEDISTANCE_H

#include <memory>

class BottleInspectionSample;


struct BottleSampleDistance {
    std::shared_ptr<const BottleInspectionSample> reference_sample;
    double combined;
    double height;
    double profile;
    double keypoints;

    // overload some operators to allow sorting
    bool operator<(const BottleSampleDistance &other) const {
        return (combined < other.combined);
    }
    bool operator<=(const BottleSampleDistance &other) const {
        return (combined <= other.combined);
    }
    bool operator>(const BottleSampleDistance &other) const {
        return (combined > other.combined);
    }
    bool operator>=(const BottleSampleDistance &other) const {
        return (combined >= other.combined);
    }
};

#endif // BOTTLESAMPLEDISTANCE_H
