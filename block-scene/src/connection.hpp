#ifndef UUID_839F0674_FAEF_4B15_BBC4_8723F1301EA1
#define UUID_839F0674_FAEF_4B15_BBC4_8723F1301EA1

#include "opaquegraphicsitem.hpp"

class Port;

class Connection : public OpaqueGraphicsItem
{
public:
    enum { Type = UserType+2 };

    Connection(Port& output);
    ~Connection();

    Port* output() const noexcept
    {
        return output_;
    }

    Port* input() const noexcept
    {
        return input_;
    }

    void setEnd(QPointF end);
    void setEnd(Port& input);
    void updatePos(QPointF end = {});

    int type() const override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,
               QWidget* /*widget*/ = nullptr) override;
private:
    Port* output_;
    Port* input_ = nullptr;
    QPainterPath path_;
};

#endif
