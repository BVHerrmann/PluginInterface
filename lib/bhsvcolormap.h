#ifndef BHSVCOLORMAP_H
#define BHSVCOLORMAP_H

#include <qwt/qwt_color_map.h>

class BHSVColorMap : public QwtColorMap
{
public:
    BHSVColorMap();
    virtual ~BHSVColorMap();

    QRgb rgb(const QwtInterval &interval, double value) const override;
    unsigned char colorIndex(const QwtInterval &interval, double value) const override;
};

#endif // BHSVCOLORMAP_H
