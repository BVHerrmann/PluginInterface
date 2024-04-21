#include "inspectionresult.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "processingresult.h"
#include "imagewidget.h"


InspectionResult::InspectionResult()
{
    _time_stamp = QDateTime::currentDateTime();
}

InspectionResult::~InspectionResult()
{
    _processing_results.clear();
}

QString InspectionResult::title() const
{
    QString title = _time_stamp.toString("hh:mm:ss.zzz");

    if (!_additional_title.isEmpty())
        title = title + " (" + _additional_title + ")";

    return title;
}

/*
 Build generic widget for this result
 TODO: Improve to allow custom builder
 */
QWidget * InspectionResult::resultWidget(std::shared_ptr<const InspectionResult> result, QWidget* parent) const
{
    (void)result;

    QWidget *widget = new QWidget(parent);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    widget->setLayout(layout);

    // display all processing results
    QHBoxLayout *image_box = new QHBoxLayout();
    layout->addLayout(image_box);

    for (auto it = _processing_results.cbegin(); it != _processing_results.cend(); ++it) {
        ImageWidget *image_widget = new ImageWidget(widget);
        image_widget->displayProcessingResult(it->second);
        image_box->addWidget(image_widget);
    }

    return widget;
}

void InspectionResult::saveResult() const
{
	QDir dir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
	
	QString applicationName = qApp->applicationName();
	if (!dir.exists(applicationName)) {
		if (!dir.mkdir(applicationName)) {
			qWarning() << "Failed to make a Application directory";
			return;
		}
	} 
	dir.cd(applicationName);

	QString resultDir = QObject::tr("Result") + " " + _time_stamp.toString("yyyy-MM-dd hh.mm.ss");
	if (dir.mkdir(resultDir)) {
		dir.cd(resultDir);

		for (auto it = _processing_results.cbegin(); it != _processing_results.cend(); ++it) {
			
			QImage image = it->second->originalImage();

			if (!image.isNull()) {
				image.save(dir.filePath(QString::number(it->second->cameraId()) + ".bmp"));
			}
		}
	}
	else {
		qWarning() << "Failed to make a directory";
	}
}