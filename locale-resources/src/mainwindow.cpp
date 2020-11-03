#include "mainwindow.hpp"
#include "translations.hpp"
#include "ui_mainwindow.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow{parent},
    ui_{new Ui::MainWindow}
{
    ui_->setupUi(this);
    // qApp is a macro that expands to the reference to QCoreApplication
    // subclass that was created in main() function.
    connect(ui_->quitAction,&QAction::triggered,qApp,&QApplication::quit);
    auto translations = getAvailableTranslations();
    auto translationNames = getFullTranslationNames(translations);
    languagesGroup_ = new QActionGroup(ui_->languageMenu);
    connect(languagesGroup_,&QActionGroup::triggered,this,&MainWindow::langActionSelected);
    for(int i=0;i<translations.size();++i){
        auto langAction = new QAction(translationNames[i],languagesGroup_);
        langAction->setData(translations[i]);
        langAction->setCheckable(true);
        languagesGroup_->addAction(langAction);
        ui_->languageMenu->addAction(langAction);
    }
    loadSystemLocale();
    if(auto ca = languagesGroup_->checkedAction())
        langActionSelected(ca);
}

MainWindow::~MainWindow() = default;

void MainWindow::changeEvent(QEvent* event)
{
    switch(event->type()){
        case QEvent::LanguageChange:
            ui_->retranslateUi(this);
            setWindowTitle(qApp->applicationDisplayName());
            break;
        case QEvent::LocaleChange:
            loadSystemLocale();
            break;
        default:
            ;
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::loadSystemLocale()
{
    auto name = QLocale::system().name();
    name.truncate(name.indexOf(QLatin1Char('_')));
    QAction* systemLanguageAction = nullptr;
    for(QAction* langAction:languagesGroup_->actions()){
        QString lang = langAction->data().toString();
        if(lang==name){
            systemLanguageAction = langAction;
            break;
        }else if(lang==QStringLiteral("en"))
            systemLanguageAction = langAction;
    }
    if(systemLanguageAction)
        systemLanguageAction->setChecked(true);
}

void MainWindow::langActionSelected(QAction* action)
{
    loadTranslation(action->data().toString());
}

void MainWindow::on_openAction_triggered()
{
    // tr() converts to QString and also marks the text as translatable
    // for lupdate to collect into .ts files. These strings are grouped
    // by contexts, which for QObject::tr is the object name.
    if(auto fileName = QFileDialog::getOpenFileName(this,tr("Open file"));
            !fileName.isEmpty()){
        QFileInfo fi(fileName);
        if(!fi.isReadable())
            // Besides context, an extra comment can be associated with a
            // translatable string to help the traslator.
            QMessageBox::warning(this,tr("Error","File read error dialog title"),
            /*: A comment that starts with a ':' before a tr() call is also
                extracted as a comment for translator. */
                                 tr("File %1 is not readable.").arg(fileName));
        else
            // To use a proper wordform for numerals and related words,
            // the third parameter to tr is the number to insert into position
            // of "%n" in the translated string. Linguist tool will allow
            // 1-3 different translations for this string, depending o the language,
            // for example, in Russian, 1 штука/2 штуки/5 штук.
            // We clamp this number to a multiple of thousands to avoid qint64->int
            // implicit conversion warning and use %1 instead of %n to manually
            // insert the full 64-bit number after translation.
            QMessageBox::information(this,tr("Info"),
                tr("This file is %1 byte(s) long.","",int((fi.size())%1000)).arg(fi.size()));
    }
}

namespace
{
    class Worker
    {
        // A class that is not a derivative of QObject can gain
        // member tr() function by using this macro.
        Q_DECLARE_TR_FUNCTIONS(Worker)
    public:
        QString do_it(bool protection)
        {
            return tr(results[protection]);
        }
    private:
        static const char* results[2];
    };

    const char* Worker::results[2] = {
        // To mark translatable text that is outside the argument to
        // tr()-like call, use a macro QT_TR_NOOP (current context) or
        // QT_TRANSLATE_NOOP (explicit context as second argument).
        // A call to a trsnation function is still required.
        QT_TRANSLATE_NOOP("Worker","Ouch, that hurts!"),
        QT_TRANSLATE_NOOP("Worker","Process completed successfully.")
    };
}

void MainWindow::on_runAction_triggered()
{
    if(QMessageBox::question(this,tr("Run"),tr("Do you want to start processing?"),
                             QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
        ui_->textBrowser->append(Worker().do_it(ui_->protectionAction->isChecked()));
}

void MainWindow::on_aboutAction_triggered()
{
    QMessageBox::about(this,qApp->applicationName(),
        // Instead of tr() in classes QCoreApplication::translate is available
        // globally with explicit context argument.
        qApp->translate("Global","%1 version %2")
            .arg(qApp->applicationDisplayName())
            .arg(qApp->applicationVersion()));
}
