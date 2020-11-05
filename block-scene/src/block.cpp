#include "block.hpp"

#include "blockmetrics.hpp"
#include "connection.hpp"
#include "port.hpp"

#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include <algorithm>

Block::Block(QGraphicsItem* parent)
    : OpaqueGraphicsItem{parent},
      name_{tr("Block")}
{
    setFlags(ItemIsMovable|ItemIsSelectable|ItemContainsChildrenInShape|ItemSendsGeometryChanges);
    setInputs(2);
    setOutputs(2);
}

void Block::setName(QString newName)
{
    if(name_!=newName){
        prepareGeometryChange();
        name_ = std::move(newName);
        recalcSize();
        double newRight = w_-BlockMetrics::get().portSize();
        for(auto port:outputs_)
            port->setX(newRight);
    }
}

void Block::setInputs(int n)
{
    setIOs(n,PortType::input);
}

void Block::setOutputs(int n)
{
    setIOs(n,PortType::output);
}

int Block::type() const
{
    return Type;
}

QRectF Block::boundingRect() const
{
    // boundingRect should accoun for the pen width used to draw border.
    // We use the wider selection pen to account for selection.
    double halfSelectedPen = BlockMetrics::get().selectedPen().widthF()/2.;
    return {-halfSelectedPen,-halfSelectedPen,w_+halfSelectedPen,h_+halfSelectedPen};
}

void Block::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)
{
    auto& bm = BlockMetrics::get();
    auto rect = boundingRect();
    bool is_selected = option->state&QStyle::State_Selected;
    const QPen& borderPen = is_selected?bm.selectedPen():bm.normalPen();
    const QPen& innerPen = bm.normalPen();
    double left = bm.portSize(),
           right = w_-bm.portSize();
    painter->setPen(borderPen);
    painter->setBrush(is_selected?bm.selectedBrush():bm.normalBrush());
    painter->drawRect(QRectF{0,0,w_,h_});
    painter->setPen(innerPen);
    painter->drawLine(QPointF{left,0},QPointF{left,h_});
    painter->drawLine(QPointF{right,0},QPointF{right,h_});
    painter->drawText(rect,Qt::AlignHCenter|Qt::AlignVCenter,name_);
}

void Block::recalcSize()
{
    auto& bm = BlockMetrics::get();
    double halfPen = bm.selectedPen().widthF()/2.;
    auto textRect = bm.fontMetrics().boundingRect(name_);
    w_ = 2*(bm.portSize()+bm.hMargin())+textRect.width();
    h_ = std::max(2*bm.hMargin()+textRect.height(),
                  bm.portTop(std::max(inputs_.size(),outputs_.size())));
    update();
}

void Block::setIOs(int n,PortType portType)
{
    Q_ASSERT(n>=0);
    auto& v = portType==PortType::input?inputs_:outputs_;
    if(n==v.size())
        return;
    prepareGeometryChange();
    if(n<v.size()){
        for(int i=n;i<v.size();++i)
            delete v[i];
        v.remove(n,v.size()-n);
    }else{
        v.reserve(n);
        auto& bm = BlockMetrics::get();
        double x = portType==PortType::input?0:w_-bm.portSize();
        for(int i=v.size();i<n;++i){
            auto port = new Port{*this,portType};
            port->setPos(x,bm.portTop(i));
            v.append(port);
        }
    }
    recalcSize();
}

QVariant Block::itemChange(GraphicsItemChange change,const QVariant& value)
{
    if(change==ItemPositionHasChanged){
        auto updateConnectionPos = [](QVector<Port*>& v){
            for(auto port:v)
                if(auto c = port->connection())
                    c->updatePos();
        };
        updateConnectionPos(inputs_);
        updateConnectionPos(outputs_);
    }
    return QGraphicsItem::itemChange(change,value);
}
