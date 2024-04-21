#ifndef INSPECTIONRESULT_H
#define INSPECTIONRESULT_H

#include <QDateTime>
#include <QHash>
#include <QString>

#include "processingresult.h"


class InspectionResult
{
public:
    explicit InspectionResult();
    virtual ~InspectionResult();

    QString title() const;
    QString resultClass() const { return _result_class; }
    QString additionalTitle() const { return _additional_title; }

    void setResultClass(QString value) { _result_class = value; }
    void setAdditionalTitle(QString value) { _additional_title = value; }

    virtual QWidget *resultWidget(std::shared_ptr<const InspectionResult> result = std::shared_ptr<const InspectionResult>(), QWidget* parent = 0) const;

    const std::map<int, std::shared_ptr<const ProcessingResult> > processingResults() const { return _processing_results; }
    const std::shared_ptr<const ProcessingResult> processingResult(const int key) const { auto it = _processing_results.find(key); return it == _processing_results.end() ? std::shared_ptr<const ProcessingResult>() : it->second; }
    void addResult(const int key, const std::shared_ptr<const ProcessingResult> & value) { _processing_results[key] = value; }
	
	void saveResult() const;

protected:
    QDateTime _time_stamp;
    QString _result_class;
    QString _additional_title;

    std::map<int, std::shared_ptr<const ProcessingResult>> _processing_results;
};

#endif // INSPECTIONRESULT_H
