#include "bottleinspectorimagewidget.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "colors.h"
#include "bottleinspector.h"
#include "bottleinspectorimagewidgetpolygonitem.h"
#include "bottleinspectorimagewidgetpolygonhandleitem.h"
#include "bottleinspectorimagewidgetrangeitem.h"
#include "bottleinspectorimagewidgetrangehandleitem.h"


BottleInspectorImageWidget::BottleInspectorImageWidget(QWidget *parent) :
    ImageWidget(parent)
{
    setRubberBandSelectionMode(Qt::ContainsItemShape);
    setDragMode(QGraphicsView::RubberBandDrag);
}

void BottleInspectorImageWidget::mousePressEvent(QMouseEvent* event)
{
    _dragStart = event->pos();

    ImageWidget::mousePressEvent(event);

    QList<QGraphicsItem *> items = scene()->items(mapToScene(event->pos()));

    _action = ActionCreate;
    for (QGraphicsItem *item : items) {
        if (dynamic_cast<BottleInspectorImageWidgetRangeHandleItem *>(item)) {
            BottleInspectorImageWidgetRangeHandleItem *handle = dynamic_cast<BottleInspectorImageWidgetRangeHandleItem *>(item);
            if (handle->handleType() == MoveHandle) {
                _action = ActionMove;
            } else {
                _action = ActionResize;
            }
            break;
        } else if (dynamic_cast<BottleInspectorImageWidgetPolygonHandleItem *>(item)) {
            _action = ActionMove;
            break;
        }
    }
}

void BottleInspectorImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
#define MIN_AREA_HEIGHT     5.0

    ImageWidget::mouseReleaseEvent(event);

    if (!result())
        return;

    std::shared_ptr<BottleInspector> inspector = result()->inspector();
    if (!inspector) {
        return;
    }

    if (_action == ActionMove || _action == ActionResize) {
        // move or resize

        boost::icl::interval_set<double> profileRanges;
        boost::icl::interval_set<double> keypointRanges;
        QList<QPolygonF> keypointAreas;

        for (QGraphicsItem *item : scene()->items()) {
            if (dynamic_cast<BottleInspectorImageWidgetRangeItem *>(item)) {
                BottleInspectorImageWidgetRangeItem *rangeItem = dynamic_cast<BottleInspectorImageWidgetRangeItem *>(item);

                qDebug() << "item" << item << item->pos() << item->boundingRect() << item->boundingRect().bottomLeft() << item->boundingRect().bottomRight() << mapToScene(item->boundingRect().bottomLeft().toPoint()) << mapToScene(item->boundingRect().bottomRight().toPoint());

                QRectF rect = item->boundingRect();

                // create ParametrizedLine
                Eigen::ParametrizedLine<double, 2> bottomLine = Eigen::ParametrizedLine<double, 2>::Through(Eigen::Vector2d(rect.bottomLeft().x(), rect.bottomLeft().y() + item->pos().y()), Eigen::Vector2d(rect.bottomRight().x(), rect.bottomRight().y() + item->pos().y()));
                Eigen::Vector2d centerBottom = inspector->bottleCenter().intersection(Eigen::Line(bottomLine));
                Eigen::Vector3d worldCenterBottom = inspector->pixelToWorld(centerBottom);

                Eigen::ParametrizedLine<double, 2> topLine = Eigen::ParametrizedLine<double, 2>::Through(Eigen::Vector2d(rect.topLeft().x(), rect.topLeft().y() + item->pos().y()), Eigen::Vector2d(rect.topRight().x(), rect.topRight().y() + item->pos().y()));
                Eigen::Vector2d centerTop = inspector->bottleCenter().intersection(Eigen::Line(topLine));
                Eigen::Vector3d worldCenterTop = inspector->pixelToWorld(centerTop);

                double pxBottom = worldCenterBottom(1);
                double pxTop = worldCenterTop(1);

                if (pxTop - pxBottom > MIN_AREA_HEIGHT) {
                    if (rangeItem->rangeType() == RangeProfile) {
                        profileRanges += boost::icl::interval<double>::closed(pxBottom, pxTop);
                    } else if (rangeItem->rangeType() == RangeKeypoints) {
                        keypointRanges += boost::icl::interval<double>::closed(pxBottom, pxTop);
                    }
                }
            } else if (dynamic_cast<BottleInspectorImageWidgetPolygonItem *>(item)) {
                //BottleInspectorImageWidgetPolygonItem *polygonItem = dynamic_cast<BottleInspectorImageWidgetPolygonItem *>(item);

                break;
            }
        }

        _profile_ranges = profileRanges;
        _keypoint_ranges = keypointRanges;
        _keypoint_areas = keypointAreas;

    } else {
        // create

        Eigen::Vector2d base = inspector->worldToPixel(0, 0, 0);

        double p1 = inspector->pixelToWorld(base(0), mapToScene(_dragStart).y())(1);
        double p2 = inspector->pixelToWorld(base(0), mapToScene(event->pos()).y())(1);

        if (p1 < 0.0) {
            p1 = 0.0;
        } else if (p1 > inspector->bottleHeight()) {
            p1 = inspector->bottleHeight();
        }
        if (p2 < 0.0) {
            p2 = 0.0;
        } else if (p2 > inspector->bottleHeight()) {
            p2 = inspector->bottleHeight();
        }

        if (std::abs(p1 - p2) < MIN_AREA_HEIGHT) {
            // area to small
            return;
        }

        // create interval
        boost::icl::continuous_interval<double> interval;
        if (p1 < p2)
            interval = boost::icl::interval<double>::closed(p1, p2);
        else
            interval = boost::icl::interval<double>::closed(p2, p1);

        if (event->pos().x() > _dragStart.x()) {
            _profile_ranges += interval;
        } else if (event->pos().x() < _dragStart.x()) {
            _keypoint_ranges += interval;
        }
    }

    emit parameterUpdated();
}

void BottleInspectorImageWidget::displayProcessingResult(std::shared_ptr<const ProcessingResult> result)
{
    ImageWidget::displayProcessingResult(result);

    if (!result)
        return;

    std::shared_ptr<const BottleProcessingResult> derived = std::dynamic_pointer_cast<const BottleProcessingResult>(result);

    std::shared_ptr<BottleInspector> inspector = derived->inspector();
    if (!inspector) {
        return;
    }

    Eigen::Vector2d bottom = inspector->worldToPixel(Eigen::Vector3d(0, 0, 0));
    Eigen::Vector2d top = inspector->worldToPixel(Eigen::Vector3d(0, inspector->bottleHeight(), 0));
    QRectF bottleRect = QRectF(0, top(1), 0, bottom(1) - top(1));
    QRectF combinedRect = result->roi();

    boost::icl::interval_set<double> profileRanges = _profile_ranges;
    for (boost::icl::interval_set<double>::iterator it = profileRanges.begin() ; it != profileRanges.end(); ++it) {
        boost::icl::continuous_interval<double> is = *it;

        Eigen::Vector2d lower = inspector->worldToPixel(Eigen::Vector3d(0, is.lower(), 0));
        Eigen::Vector2d upper = inspector->worldToPixel(Eigen::Vector3d(0, is.upper(), 0));

        BottleInspectorImageWidgetRangeItem *rect = new BottleInspectorImageWidgetRangeItem(RangeProfile, bottleRect, inspector->boundingRect().left() - 10 - 25, upper(1), inspector->boundingRect().width() + 10 + 10 + 25, lower(1) - upper(1));

        QPen pen(HMIColor::red);
        pen.setWidth(0);
        rect->setPen(pen);

        _scene->addItem(rect);

        combinedRect = combinedRect.united(rect->boundingRect());
    }

    boost::icl::interval_set<double> keypointRanges = _keypoint_ranges;
    for (boost::icl::interval_set<double>::iterator it = keypointRanges.begin() ; it != keypointRanges.end(); ++it) {
        boost::icl::continuous_interval<double> is = *it;

        Eigen::Vector2d lower = inspector->worldToPixel(Eigen::Vector3d(0, is.lower(), 0));
        Eigen::Vector2d upper = inspector->worldToPixel(Eigen::Vector3d(0, is.upper(), 0));

        BottleInspectorImageWidgetRangeItem *rect = new BottleInspectorImageWidgetRangeItem(RangeKeypoints, bottleRect, inspector->boundingRect().left() - 10, upper(1), inspector->boundingRect().width() + 10 + 10 + 25, lower(1) - upper(1));

        QPen pen(HMIColor::green);
        pen.setWidth(0);
        rect->setPen(pen);

        _scene->addItem(rect);

        combinedRect = combinedRect.united(rect->boundingRect());
    }

    for (const QPolygonF &area : _keypoint_areas) {

        // calculate adjusted area

        QPolygonF a;
        for (const QPointF &f : area) {
            Eigen::Vector2d p = inspector->worldToPixel(Eigen::Vector3d(f.x(), f.y(), 0));
            a << QPointF(p(0), p(1));
        }

        qDebug() << area;
        qDebug() << a;

        BottleInspectorImageWidgetPolygonItem *item = new BottleInspectorImageWidgetPolygonItem(a);

        QPen pen(HMIColor::green);
        pen.setWidth(0);
        item->setPen(pen);

        _scene->addItem(item);

        combinedRect = combinedRect.united(item->boundingRect());
    }

    // update roi to make sure all handles etc are shown
    combinedRect.setLeft(combinedRect.left() - 25);
    combinedRect.setWidth(combinedRect.width() + 50);
    fitInView(combinedRect, Qt::KeepAspectRatio);
}
