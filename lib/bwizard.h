#ifndef BWIZARD_H
#define BWIZARD_H

#include "popupdialog.h"

#include <QtCore>
#include <QtWidgets>
#include <QtGui>


class BWizard : public PopupDialog
{
    Q_OBJECT
public:
    explicit BWizard(QWidget *parent = nullptr);
    
    int addPage(QWizardPage *page);
    void setPage(int id, QWizardPage *page);
    
    void setOption(QWizard::WizardOption option, bool on = true);
    bool testOption(QWizard::WizardOption option) const;
    void setOptions(QWizard::WizardOptions options);
    QWizard::WizardOptions options() const;
    
public slots:
    void back();
    void next();
    
    void _q_updateButtonStates();
    
protected:
    enum Direction {
        Backward,
        Forward
    };
    
    void switchToPage(int newId, Direction direction);
    void updateCurrentPage();
    
    QWizardPage *page(int theid) const;
    QWizardPage *currentPage() const;
    bool validateCurrentPage();
    int nextId() const;
    
    QMap<int, QWizardPage *> _pageMap;
    
    QBoxLayout *_layout;
    QPushButton *_btnBack = nullptr;
    QPushButton *_btnNext = nullptr;
    QPushButton *_btnDone = nullptr;
    QPushButton *_btnCancel = nullptr;
    
    QWizard::WizardOptions _opts = { };
    int _current = -1;
    bool _canContinue = false;
    bool _canFinish = false;
};

#endif // BWIZARD_H
