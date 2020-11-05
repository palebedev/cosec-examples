#include "connection.hpp"

#include "blockmetrics.hpp"
#include "port.hpp"

#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionGraphicsItem>

Connection::Connection(Port& output)
    : output_{&output}
{
    setFlags(ItemIsSelectable);
}

Connection::~Connection()
{
    output_->disconnect();
    if(input_)
        input_->disconnect();
}

void Connection::setEnd(QPointF end)
{
    input_ = nullptr;
    updatePos(end);
}

void Connection::setEnd(Port& input)
{
    input_ = &input;
    updatePos();
}

void Connection::updatePos(QPointF end)
{
    prepareGeometryChange();
    auto& bm = BlockMetrics::get();
    QPointF p1 = output_->mapToScene(output_->boundingRect().center()),
            c1 = {p1.x()+bm.curveLead(),p1.y()},
            p2 = input_?input_->mapToScene(input_->boundingRect().center()):end,
            c2 = {p2.x()-bm.curveLead(),p2.y()};
    path_.clear();
    path_.moveTo(p1);
    path_.cubicTo(c1,c2,p2);
    auto rect = path_.boundingRect();
    setPos(rect.topLeft());
    path_ = mapFromScene(path_);
    update();
}

int Connection::type() const
{
    return Type;
}

QRectF Connection::boundingRect() const
{
    auto rect = path_.boundingRect();
    double halfSelectedPen = BlockMetrics::get().selectedPen().widthF()/2.;
    rect.adjust(-halfSelectedPen,-halfSelectedPen,halfSelectedPen,halfSelectedPen);
    return rect;
}

QPainterPath Connection::shape() const
{
    return path_;
}

void Connection::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)
{
    auto& bm = BlockMetrics::get();
    painter->setPen(option->state&QStyle::State_Selected?bm.selectedPen():bm.normalPen());
    painter->drawPath(path_);
}
