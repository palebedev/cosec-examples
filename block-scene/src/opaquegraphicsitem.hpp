#ifndef UUID_E3585717_418F_4EF8_8E4C_ECA4BF14AF2F
#define UUID_E3585717_418F_4EF8_8E4C_ECA4BF14AF2F

#include <QtWidgets/QGraphicsItem>

class OpaqueGraphicsItem : public QGraphicsItem
{
public:
    using QGraphicsItem::QGraphicsItem;

    QPainterPath opaqueArea() const override;
};

#endif
