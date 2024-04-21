#ifndef POPUPDIALOG_H
#define POPUPDIALOG_H

#include <QDialog>

class PopupTitleWidget;


class PopupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PopupDialog(QWidget *parent = nullptr);
    
    void setTitleBackgroundColor(const QColor &color);
    
    QWidget *centralWidget() const;
    
    void mouseReleaseEvent(QMouseEvent *event);
    
signals:
    
public slots:
    
protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *event);
    
    PopupTitleWidget *_title;
    QWidget *_central_widget;
};

#endif // POPUPDIALOG_H
