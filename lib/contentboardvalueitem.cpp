#include "contentboardvalueitem.h"

#include <QtWidgets>
#include <QtGui>


ContentBoardValueItem::ContentBoardValueItem(const QString &title, QWidget *parent) : ContentBoardItem(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    
    _title_label = new QLabel(title);
    _title_label->setObjectName("title");
    
    _value_label = new QLabel("");
    _value_label->setObjectName("subTitle");
    
    layout->addWidget(_title_label, 0, 0, Qt::AlignTop);
    layout->addWidget(_value_label, 1, 0, Qt::AlignBottom);
}

void ContentBoardValueItem::setTitle(const QString &value)
{
    _title_label->setText(value);
}

void ContentBoardValueItem::setValue(const QString &value)
{
    _value_label->setText(value);
}
