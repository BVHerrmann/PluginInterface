#ifndef CONTENTBOARDVALUEITEM_H
#define CONTENTBOARDVALUEITEM_H

#include "contentboarditem.h"

class QLabel;


class ContentBoardValueItem : public ContentBoardItem
{
    Q_OBJECT
    
public:
    explicit ContentBoardValueItem(const QString &title, QWidget *parent = nullptr);
    
signals:
    
public slots:
    void setTitle(const QString &title);
    void setValue(const QString &value);
    
protected:
    QLabel *_title_label;
    QLabel *_value_label;
    
private:
};

#endif // CONTENTBOARDVALUEITEM_H
