#include "port.hpp"

#include "block.hpp"
#include "blockmetrics.hpp"
#include "connection.hpp"

#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsSceneMouseEvent>

Port::Port(Block& parent,PortType portType)
    : OpaqueGraphicsItem{&parent},
      portType_{portType}
{}

Port::~Port()
{
    delete connection_;
}

int Port::type() const
{
    return Type;
}

QRectF Port::boundingRect() const
{
    return BlockMetrics::get().portRect();
}

void Port::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* /*widget*/)
{
    auto& bm = BlockMetrics::get();
    painter->setPen(bm.normalPen());
    painter->setBrush(portType_==PortType::input?bm.inputPortBrush():bm.outputPortBrush());
    painter->drawRect(bm.portRect());
}

void Port::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(portType_==PortType::output&&!connection_&&event->button()==Qt::LeftButton){
        connection_ = new Connection{*this};
        scene()->addItem(connection_);
        creatingConnection_ = true;
    }
}

void Port::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(!creatingConnection_)
        return;
    if(auto port = qgraphicsitem_cast<Port*>(scene()->itemAt(event->scenePos(),{}));
            port&&port->portType()==PortType::input)
        connection_->setEnd(*port);
    else
        connection_->setEnd(event->scenePos());
}

void Port::mouseReleaseEvent(QGraphicsSceneMouseEvent* /*event*/)
{
    if(!creatingConnection_)
        return;
    if(connection_->input())
        connection_->input()->connection_ = connection_;
    else
        delete connection_;
    creatingConnection_ = false;
}
