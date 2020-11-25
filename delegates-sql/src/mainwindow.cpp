#include "mainwindow.hpp"

#include "ui_mainwindow.h"

#include "colordelegate.hpp"
#include "fabricsmodel.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QWidget{parent},
      ui_{new Ui::MainWindow},
      model_{new FabricsModel{this}},
      cd_{new ColorDelegate{this}}
{
    ui_->setupUi(this);
    ui_->tableView->setModel(model_);
    ui_->tableView->resizeColumnsToContents();
    ui_->tableView->setItemDelegateForColumn(2,cd_);
    connect(ui_->tableView->selectionModel(),&QItemSelectionModel::selectionChanged,[this]{
        ui_->deleteButton->setEnabled(ui_->tableView->selectionModel()->hasSelection());
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::on_newButton_clicked()
{
    // The whole model is normally not stored in cache,
    // so fetch the whole model before calling rowCount().
    while(model_->canFetchMore())
        model_->fetchMore();
    model_->insertRow(model_->rowCount());
    ui_->tableView->scrollToBottom();
}

void MainWindow::on_deleteButton_clicked()
{
    // Removing rows leaves them empty in the model, so no need to sort,
    // but we have to reselect data at the end.
    for(const QModelIndex& mi:(ui_->tableView->selectionModel()->selectedRows()))
        model_->removeRow(mi.row());
    model_->select();
}
