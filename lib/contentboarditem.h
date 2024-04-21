#ifndef CONTENTBOARDITEM_H
#define CONTENTBOARDITEM_H

#include <QWidget>


class ContentBoardItem : public QWidget
{
    Q_OBJECT
    
public:
    explicit ContentBoardItem(QWidget *parent = nullptr);
    
signals:
    
public slots:
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
};

#endif // CONTENTBOARDITEM_H
