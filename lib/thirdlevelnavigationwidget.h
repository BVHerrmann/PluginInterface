#ifndef THIRDLEVELNAVIGATIONWIDGET_H
#define THIRDLEVELNAVIGATIONWIDGET_H

#include <QTabWidget>


class ThirdLevelNavigationWidget : public QTabWidget
{
    Q_OBJECT
    
public:
    explicit ThirdLevelNavigationWidget(QWidget *parent = nullptr);
    
    int addTab(QWidget *child, const QString &label);
    int addTab(QWidget *child, const QIcon& icon, const QString &label);
    
    int insertTab(int index, QWidget *w, const QString &label);
    int insertTab(int index, QWidget *w, const QIcon& icon, const QString &label);
    
    void removeTab(int index);
    
    QString tabText(int index) const;
    void setTabText(int index, const QString &);

    void resizeEvent(QResizeEvent *event) override;
signals:
    
public slots:
    
private:
    void clearTabs();
    void fillTabs();
};

#endif // THIRDLEVELNAVIGATIONWIDGET_H
