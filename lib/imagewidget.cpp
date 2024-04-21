#include "imagewidget.h"

#include <QtSvg>
#include <QtGui>
#include <QtWidgets>
#include <QtPrintSupport>

#include <opencv2/highgui.hpp>

#include "common.h"
#include "bgraphicsitem.h"


ImageWidget::ImageWidget(QWidget *parent) :
        QGraphicsView(parent)
{
    _scene = new QGraphicsScene(this);
    _pixmap = _scene->addPixmap(QPixmap());
    setScene(_scene);

    setupUi();

	grabGesture(Qt::PinchGesture);
}

ImageWidget::~ImageWidget()
{

}

void ImageWidget::setupUi()
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
}

void ImageWidget::updateToolTip()
{
//    QMutexLocker locker(mutex);
//    (void)locker;

    if (_result) {
        QImage image = _result->getImage(_current_image_key);

        if (!image.isNull()) {
            QPoint pos = mapToScene(mapFromGlobal(QCursor::pos())).toPoint();

            if (_pixmap->contains(pos)) {
                QRgb value = image.pixel(pos);
                setToolTip("(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + "): " + QString::number(qGray(value)));
            } else {
                setToolTip("");
            }
        } else {
            setToolTip("");
        }
    }
}

void ImageWidget::setCenter(const QPointF& centerPoint)
{
    //Get the rectangle of the visible area in scene coords
    QRectF visible_area = mapToScene(rect()).boundingRect();

    //Get the scene area
    QRectF scene_bounds = sceneRect();

    double boundX = visible_area.width() / 2.0;
    double boundY = visible_area.height() / 2.0;
    double boundWidth = scene_bounds.width() - 2.0 * boundX;
    double boundHeight = scene_bounds.height() - 2.0 * boundY;

    //The max boundary that the centerPoint can be to
    QRectF bounds(boundX, boundY, boundWidth, boundHeight);

    if (bounds.contains(centerPoint)) {
        //We are within the bounds
        _current_center_point = centerPoint;
    } else {
        //We need to clamp or use the center of the screen
        if(visible_area.contains(scene_bounds)) {
            //Use the center of scene ie. we can see the whole scene
            _current_center_point = scene_bounds.center();
        } else {

            _current_center_point = centerPoint;

            //We need to clamp the center. The centerPoint is too large
            if(centerPoint.x() > bounds.x() + bounds.width()) {
                _current_center_point.setX(bounds.x() + bounds.width());
            } else if(centerPoint.x() < bounds.x()) {
                _current_center_point.setX(bounds.x());
            }

            if(centerPoint.y() > bounds.y() + bounds.height()) {
                _current_center_point.setY(bounds.y() + bounds.height());
            } else if(centerPoint.y() < bounds.y()) {
                _current_center_point.setY(bounds.y());
            }

        }
    }

    //Update the scrollbars
    centerOn(_current_center_point);
}

void ImageWidget::zoomActualSize()
{
    resetTransform();
}

void ImageWidget::zoomToFit()
{
    fitInView(_result->roi(), Qt::KeepAspectRatio);
    _current_center_point = _result->roi().center();
}

void ImageWidget::zoomIn()
{
    scale(1.5, 1.5);
}

void ImageWidget::zoomOut()
{
    scale(0.5, 0.5);
}

bool ImageWidget::event(QEvent *event)
{
	if (event->type() == QEvent::Gesture)
		return gestureEvent(static_cast<QGestureEvent*>(event));

	return QGraphicsView::event(event);
}

bool ImageWidget::gestureEvent(QGestureEvent *event)
{
	if (QGesture *pan = event->gesture(Qt::PanGesture))
		panTriggered(static_cast<QPanGesture *>(pan));
	if (QGesture *pinch = event->gesture(Qt::PinchGesture))
		pinchTriggered(static_cast<QPinchGesture *>(pinch));
	
	return true;
}

void ImageWidget::panTriggered(QPanGesture *gesture)
{
#ifndef QT_NO_CURSOR
	switch (gesture->state()) {
	case Qt::GestureStarted:
	case Qt::GestureUpdated:
		setCursor(Qt::SizeAllCursor);
		break;
	default:
		setCursor(Qt::ArrowCursor);
	}
#endif
	
	qDebug() << "pan" << gesture->delta();

	setCenter(_current_center_point + gesture->delta());
}

void ImageWidget::pinchTriggered(QPinchGesture *gesture)
{
	//Get the position of the mouse before scaling, in scene coords
	QPointF pointBeforeScale(mapToScene(gesture->centerPoint().toPoint()));

	//Get the original screen centerpoint
	QPointF screenCenter = _current_center_point;

	switch (gesture->state()) {
	case Qt::GestureStarted:
		_pinchStartTransform = transform();
	case Qt::GestureUpdated:
	case Qt::GestureFinished:
	{
		// scale
		scale(_pinchStartTransform.m11() * gesture->totalScaleFactor() / transform().m11(), _pinchStartTransform.m22() * gesture->totalScaleFactor() / transform().m22());

		//Get the position after scaling, in scene coords
		QPointF pointAfterScale(mapToScene(gesture->centerPoint().toPoint()));

		//Get the offset of how the screen moved
		QPointF offset = pointBeforeScale - pointAfterScale;
		
		//Adjust to the new center for correct zooming
		QPointF newCenter = screenCenter + offset;
		setCenter(newCenter);

		break;
	}
	case Qt::GestureCanceled:
		setTransform(_pinchStartTransform);
		break;
    case Qt::NoGesture:
        break;
	}
}

void ImageWidget::resizeEvent(QResizeEvent *event)
{
//    QMutexLocker locker(mutex);
//    (void)locker;

    if (_result) {
        // fit image if necessary
        if (_previous_viewport_rect.isNull() ||
                _previous_viewport_rect.contains(_result->roi())) {
            fitInView(_result->roi(), Qt::KeepAspectRatio);
        }

        // save the new rect
        if (!_pixmap->pixmap().isNull()) {
            _previous_viewport_rect = _result->roi();
        }
    }

    QGraphicsView::resizeEvent(event);
}

void ImageWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(parentWidget());

    if (_result) {
        // choices for images available
        QList<QString> keys = _result->imageKeys();
        for (const QString &key : keys) {
            QAction *action = new QAction(key, &menu);
            action->setObjectName(action->text());
            if (_current_image_key == action->objectName()) {
                action->setCheckable(true);
                action->setChecked(true);
            }
            connect(action, &QAction::triggered, this, &ImageWidget::changeImage);

            menu.addAction(action);
        }

        // more actions
        menu.addSeparator();
        menu.addAction(tr("Actual Size"), this, &ImageWidget::zoomActualSize);
        menu.addAction(tr("Zoom to Fit"), this, &ImageWidget::zoomToFit);
        menu.addAction(tr("Zoom In"), this, &ImageWidget::zoomIn);
        menu.addAction(tr("Zoom Out"), this, &ImageWidget::zoomOut);

        menu.addSeparator();
        menu.addAction(tr("Save Image..."), this, &ImageWidget::saveCurrentImage);
        menu.addAction(tr("Save Scene..."), this, &ImageWidget::saveCurrentScene);

        // display menu
        menu.exec(event->globalPos());
    }

    QGraphicsView::contextMenuEvent(event);
}

void ImageWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;
    /*
    //For panning the view
    _last_pan_point = event->pos();
    setCursor(Qt::ClosedHandCursor);
    */
    QGraphicsView::mousePressEvent(event);
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
    /*
    setCursor(Qt::OpenHandCursor);
    _last_pan_point = QPoint();
    */
    QGraphicsView::mouseReleaseEvent(event);
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
//    QMutexLocker locker(mutex);
//    (void)locker;
    /*
    if (!_last_pan_point.isNull()) {
        //Get how much we panned
        QPointF delta = mapToScene(_last_pan_point) - mapToScene(event->pos());
        _last_pan_point = event->pos();

        //Update the center ie. do the pan
        setCenter(_current_center_point + delta);
    }
    */

    updateToolTip();

    QGraphicsView::mouseMoveEvent(event);
}

void ImageWidget::wheelEvent(QWheelEvent* event)
{
    if (event->source() == Qt::MouseEventSynthesizedBySystem) {
        QAbstractScrollArea::wheelEvent(event);
    } else {
        QGraphicsView::wheelEvent(event);
    }
    
    /*
//    QMutexLocker locker(mutex);
//    (void)locker;

    //Get the position of the mouse before scaling, in scene coords
    QPointF pointBeforeScale(mapToScene(event->pos()));

    //Get the original screen centerpoint
    QPointF screenCenter = _current_center_point;

    //Scale the view ie. do the zoom
    double scaleFactor = 1.05; //How fast we zoom
    if (event->delta() > 0) {
        //Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        //Zooming out
        if (matrix().mapRect(QRectF(0, 0, 1, 1)).width() < 1.0) {
            fitInView(_pixmap, Qt::KeepAspectRatio);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }

    //Get the position after scaling, in scene coords
    QPointF pointAfterScale(mapToScene(event->pos()));

    //Get the offset of how the screen moved
    QPointF offset = pointBeforeScale - pointAfterScale;

    //Adjust to the new center for correct zooming
    QPointF newCenter = screenCenter + offset;
    setCenter(newCenter);
     */
}

void ImageWidget::displayProcessingResult(std::shared_ptr<const ProcessingResult> result)
{
    // TODO: clear image and display error if no result
    if (!result)
        return;

#ifdef DEBUG
    if (QApplication::instance()->thread() != QThread::currentThread()) {
        qWarning() << "ImageWidget: Current thread is not main thread!!!";
        return;
    }
#endif

//    QMutexLocker locker(mutex);
//    (void)locker;

    if (!_display_mutex.tryLock()) {
        qWarning() << "ImageWidget still locked. Dropping display of result.";
        return;
    }

    START_MEASURE_PERFORMANCE();

    // keep previous result around for smoother display
    std::shared_ptr<const ProcessingResult> previous_result = result;
    (void)previous_result;

    // save new result
    _result = result;

    // set image
    if (_result->imageKeys().contains(_current_image_key))
        setImage(_current_image_key);
    else
        setImage("original");

//    // show roi and center
    fitInView(_result->roi(), Qt::KeepAspectRatio);
    _current_center_point = _result->roi().center();

    // remove existing items
    QList<QGraphicsItem *> items = _scene->items();
    for (int i=0; i < items.size(); i++) {
        QGraphicsItem *item = items.at(i);

        if (item != _pixmap) {
            _scene->removeItem(item);
            delete item;
        }
    }

    // draw new items
    const std::vector<std::shared_ptr<BGraphicsItem> > graphics_items = _result->graphicsItems();
    for (std::vector<std::shared_ptr<BGraphicsItem> >::const_iterator it = graphics_items.begin(); it != graphics_items.end(); ++it) {
        _scene->addItem((*it)->toGraphicsItem());
    }

    // unlock mutex
    _display_mutex.unlock();

    STOP_MEASURE_PERFORMANCE();
}

void ImageWidget::changeImage()
{
    setImage(sender()->objectName());
}

void ImageWidget::setImage(QString key)
{
//    QMutexLocker locker(mutex);
//    (void)locker;

    if (!_result || !_result->imageKeys().contains(key))
        return;

    _current_image_key = key;
    _pixmap->setPixmap(QPixmap::fromImage(_result->getImage(key)));

    setSceneRect(_pixmap->boundingRect());

    // make sure the tooltip is updated
    updateToolTip();
}

void ImageWidget::saveCurrentImage()
{
//    QMutexLocker locker(mutex);
//    (void)locker;

    QImage image = _result->getImage(_current_image_key);

    if (!image.isNull()) {
        QSettings settings;

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), settings.value("ImageWidget/LastSavePath").toString() + _current_image_key + ".png", tr("Image Files (*.png *.bmp)"));

        if (fileName.isNull())
            return;

        QFileInfo info(fileName);
        settings.setValue("ImageWidget/LastSavePath", info.dir().canonicalPath() + "/");

        if (!image.save(fileName))
            QMessageBox::critical(this, tr("Could not save Image"), tr("Could not save Image to File %1").arg(fileName));
    }
}

void ImageWidget::saveCurrentScene()
{
//    QMutexLocker locker(mutex);
//    (void)locker;

    QSettings settings;

    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Scene"), settings.value("ImageWidget/LastSavePath").toString() + _current_image_key + ".svg", QString("%1;;%2;;%3;;%4;;%5").arg(tr("Scalable Vector Graphics Files (*.svg)")).arg(tr("Portable Document Format (*.pdf)")).arg("Windows Bitmap (*.bmp)").arg("Joint Photographic Experts Group (*.jpg)").arg("Portable Network Graphics (*.png)"));

    if (file_name.isNull())
        return;

    QFileInfo info(file_name);
    settings.setValue("ImageWidget/LastSavePath", info.dir().canonicalPath() + "/");

    if (file_name.endsWith(".svg")) {
        QSvgGenerator generator;
        generator.setFileName(file_name);
        generator.setSize(_pixmap->boundingRect().size().toSize());
        generator.setViewBox(_pixmap->boundingRect());
        generator.setTitle(_current_image_key);
        generator.setDescription("Inspector Scene");

        QPainter painter;
        painter.begin(&generator);
        _scene->render(&painter);
        painter.end();
        
    } else if (file_name.endsWith(".pdf")) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(file_name);
        
        QPainter painter;
        painter.begin(&printer);
        _scene->render(&painter);
        painter.end();
        
    } else {
        QImage image(_pixmap->boundingRect().width(), _pixmap->boundingRect().height(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);
        _scene->render(&painter);
        painter.end();
        image.save(file_name);
    }
}
