#include "bottleimageinspectorimagewidget.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "bottleimageinspector.h"
#include "bottleinspectorimagewidgetpolygonitem.h"
#include "bottleinspectorimagewidgetpolygonhandleitem.h"
#include "bottleinspectorimagewidgetrangeitem.h"
#include "bottleinspectorimagewidgetrangehandleitem.h"
#include "colors.h"
#include "imagewidgetareaitem.h"
#include "imagewidgetareahandleitem.h"


BottleImageInspectorImageWidget::BottleImageInspectorImageWidget(QWidget *parent) :
    ImageWidget(parent)
{
    setMinSideSize(25);

    setRubberBandSelectionMode(Qt::ContainsItemShape);
    setDragMode(QGraphicsView::RubberBandDrag);
}

void BottleImageInspectorImageWidget::mousePressEvent(QMouseEvent* event)
{
    _dragStart = event->pos();

    ImageWidget::mousePressEvent(event);

    QList<QGraphicsItem *> items = scene()->items(mapToScene(event->pos()));

    _action = ActionCreate;
    for (QGraphicsItem *item : items) {
        if (dynamic_cast<BottleInspectorImageWidgetRangeHandleItem *>(item)) {
            BottleInspectorImageWidgetRangeHandleItem *handle = qgraphicsitem_cast<BottleInspectorImageWidgetRangeHandleItem *>(item);
            if (handle->handleType() == MoveHandle) {
                _action = ActionMove;
            } else {
                _action = ActionResize;
            }
            break;
        } else if (dynamic_cast<ImageWidgetAreaHandleItem *>(item)) {
            _action = ActionResize;
            break;
        } else if (dynamic_cast<BottleInspectorImageWidgetPolygonHandleItem *>(item)) {
            _action = ActionMove;
            break;
        }
    }
}

void BottleImageInspectorImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
    ImageWidget::mouseReleaseEvent(event);

    if (!result())
        return;

    std::shared_ptr<BottleImageInspector> inspector = result()->inspector();
    if (!inspector) {
        return;
    }

    if (_action == ActionMove || _action == ActionResize) {
        // move or resize

        boost::icl::interval_set<double> profileRanges;
        std::vector<cv::Rect> keypointAreas;

        for (QGraphicsItem *item : scene()->items()) {
            if (dynamic_cast<BottleInspectorImageWidgetRangeItem *>(item)) {
                BottleInspectorImageWidgetRangeItem *rangeItem = qgraphicsitem_cast<BottleInspectorImageWidgetRangeItem *>(item);

                QRectF rect = item->boundingRect();
                double pxBottom = inspector->image().rows - rect.bottom();
                double pxTop = inspector->image().rows - rect.top();

                if (pxTop - pxBottom > minSideSize()) {
                    if (rangeItem->rangeType() == RangeProfile) {
                        profileRanges += boost::icl::interval<double>::closed(pxBottom, pxTop);
                    }
                }
            } else if (dynamic_cast<ImageWidgetAreaItem *>(item)) {
                QRectF rect = item->boundingRect();

                cv::Rect area = cv::Rect(rect.left() - (inspector->image().cols / 2), inspector->image().rows - rect.bottom(), rect.width(), rect.height());
                if (area.area() > minSideSize() * minSideSize()) {
                    keypointAreas.push_back(area);
                }
            } else if (qgraphicsitem_cast<BottleInspectorImageWidgetPolygonItem *>(item)) {

            }
        }

        _profile_ranges = profileRanges;
        _keypoint_areas = keypointAreas;

    } else {
        // create
        double r1 = inspector->image().rows - mapToScene(_dragStart).y();
        double r2 = inspector->image().rows - mapToScene(event->pos()).y();

        if (r1 < 0.0) {
            r1 = 0.0;
        } else if (r1 >= inspector->image().rows) {
            r1 = inspector->image().rows - 1;
        }
        if (r2 < 0.0) {
            r2 = 0.0;
        } else if (r2 >= inspector->image().rows) {
            r2 = inspector->image().rows - 1;
        }

        if (std::abs(r1 - r2) < minSideSize() || std::abs(r1 - r2) > inspector->image().rows) {
            // area to small
            return;
        }

        if (event->pos().x() > _dragStart.x()) {
            // create interval
            boost::icl::continuous_interval<double> interval;
            if (r1 < r2)
                interval = boost::icl::interval<double>::closed(r1, r2);
            else
                interval = boost::icl::interval<double>::closed(r2, r1);

            _profile_ranges += interval;

        } else if (event->pos().x() < _dragStart.x()) {
            // area
            double c1 = mapToScene(_dragStart).x();
            double c2 = mapToScene(event->pos()).x();

            if (c1 < 0.0) {
                c1 = 0.0;
            } else if (c1 >= inspector->image().cols) {
                c1 = inspector->image().cols - 1;
            }
            if (c2 < 0.0) {
                c2 = 0.0;
            } else if (c2 >= inspector->image().cols) {
                c2 = inspector->image().cols - 1;
            }

            c1 -= (inspector->image().cols / 2);
            c2 -= (inspector->image().cols / 2);

            if (std::abs(c1 - c2) < minSideSize() || std::abs(c1 - c2) > inspector->image().cols) {
                // area to small
                return;
            }

            cv::Rect area = cv::Rect(std::min(c1, c2), std::min(r1, r2), std::abs(c1 - c2), std::abs(r1 - r2));
            _keypoint_areas.push_back(area);

        }
    }

    emit parameterUpdated();
}

void BottleImageInspectorImageWidget::displayProcessingResult(std::shared_ptr<const ProcessingResult> result)
{
    ImageWidget::displayProcessingResult(result);
    if (!result)
        return;

    std::shared_ptr<const BottleImageProcessingResult> derived = std::dynamic_pointer_cast<const BottleImageProcessingResult>(result);
    if (!derived)
        return;

    std::shared_ptr<BottleImageInspector> inspector = derived->inspector();
    if (!inspector)
        return;

    QRectF bottleRect = inspector->boundingRect();
    QRectF combinedRect = result->roi().united(bottleRect);

    boost::icl::interval_set<double> profileRanges = _profile_ranges;
    for (boost::icl::interval_set<double>::iterator it = profileRanges.begin(); it != profileRanges.end(); ++it) {
        boost::icl::continuous_interval<double> is = *it;
        BottleInspectorImageWidgetRangeItem *rect = new BottleInspectorImageWidgetRangeItem(RangeProfile, bottleRect, bottleRect.left() - 10, inspector->image().rows - is.upper(), bottleRect.width() + 10 + 10 + 25, (inspector->image().rows - is.lower()) - (inspector->image().rows - is.upper()));

        QPen pen(HMIColor::red);
        pen.setWidth(0);
        rect->setPen(pen);

        _scene->addItem(rect);

        combinedRect = combinedRect.united(rect->boundingRect());
    }

    std::vector<cv::Rect> keypointAreas = _keypoint_areas;
    for (auto it = keypointAreas.begin(); it != keypointAreas.end(); ++it) {
        cv::Rect r = *it;
        ImageWidgetAreaItem *rect = new ImageWidgetAreaItem(QRectF(0, 0, inspector->image().cols, inspector->image().rows), (inspector->image().cols / 2) + r.x, inspector->image().rows - r.y - r.height, r.width, r.height);

        QPen pen(HMIColor::green);
        pen.setWidth(0);
        rect->setPen(pen);

        _scene->addItem(rect);

        combinedRect = combinedRect.united(rect->boundingRect());
    }

    // update roi to make sure all handles etc are shown
    fitInView(combinedRect, Qt::KeepAspectRatio);
}
