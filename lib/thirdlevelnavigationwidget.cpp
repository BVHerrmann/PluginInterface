#include "thirdlevelnavigationwidget.h"

#include <QtWidgets>

ThirdLevelNavigationWidget::ThirdLevelNavigationWidget(QWidget *parent) : QTabWidget(parent)
{

}

int ThirdLevelNavigationWidget::addTab(QWidget *child, const QString &label)
{
	return insertTab(-1, child, label);
}

int ThirdLevelNavigationWidget::addTab(QWidget *child, const QIcon& icon, const QString &label)
{
	return insertTab(-1, child, icon, label);
}

int ThirdLevelNavigationWidget::insertTab(int index, QWidget *w, const QString &label)
{
	return insertTab(index, w, QIcon(), label);
}

int ThirdLevelNavigationWidget::insertTab(int index, QWidget *w, const QIcon& icon, const QString &label)
{
	if (index == -1) {
		clearTabs();
	}

	QLabel *textLabel = new QLabel(label);
	int idx = QTabWidget::insertTab(index, w, icon, QString());
	tabBar()->setTabButton(idx, QTabBar::LeftSide, textLabel);

	fillTabs();
	return idx;
}

void ThirdLevelNavigationWidget::removeTab(int index)
{
	QTabWidget::removeTab(index);
	fillTabs();
}


QString ThirdLevelNavigationWidget::tabText(int index) const
{
    QWidget *button = tabBar()->tabButton(index, QTabBar::LeftSide);
    if (button && qobject_cast<QLabel *>(button)) {
        return qobject_cast<QLabel *>(button)->text();
    }
    else {
        return QTabWidget::tabText(index);
    }
}

void ThirdLevelNavigationWidget::setTabText(int index, const QString &text)
{
    QWidget *button = tabBar()->tabButton(index, QTabBar::LeftSide);
    if (button && qobject_cast<QLabel *>(button)) {
        qobject_cast<QLabel *>(button)->setText(text);
    }
    else {
        QTabWidget::setTabText(index, text);
    }
}

void ThirdLevelNavigationWidget::resizeEvent(QResizeEvent * event)
{
	fillTabs();
}

void ThirdLevelNavigationWidget::clearTabs()
{
	for (int i = count(); i > 0; --i) {
		int idx = i - 1;
		if (tabBar()->isTabEnabled(idx) == false) {
			QTabWidget::removeTab(idx);
		}
		else {
			break;
		}
	}
}

void ThirdLevelNavigationWidget::fillTabs()
{
	int space = size().height() - (count() * 56);
	if (space > 0) {
		int to_add = space / 56;
		for (int i = 0; i < to_add; ++i) {
			int idx = QTabWidget::addTab(new QWidget(), QString());
			tabBar()->setTabEnabled(idx, false);
		}
	}
	else {
		int to_remove = std::ceil(std::abs(space) / 56);
		for (int i = 0; i < to_remove; ++i) {
			int idx = count();
			if (tabBar()->isTabEnabled(idx) == false) {
				QTabWidget::removeTab(idx);
			}
			else {
				break;
			}
		}
	}
}
