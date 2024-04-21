#ifndef PROCESSINGRESULT_H
#define PROCESSINGRESULT_H

#include <map>
#include <memory>

#include <QObject>
#include <QMetaType>
#include <QImage>

class BGraphicsItem;
#include "featureset.h"
class ImageInspector;

namespace cv {
    class Mat;
}

Q_DECLARE_METATYPE(std::vector<double>)


class ProcessingResult
{
public:
    explicit ProcessingResult(const int cameraId, const cv::Mat &image);
    virtual ~ProcessingResult();

    int cameraId() const { return _camera_id; }

    // inspector
    void setInspector(std::shared_ptr<ImageInspector> inspector) { _inspector = inspector; }
    std::shared_ptr<ImageInspector> inspector() const { return _inspector; }

    // images
    const cv::Mat getMat(QString key = "original") const;
    const QImage getImage(QString key) const;
    const QImage originalImage() const;
    QList<QString> imageKeys() const;
    void addImage(QString key, const cv::Mat &image);

    // graphics items
    std::vector<std::shared_ptr<BGraphicsItem> > graphicsItems() const { return _graphics_items; }
    void addGraphicsItem(std::shared_ptr<BGraphicsItem> value);

    // roi
    QRectF roi() const { return _roi.isValid() ? _roi : QRectF(QPoint(0, 0), originalImage().size()); }
    void setRoi(QRectF roi) { _roi = roi; }

    // feature set
    void setFeatureSet(std::shared_ptr<const FeatureSet> set) { _featureSet = set; }
    std::shared_ptr<const FeatureSet> featureSet() const { return _featureSet; }

protected:
    int _camera_id;
    std::shared_ptr<ImageInspector> _inspector;

    std::map<QString, cv::Mat> _images;
    std::vector<std::shared_ptr<BGraphicsItem> > _graphics_items;
    QRectF _roi;

    std::shared_ptr<const FeatureSet> _featureSet;
};

#endif // PROCESSINGRESULT_H
