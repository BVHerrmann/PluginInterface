#include "bmessagebox.h"

#include "colors.h"

BMessageBox::BMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text, QWidget *parent) : PopupDialog(parent)
{
    _defaultButton = nullptr;
    _clickedButton = nullptr;
    
    switch(icon) {
        case QMessageBox::Warning:
            setTitleBackgroundColor(HMIColor::WarningLow);
            break;
        case QMessageBox::Critical:
            setTitleBackgroundColor(HMIColor::Alarm);
            break;
        default:
            break;
    }
    
    setWindowTitle(title);
    
    QBoxLayout *box = new QVBoxLayout();
    centralWidget()->setLayout(box);
    
    QLabel *label = new QLabel(text);
    label->setWordWrap(true);
    box->addWidget(label);
    
    // buttons
    _buttonBox = new QDialogButtonBox();
    connect(_buttonBox, &QDialogButtonBox::clicked, [=](QAbstractButton *button) { _clickedButton = button; });
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    box->addWidget(_buttonBox);
}

QMessageBox::StandardButton BMessageBox::information(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, QMessageBox::Information, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton BMessageBox::question(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, QMessageBox::Question, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton BMessageBox::warning(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, QMessageBox::Warning, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton BMessageBox::critical(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, QMessageBox::Critical, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton BMessageBox::showNewMessageBox(QWidget *parent, QMessageBox::Icon icon, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    BMessageBox msgBox(icon, title, text, parent);
    QDialogButtonBox *buttonBox = msgBox.findChild<QDialogButtonBox *>();
    assert(buttonBox != nullptr);
    
    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb)
            continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);
        // Choose the first accept role as the default
        if (msgBox.defaultButton())
            continue;
        if ((defaultButton == QMessageBox::NoButton && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
            || (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }
    
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QPushButton *BMessageBox::addButton(QMessageBox::StandardButton button)
{
    QPushButton *pushButton = _buttonBox->addButton((QDialogButtonBox::StandardButton)button);
    switch(buttonRole(pushButton)) {
        case QMessageBox::AcceptRole:
        case QMessageBox::ApplyRole:
        case QMessageBox::YesRole:
            break;
        case QMessageBox::RejectRole:
        case QMessageBox::NoRole:
            pushButton->setObjectName("close");
            break;
        case QMessageBox::DestructiveRole:
            pushButton->setObjectName("delete");
            break;
        case QMessageBox::ActionRole:
        case QMessageBox::ResetRole:
            pushButton->setObjectName("action");
            break;
        default:
            break;
    }
    
    pushButton->setStyleSheet(";"); // force evaulation of style sheet
    
    return pushButton;
}

QPushButton *BMessageBox::defaultButton() const
{
    return _defaultButton;
}

void BMessageBox::setDefaultButton(QPushButton *button)
{;
    if (!_buttonBox->buttons().contains(button))
        return;
    _defaultButton = button;
    button->setDefault(true);
    button->setFocus();
}

QAbstractButton *BMessageBox::clickedButton() const
{
    return _clickedButton;
}

QMessageBox::StandardButton BMessageBox::standardButton(QAbstractButton *button) const
{
    return (QMessageBox::StandardButton)_buttonBox->standardButton(button);
}

QMessageBox::ButtonRole BMessageBox::buttonRole(QAbstractButton *button) const
{
    return QMessageBox::ButtonRole(_buttonBox->buttonRole(button));
}
