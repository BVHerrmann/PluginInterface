#ifndef BDISTANCESPINBOX_H
#define BDISTANCESPINBOX_H

#include <QDoubleSpinBox>


class BDistanceSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

    Q_PROPERTY(double factor READ factor WRITE setFactor)
    Q_PROPERTY(double storedValue READ storedValue WRITE setStoredValue NOTIFY storedValueChanged USER true)

public:
    explicit BDistanceSpinBox(QWidget *parent = 0);
    
    double factor() const { return _factor; }

    double storedValue() const;

signals:
    void storedValueChanged(double value);

public slots:
    void setFactor(double value);

    void setStoredValue(double value);

    void baseValueChanged(double value);

private:
    double _factor;
};

#endif // BDISTANCESPINBOX_H
