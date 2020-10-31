#include "draw_widget.hpp"

#include <QtGui/QMouseEvent>
#include <QtGui/QBrush>
#include <QtGui/QPainter>
#include <QtGui/QPen>

void DrawWidget::mousePressEvent(QMouseEvent* event)
{
    points_ += event->localPos();
    if(points_.size()>3)
        points_.removeFirst();
    update();
    event->accept();
}

void DrawWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter{this};
    QBrush brush{Qt::red};
    brush.setStyle(Qt::DiagCrossPattern);
    painter.setBrush(brush);
    QPen pen{QColor::fromRgbF(0.1,0.5,0.7)};
    pen.setStyle(Qt::DashLine);
    pen.setWidth(4);
    painter.setPen(pen);
    for(auto&& point:points_)
        painter.drawEllipse(point,30,30);
}
