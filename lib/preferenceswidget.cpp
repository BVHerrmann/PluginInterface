#include "preferenceswidget.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "bdistancespinbox.h"


PreferencesWidget::PreferencesWidget(QWidget *parent) :
    QWidget(parent)
{
}

void PreferencesWidget::showEvent(QShowEvent *event)
{
    this->dynamicLoadPreferences(this);
    QWidget::showEvent(event);
}

void PreferencesWidget::dynamicLoadPreferences(QWidget *widget)
{
    // Recurse through all widget children
    for (QObject *child : widget->children()) {
        if (child->isWidgetType())
            dynamicLoadPreferences(qobject_cast<QWidget*>(child));
    }

    if (widget->dynamicPropertyNames().contains(kPropertyKey)) {
        QSettings settings;
        QString property = widget->property(kPropertyKey).toString();

        if (settings.contains(property)) {
            QVariant value = settings.value(property);

            BDistanceSpinBox *distanceSpinBox = 0;
            QSpinBox *spinBox = 0;
            QDoubleSpinBox *doubleSpinBox = 0;

            if ((distanceSpinBox = dynamic_cast<BDistanceSpinBox *>(widget)) != 0) {
                distanceSpinBox->setStoredValue(value.toDouble());
            } else if ((spinBox = dynamic_cast<QSpinBox *>(widget)) != 0) {
                spinBox->setValue(value.toInt());
            } else if ((doubleSpinBox = dynamic_cast<QDoubleSpinBox *>(widget)) != 0) {
                doubleSpinBox->setValue(value.toDouble());
            }
        }
    }
}

void PreferencesWidget::changeValue(QObject *sender, QVariant value)
{
    if (!sender) {
        qWarning() << "No sender for changeValue(" << value << ")!";
        return;
    }

    QVariant propertyKey = sender->property(kPropertyKey);
    if (propertyKey.isValid()) {
        QSettings settings;
        if (value.isValid() && !value.isNull()) {
            settings.setValue(propertyKey.toString(), value);
        } else {
            settings.remove(propertyKey.toString());
        }
    } else {
        qWarning() << "No property \"" << kPropertyKey << "\" for sender" << sender << "value" << value;
        return;
    }
}

void PreferencesWidget::changeValue(bool value)
{
    changeValue(QObject::sender(), QVariant(value));
}

void PreferencesWidget::changeValue(int value)
{
    changeValue(QObject::sender(), QVariant(value));
}

void PreferencesWidget::changeValue(double value)
{
    changeValue(QObject::sender(), QVariant(value));
}

void PreferencesWidget::changeValue(const QString &value)
{
    changeValue(QObject::sender(), QVariant(value));
}

void PreferencesWidget::changeIndex(int index)
{
    if (QComboBox *comboBox = qobject_cast<QComboBox *>(QObject::sender())) {
        changeValue(QObject::sender(), QVariant(comboBox->itemData(index)));
    } else {
        qWarning() << "Unsupported Widget Type for sender" << QObject::sender();
    }
}
