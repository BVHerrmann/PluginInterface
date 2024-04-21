#ifndef BGRAPHICSKEYPOINTITEM_H
#define BGRAPHICSKEYPOINTITEM_H

#include "babstractgraphicsshapeitem.h"

#include <QLineF>

#include <opencv2/features2d.hpp>


class BGraphicsKeyPointItem : public BAbstractGraphicsShapeItem
{
public:
    explicit BGraphicsKeyPointItem();
    
    explicit BGraphicsKeyPointItem(const cv::KeyPoint &keypoint) : BAbstractGraphicsShapeItem() {
        setKeyPoint(keypoint);
    }

    cv::KeyPoint keyPoint() const { return _keypoint; }
    void setKeyPoint(const cv::KeyPoint &keypoint) { _keypoint = keypoint; }

    QGraphicsItem *toGraphicsItem() const override;

protected:
    cv::KeyPoint _keypoint;
};

#endif // BGRAPHICSKEYPOINTITEM_H
