#ifndef OPTIONPANEL_H
#define OPTIONPANEL_H

#include <QWidget>


class OptionPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit OptionPanel(QWidget *parent = nullptr);

signals:
    
public slots:
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
};

#endif // OPTIONPANEL_H
