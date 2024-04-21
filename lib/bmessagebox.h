#ifndef BMESSAGEBOX_H
#define BMESSAGEBOX_H

#include <QtWidgets>

#include "popupdialog.h"


class BMessageBox : public PopupDialog
{
    Q_OBJECT
public:
    explicit BMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text, QWidget *parent = nullptr);
    
    static QMessageBox::StandardButton information(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton question(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No), QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton warning(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton critical(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    
    QPushButton *addButton(QMessageBox::StandardButton button);
    
    QPushButton *defaultButton() const;
    void setDefaultButton(QPushButton *button);
    
    QAbstractButton *clickedButton() const;
    
    QMessageBox::StandardButton standardButton(QAbstractButton *button) const;
    QMessageBox::ButtonRole buttonRole(QAbstractButton *button) const;
    
signals:
    
public slots:
    
protected:
    static QMessageBox::StandardButton showNewMessageBox(QWidget *parent, QMessageBox::Icon icon, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton);
    
    QDialogButtonBox *_buttonBox;
    
    QPushButton *_defaultButton;
    QAbstractButton *_clickedButton;
    
};

#endif // BMESSAGEBOX_H
