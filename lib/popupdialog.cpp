#include "popupdialog.h"

#include <QtWidgets>

#include "popuptitlewidget.h"


PopupDialog::PopupDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
    
    // works for display, but causing popup not to receive mouse events on windows
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    
    // Add outer area to make clicking outside of dialog for closing work
    QBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(100, 100, 100, 100); // make sure the main content does not use all the space
    mainLayout->setSpacing(0);
    QWidget *mainWidget = new QWidget();
    mainWidget->setObjectName("PopupOutside");
    mainLayout->addWidget(mainWidget);
    setLayout(mainLayout);
    
    QBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addStretch();
    
    _title = new PopupTitleWidget();
    _title->setTitle(windowTitle());
    connect(_title, &PopupTitleWidget::closeClicked, [=]() { close(); });
    connect(this, &QDialog::windowTitleChanged, _title, &PopupTitleWidget::setTitle);
    contentLayout->addWidget(_title);
    
    _central_widget = new QWidget();
    _central_widget->setObjectName("Popup");
    contentLayout->addWidget(_central_widget);
    
    contentLayout->addStretch();
    
    QBoxLayout *layout = new QHBoxLayout();
    mainWidget->setLayout(layout);
    
    layout->addStretch();
    layout->addLayout(contentLayout);
    layout->addStretch();
}

void PopupDialog::setTitleBackgroundColor(const QColor &color)
{
    _title->setBackgroundColor(color);
}

QWidget *PopupDialog::centralWidget() const
{
    return _central_widget;
}

void PopupDialog::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PopupDialog::showEvent(QShowEvent *event)
{
    // center popup on parent
    if (parentWidget()) {
        QWidget *host = parentWidget()->window();
        if (host) {
            auto hostRect = host->geometry();
            move(hostRect.topLeft());
            resize(hostRect.size());
        }
    }
}

void PopupDialog::mouseReleaseEvent(QMouseEvent *event)
{
    // reject if clicked outside of popup
    if (!_central_widget->rect().contains(_central_widget->mapFromGlobal(event->screenPos().toPoint()))
        && !_title->rect().contains(_title->mapFromGlobal(event->screenPos().toPoint()))) {
        reject();
    }
    
    QDialog::mouseReleaseEvent(event);
}
