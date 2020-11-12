#include "mainwindow.hpp"

#include "ui_mainwindow.h"

#include "colordelegate.hpp"
#include "fabricsmodel.hpp"

#include <algorithm>
#include <functional>

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
    int rc = model_->rowCount();
    model_->insertRow(rc);
    ui_->tableView->scrollToBottom();
}

void MainWindow::on_deleteButton_clicked()
{
    // Get a descending list of row addresses so that removing them in this
    // order doesn't invalidate them.
    auto indices = ui_->tableView->selectionModel()->selectedRows();
    QVector<int> rows_(indices.size());
    std::transform(indices.begin(),indices.end(),rows_.begin(),[](const QModelIndex& i){
        return i.row();
    });
    std::sort(rows_.begin(),rows_.end(),std::greater<>{});
    for(int row:rows_)
        model_->removeRow(row);
    // Removing rows leaves them empty in the model, reselect to purge them.
    model_->select();
}
