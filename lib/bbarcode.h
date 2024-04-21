#ifndef BBARCODE_H
#define BBARCODE_H

#include <QString>
#include <QPolygonF>


typedef enum {
    EAN13   = 0,
    UPCA    = 1
} BarcodeType;


class BBarcode
{
public:
    BBarcode(BarcodeType type, const QString &data);
    virtual ~BBarcode() { }

    BarcodeType type() { return _type; }
    
    QString data() { return _data; }
    
    QPolygonF area() { return _area; }
    void setArea(const QPolygonF &value) { _area = value; }
    
	QPointF center() { return _center; }
	void setCenter(const QPointF &value) { _center = value; }

	double angle() { return _angle; }
	void setAngle(const double &value) { _angle = value; }

protected:
    BarcodeType _type;
    QString _data;

    QPolygonF _area;
	QPointF _center;
	double _angle;
};

#endif // BBARCODE_H
