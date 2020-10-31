#include "testwidget.hpp"
#include "ui_testwidget.h"

#include "mymodel.hpp"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTableWidgetItem>

TestWidget::TestWidget(QWidget *parent) :
    QWidget{parent},
    ui_{new Ui::TestWidget}
{
    ui_->setupUi(this);
    // Q{List,Table,Tree}Widget store displayed data in themselves
    // without using an external model.
    ui_->tableWidget->setRowCount(2);
    ui_->tableWidget->setColumnCount(3);
    for(int i=0;i<2;++i)
        for(int j=0;j<3;++j){
            QTableWidgetItem* item = new QTableWidgetItem;
            item->setText(QStringLiteral("Item %1-%2").arg(i+1).arg(j+1));
            ui_->tableWidget->setItem(i,j,item);
            // These widgets own their items.
        }
    // Q{List,Table,Tree,Column}View use an external model.
    ui_->tableView->setModel(new MyModel{ui_->tableView});
}

TestWidget::~TestWidget() = default;

void TestWidget::on_widgetButton_clicked()
{
    auto item = ui_->tableWidget->currentItem();
    if(item)
        QMessageBox::information(this,tr("QTableWidget selection"),item->text());
}

void TestWidget::on_viewButton_clicked()
{
    auto index = ui_->tableView->currentIndex();
    if(index.isValid())
        QMessageBox::information(this,tr("QTableWidget selection"),
                                 ui_->tableView->model()->data(index,Qt::DisplayRole).toString());
}

// A third solution is to use a View widget with a QStandardItemModel.
