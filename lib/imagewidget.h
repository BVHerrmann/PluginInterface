#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtWidgets>

#include "processingresult.h"

typedef enum {
    ActionCreate,
    ActionMove,
    ActionResize
} ImageAction;


class ImageWidget : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = 0);
    virtual ~ImageWidget();

    void setupUi();
    void updateToolTip();
    void setCenter(const QPointF& centerPoint);

	bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent * event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    std::shared_ptr<const ProcessingResult> result() const { return _result; }

signals:

public slots:
    void displayProcessingResult(std::shared_ptr<const ProcessingResult> result);
    void changeImage();
    void setImage(QString key);

    void zoomActualSize();
    void zoomToFit();
    void zoomIn();
    void zoomOut();

    void saveCurrentImage();
    void saveCurrentScene();

protected:
	bool gestureEvent(QGestureEvent *event);
	void panTriggered(QPanGesture *event);
	void pinchTriggered(QPinchGesture *event);
	QTransform _pinchStartTransform;

    std::shared_ptr<const ProcessingResult> _result;

    QGraphicsScene *_scene;
    QGraphicsPixmapItem *_pixmap;

    QString _current_image_key;

    // helper for paning and zooming
    QRectF _previous_viewport_rect;
    QPointF _current_center_point;
    QPoint _last_pan_point;

    QMutex _display_mutex;
};

#endif // IMAGEWIDGET_H
