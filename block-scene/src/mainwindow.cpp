#include "mainwindow.hpp"

#include "ui_mainwindow.h"

#include "block.hpp"
#include "blockpropertiesmodel.hpp"
#include "blockscene.hpp"
#include "blocktypes.hpp"
#include "blocktypesmodel.hpp"

#include <QtCore/QTimer>
#include <QtWidgets/QGraphicsScene>

MainWindow::MainWindow(QWidget* parent)
    : QWidget{parent},
      ui_{new Ui::MainWindow},
      btm_{new BlockTypesModel{this}},
      scene_{new BlockScene{this}},
      bpm_{new BlockPropertiesModel{this}}
{
    ui_->setupUi(this);
    // Horizontal splitter sets a very inconvenient size.
    // Wait until we're shown to see the actual width and
    // resize manually.
    QTimer::singleShot(0,[this]{
        auto sizes = ui_->hSplitter->sizes();
        int sum = sizes[0]+sizes[1];
        ui_->hSplitter->setSizes({sum/4,sum/4*3});
    });
    ui_->toolBoxView->setModel(btm_);
    ui_->blockView->setScene(scene_);
    ui_->propertiesView->setModel(bpm_);
    connect(scene_,&QGraphicsScene::selectionChanged,[this]{
        auto si = scene_->selectedItems();
        bpm_->setBlock(si.size()==1?qgraphicsitem_cast<Block*>(si.first()):nullptr);
    });
}

MainWindow::~MainWindow() = default;
