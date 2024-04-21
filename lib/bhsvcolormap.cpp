#include "bhsvcolormap.h"

#include <QtCore>


BHSVColorMap::BHSVColorMap()
{

}

BHSVColorMap::~BHSVColorMap()
{

}

QRgb BHSVColorMap::rgb(const QwtInterval &interval, double value) const
{
    const double width = interval.width();
    if (!qIsNaN(value) && width >= 0.0) {
        double adjustedValue = value;
        if (interval.minValue() < 0.0) {
            adjustedValue += -interval.minValue();
        } else if (interval.minValue() > 0.0) {
            adjustedValue -= interval.minValue();
        }

        return QColor::fromHsvF(std::fmod(adjustedValue, width) / width, 1.0, 0.9).rgb();
    }

    return QColor(0, 0, 0).rgb();
}

unsigned char BHSVColorMap::colorIndex(const QwtInterval &interval, double value) const
{
    (void)interval;
    (void)value;

    return 0;
}
