#include "blockscene.hpp"

#include "blocktypes.hpp"
#include "connection.hpp"

#include <QtCore/QMimeData>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsSceneDragDropEvent>

#include <algorithm>

void BlockScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if(event->mimeData()->hasFormat(blockTypeMIMEtype)){
        auto types = event->mimeData()->data(blockTypeMIMEtype).split('\0');
        if(types.size()==2&&types.last().isEmpty()){
            auto bts = getRegisteredBlockTypes();
            auto it = std::find_if(bts.begin(),bts.end(),[name=QString::fromUtf8(types.first())]
                                                         (const BlockType& bt){
                return bt.name==name;
            });
            if(it!=bts.end()){
                factory_ = it->factory;
                event->acceptProposedAction();
            }
        }
    }
}

void BlockScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    event->acceptProposedAction();
}

void BlockScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    auto item = factory_();
    addItem(item);
    item->setPos(event->scenePos());
}

void BlockScene::keyPressEvent(QKeyEvent* event)
{
    if(event->key()==Qt::Key_Delete){
        // First pass - delete only connections.
        // Deleting blocks might cause deletion of connections which
        // will become dangling for us.
        for(auto si = selectedItems();auto i:si)
            if(auto c = qgraphicsitem_cast<Connection*>(i))
                delete c;
        for(auto si = selectedItems();auto i:si)
            delete i;
        return;
    }
    QGraphicsScene::keyPressEvent(event);
}
