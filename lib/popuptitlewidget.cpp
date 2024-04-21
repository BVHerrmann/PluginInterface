#include "popuptitlewidget.h"

#include <QtWidgets>


PopupTitleWidget::PopupTitleWidget(QWidget *parent) : QWidget(parent)
{
    QBoxLayout *box = new QHBoxLayout(this);
    box->setContentsMargins(0, 0, 0, 0);
    _title_label = new QLabel();
    box->addWidget(_title_label);
    
    box->addStretch();
    
    _close_button = new QPushButton("x");
    connect(_close_button, &QPushButton::clicked, this, &PopupTitleWidget::closeClicked);
    box->addWidget(_close_button);
}

void PopupTitleWidget::setTitle(const QString &title)
{
    _title_label->setText(title);
}

void PopupTitleWidget::setBackgroundColor(const QColor &color)
{
    setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void PopupTitleWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
