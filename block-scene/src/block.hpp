#ifndef UUID_3F9C3E3F_0FCF_4B93_97BA_683857C4B90D
#define UUID_3F9C3E3F_0FCF_4B93_97BA_683857C4B90D

#include "opaquegraphicsitem.hpp"

#include <QtCore/QCoreApplication>

enum class PortType;
class Port;

class Block : public OpaqueGraphicsItem
{
    Q_DECLARE_TR_FUNCTIONS(Block)
public:
    enum { Type = UserType+1 };

    Block(QGraphicsItem* parent = nullptr);

    const QString& name() const noexcept
    {
        return name_;
    }

    void setName(QString newName);

    int inputs() const noexcept
    {
        return inputs_.size();
    }

    void setInputs(int n);

    int outputs() const noexcept
    {
        return outputs_.size();
    }

    void setOutputs(int n);

    int type() const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
protected:
    QVariant itemChange(GraphicsItemChange change,const QVariant& value) override;
private:
    QString name_;
    double w_,h_;
    QVector<Port*> inputs_,outputs_;

    void recalcSize();
    void setIOs(int n,PortType portType);
};

#endif
