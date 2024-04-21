#ifndef POPUPTITLEWIDGET_H
#define POPUPTITLEWIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;


class PopupTitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupTitleWidget(QWidget *parent = nullptr);
    
    void setTitle(const QString &title);
    void setBackgroundColor(const QColor &color);
    
signals:
    void closeClicked();
    
public slots:
    
protected:
    void paintEvent(QPaintEvent *event);
    
    QLabel *_title_label;
    QPushButton *_close_button;
};

#endif // POPUPTITLEWIDGET_H
