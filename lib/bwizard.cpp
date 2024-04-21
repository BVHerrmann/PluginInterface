#include "bwizard.h"

#include "popuptitlewidget.h"


BWizard::BWizard(QWidget *parent) : PopupDialog(parent)
{
    QVBoxLayout *box = new QVBoxLayout();
    centralWidget()->setLayout(box);
    
    _layout = new QVBoxLayout();
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);
    box->addLayout(_layout);
    
    QHBoxLayout *button_box = new QHBoxLayout();
    button_box->addStretch();
    _btnBack = new QPushButton(tr("Go Back"));
    connect(_btnBack, &QPushButton::clicked, [=](){ back(); });
    button_box->addWidget(_btnBack);
    _btnNext = new QPushButton(tr("Continue"));
    connect(_btnNext, &QPushButton::clicked, [=](){ next(); });
    button_box->addWidget(_btnNext);
    _btnDone = new QPushButton(tr("Done"));
    connect(_btnDone, &QPushButton::clicked, [=](){ accept(); });
    _btnDone->setObjectName("action");
    button_box->addWidget(_btnDone);
    _btnCancel = new QPushButton(tr("Cancel"));
    connect(_btnCancel, &QPushButton::clicked, [=](){ reject(); });
    _btnCancel->setObjectName("close");
    button_box->addWidget(_btnCancel);
    
    box->addLayout(button_box);
}

int BWizard::addPage(QWizardPage *page)
{
    int theid = 0;
    if (!_pageMap.isEmpty())
        theid = (_pageMap.constEnd() - 1).key() + 1;
    setPage(theid, page);
    
    updateCurrentPage();
    
    return theid;
}

void BWizard::setPage(int theid, QWizardPage *page)
{
    if (Q_UNLIKELY(!page)) {
        qWarning("QWizard::setPage: Cannot insert null page");
        return;
    }

    if (Q_UNLIKELY(theid == -1)) {
        qWarning("QWizard::setPage: Cannot insert page with ID -1");
        return;
    }

    if (Q_UNLIKELY(_pageMap.contains(theid))) {
        qWarning("QWizard::setPage: Page with duplicate ID %d ignored", theid);
        return;
    }

    _pageMap.insert(theid, page);
    
    _layout->addWidget(page);
    if (theid == 0) {
        _current = 0;
    } else {
        page->hide();
    }

    PopupDialog::connect(page, SIGNAL(completeChanged()), this, SLOT(_q_updateButtonStates()));
}

void BWizard::switchToPage(int newId, Direction direction)
{
    if (QWizardPage *oldPage = currentPage()) {
        oldPage->hide();
        
        if (direction == Backward) {
            if (!(_opts & QWizard::IndependentPages)) {
                oldPage->cleanupPage();
            }
        }
    }

    _current = newId;

    QWizardPage *newPage = currentPage();
    if (newPage) {
        if (direction == Forward) {
            newPage->initializePage();
        }
        
        newPage->show();
    }

    updateCurrentPage();
}

void BWizard::updateCurrentPage()
{
    if (currentPage()) {
        setWindowTitle(currentPage()->title());
        
        _canContinue = (nextId() != -1);
        _canFinish = _current == (_pageMap.size() - 1);
    } else {
        _canContinue = false;
        _canFinish = false;
    }
    _q_updateButtonStates();
}

void BWizard::_q_updateButtonStates()
{
    const QWizardPage *page = currentPage();
    bool complete = page && page->isComplete();

    _btnBack->setEnabled(_current > 0
                         && (!_canFinish || !(_opts & QWizard::DisabledBackButtonOnLastPage)));
    _btnNext->setEnabled(_canContinue && complete);
    _btnDone->setEnabled(_canFinish && complete);
    
    const bool backButtonVisible = _btnBack
        && ( _current > 0 || !(_opts & QWizard::NoBackButtonOnStartPage))
        && (_canContinue || !(_opts & QWizard::NoBackButtonOnLastPage));
    bool commitPage = page && page->isCommitPage();
    _btnBack->setVisible(backButtonVisible);
    _btnNext->setVisible(_btnNext && !commitPage
                         && (_canContinue || (_opts & QWizard::HaveNextButtonOnLastPage)));
    _btnDone->setVisible(_btnDone
                           && (_canFinish || (_opts & QWizard::HaveFinishButtonOnEarlyPages)));

    if (!(_opts & QWizard::NoCancelButton))
        _btnCancel->setVisible(_btnCancel
                               && (_canContinue || !(_opts & QWizard::NoCancelButtonOnLastPage)));

    bool useDefault = !(_opts & QWizard::NoDefaultButton);
    if (QPushButton *nextPush = qobject_cast<QPushButton *>(_btnNext))
        nextPush->setDefault(_canContinue && useDefault && !commitPage);
    if (QPushButton *finishPush = qobject_cast<QPushButton *>(_btnDone))
        finishPush->setDefault(!_canContinue && useDefault);
}

QWizardPage *BWizard::page(int theid) const
{
    return _pageMap.value(theid);
}

QWizardPage *BWizard::currentPage() const
{
    return page(_current);
}

void BWizard::back()
{
    if (_current <= 0) {
        return;
    }
    
    switchToPage(_current - 1, Backward);
}

void BWizard::next()
{
    if (_current == -1)
        return;

    if (validateCurrentPage()) {
        int next = nextId();
        if (next != -1) {
            if (Q_UNLIKELY(!_pageMap.contains(next))) {
                qWarning("QWizard::next: No such page %d", next);
                return;
            }
            switchToPage(next, Forward);
        }
    }
}

bool BWizard::validateCurrentPage()
{
    QWizardPage *page = currentPage();
    if (!page)
        return true;

    return page->validatePage();
}

int BWizard::nextId() const
{
    const QWizardPage *page = currentPage();
    if (!page)
        return -1;
    
    // from QWizardPage::nextId()
    bool foundCurrentPage = false;
    auto i = _pageMap.constBegin();
    auto end = _pageMap.constEnd();

    for (; i != end; ++i) {
        if (i.value() == page) {
            foundCurrentPage = true;
        } else if (foundCurrentPage) {
            return i.key();
        }
    }
    
    return -1;
}

void BWizard::setOption(QWizard::WizardOption option, bool on)
{
    if (!(_opts & option) != !on)
        setOptions(_opts ^ option);
}

bool BWizard::testOption(QWizard::WizardOption option) const
{
    return (_opts & option) != 0;
}

void BWizard::setOptions(QWizard::WizardOptions options)
{
    QWizard::WizardOptions changed = (options ^ _opts);
    if (!changed)
        return;

    _opts = options;
}

QWizard::WizardOptions BWizard::options() const
{
    return _opts;
}
