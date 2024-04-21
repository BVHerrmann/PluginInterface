#include "bdistancespinbox.h"

#include <QtCore>


BDistanceSpinBox::BDistanceSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    _factor = 1.0;
    connect(this, static_cast<void (BDistanceSpinBox::*)(double)>(&BDistanceSpinBox::valueChanged), this, &BDistanceSpinBox::baseValueChanged);
}

void BDistanceSpinBox::setFactor(double value)
{
    double previousStoredValue = storedValue();

    _factor = value;

    setStoredValue(previousStoredValue);
}

double BDistanceSpinBox::storedValue() const
{
    return value() * _factor;
}

void BDistanceSpinBox::setStoredValue(double value)
{
    setValue(value / _factor);
}

void BDistanceSpinBox::baseValueChanged(double value)
{
    (void)value;

    emit storedValueChanged(storedValue());
}
