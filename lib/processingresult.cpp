#include "processingresult.h"

#include <QtCore>

#include <opencv2/core.hpp>

#include "types.h"
#include "bgraphicsitem.h"


ProcessingResult::ProcessingResult(const int cameraId, const cv::Mat &image)
{
    _camera_id = cameraId;

    _images["original"] = image;
}

ProcessingResult::~ProcessingResult()
{
    _images.clear();
}

const cv::Mat ProcessingResult::getMat(QString key) const
{
    if (_images.count(key))
        return _images.at(key);
    else
        return cv::Mat();
}

const QImage ProcessingResult::getImage(QString key) const
{
    if (_images.count(key))
        return MatToQImage(_images.at(key), true);
    else
        return QImage();
}

const QImage ProcessingResult::originalImage() const
{
    return getImage("original");
}

QList<QString> ProcessingResult::imageKeys() const
{
    QList<QString> keys;
    for (std::map<QString, cv::Mat>::const_iterator it = _images.begin(); it != _images.end(); ++it) {
        keys << it->first;
    }

    return keys;
}

void ProcessingResult::addImage(QString key, const cv::Mat &image)
{
    _images[key] = image;
}

void ProcessingResult::addGraphicsItem(std::shared_ptr<BGraphicsItem> value)
{
    _graphics_items.push_back(value);
}

