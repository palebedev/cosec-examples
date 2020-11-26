#include "mainwindow.hpp"

#include "ui_mainwindow.h"

#include "notes.hpp"
#include "notesmodel.hpp"
#include <qitemselectionmodel.h>

MainWindow::MainWindow(QWidget* parent)
    : QWidget{parent},
      ui_{new Ui::MainWindow},
      notes_{new Notes},
      model_{new NotesModel{this}}
{
    ui_->setupUi(this);
    model_->setNotes(notes_.get());
    ui_->treeView->setModel(model_);
    connect(ui_->treeView->selectionModel(),&QItemSelectionModel::selectionChanged,[this]{
        if(currentNote){
            currentNote->setText(ui_->textEdit->toPlainText());
            currentNote = nullptr;
        }
        auto indices = ui_->treeView->selectionModel()->selectedIndexes();
        bool insertEnabled = indices.empty()||
            (indices.size()==1&&maybe_folder(indices.front()));
        ui_->newFolderButton->setEnabled(insertEnabled);
        ui_->newItemButton->setEnabled(insertEnabled);
        ui_->deleteButton->setEnabled(!indices.isEmpty());
        if(indices.size()==1&&(currentNote=maybe_note(indices.front()))){
            ui_->textEdit->setEnabled(true);
            ui_->textEdit->setPlainText(currentNote->text());
        }else{
            ui_->textEdit->clear();
            ui_->textEdit->setEnabled(false);
        }
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::insert(void (*inserter)(NotesModel*,const QModelIndex&))
{
    auto selection = ui_->treeView->selectionModel()->selectedIndexes();
    auto index = selection.isEmpty()?QModelIndex{}:selection.front();
    ui_->treeView->setExpanded(index,true);
    inserter(model_,index);
    auto newIndex = model_->index(model_->rowCount(index)-1,0,index);
    ui_->treeView->setCurrentIndex(newIndex);
    ui_->treeView->edit(newIndex);
}

void MainWindow::on_newFolderButton_clicked()
{
    insert([](NotesModel* model,const QModelIndex& index){
        model->insertFolder(model->rowCount(index),index);
    });
}

void MainWindow::on_newItemButton_clicked()
{
    insert([](NotesModel* model,const QModelIndex& index){
        model->insertRow(model->rowCount(index),index);
    });
}

void MainWindow::on_deleteButton_clicked()
{
    // Remove by one and requiry selected items to handle any
    // kind of multiple selections.
    QModelIndexList selected;
    while(selected = ui_->treeView->selectionModel()->selectedIndexes(),
          !selected.isEmpty())
        model_->removeRow(selected.front().row(),selected.front().parent());
}
