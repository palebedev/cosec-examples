#include "copywidget.hpp"
#include "ui_copywidget.h"

#include "filecopier.hpp"

#include <QtCore/QThread>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QMessageBox>

CopyWidget::CopyWidget(QWidget* parent)
    : QWidget{parent},
      ui_{new Ui::CopyWidget},
      // A thread to run a separate thread and event loop.
      // a QThread object itself remains in the main thread
      // as a child of this widget.
      thread_{new QThread{this}},
      // A File copier object with no parent.
      fileCopier_{new FileCopier}
{
    ui_->setupUi(this);
    // A QCompleter provides an autocompletion source for line edits.
    auto completer = new QCompleter{this};
    // It can take its candidates from a QAbstractItemModel.
    // A QFileSystemModel is a model for the live filesystem.
    auto model = new QFileSystemModel{completer};
    model->setRootPath({});
    completer->setModel(model);
    ui_->sourceEdit->setCompleter(completer);
    ui_->destinationEdit->setCompleter(completer);

    // Start the new thread. Base implementation of QThread::run() is an event loop.
    thread_->start();
    // Move our file copier object to the new thread.
    // Now signal/slot connections from main thread will use
    // a queued connection that sends events for slot activations to another thread.
    fileCopier_->moveToThread(thread_);
    // Make a connection to delete file copier object when its thread finishes.
    connect(thread_,&QThread::finished,fileCopier_,&QObject::deleteLater);
    connect(fileCopier_,&FileCopier::progressChanged,ui_->progressBar,&QProgressBar::setValue);
    connect(fileCopier_,&FileCopier::done,this,&CopyWidget::done);
}

CopyWidget::~CopyWidget()
{
    // We must stop our helper thread before destroying a QThread instance
    // that manages it. Signal its message loop to quit and wait for it.
    thread_->quit();
    thread_->wait();
}

void CopyWidget::closeEvent(QCloseEvent* event)
{
    if(running_){
        // If a use tries to close our window while the copy is running,
        // don't close and simulate click of the cancel button.
        ui_->runCancelButton->click();
        event->ignore();
    }else
        // If not, close normally.
        event->accept();
}

void CopyWidget::on_browseSourceButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select copy source"));
    if(!fileName.isNull())
        ui_->sourceEdit->setText(fileName);
}

void CopyWidget::on_browseDestinationButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Select copy destination"));
    if(!fileName.isNull())
        ui_->destinationEdit->setText(fileName);
}

void CopyWidget::on_runCancelButton_clicked()
{
    // Run and Cancel is a single button which we switch the caption for.
    if(!running_){
        ui_->groupBox->setEnabled(false);
        ui_->runCancelButton->setText(tr("&Cancel"));
        // Reset cancellation flag here to make all manipulation of it
        // from a single thread to avoid races.
        fileCopier_->setCancelled(false);
        // Call a slot we need through a meta object.
        // We would have to make a signal to connect to it otherwise.
        QMetaObject::invokeMethod(fileCopier_,"copyFile",
                                  Q_ARG(QString,ui_->sourceEdit->text()),
                                  Q_ARG(QString,ui_->destinationEdit->text()));
        running_ = true;
    }else{
        ui_->runCancelButton->setEnabled(false);
        fileCopier_->setCancelled(true);
    }
}

void CopyWidget::done(const QString& result)
{
    QMessageBox::information(this,tr("Copy result"),result);
    ui_->groupBox->setEnabled(true);
    ui_->runCancelButton->setText(tr("&Run"));
    ui_->runCancelButton->setEnabled(true);
    running_ = false;
}
