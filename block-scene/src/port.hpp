#ifndef UUID_67949EDD_F4C0_4E37_9EE5_D2B971CBC45D
#define UUID_67949EDD_F4C0_4E37_9EE5_D2B971CBC45D

#include "opaquegraphicsitem.hpp"

class Block;
class Connection;

enum class PortType
{
    input,
    output
};

class Port : public OpaqueGraphicsItem
{
public:
    enum { Type = UserType+2 };

    Port(Block& parent,PortType portType);
    ~Port();

    PortType portType() const noexcept
    {
        return portType_;
    }

    Connection* connection() const noexcept
    {
        return connection_;
    }

    void disconnect()
    {
        connection_ = nullptr;
    }

    int type() const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,
               QWidget* /*widget*/ = nullptr) override;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* /*event*/) override;
private:
    PortType portType_;
    Connection* connection_ = nullptr;
    bool creatingConnection_ = false;
};

#endif
