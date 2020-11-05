#ifndef UUID_2E5F4F89_7E8C_4B9A_BD9C_B3BD47D1496F
#define UUID_2E5F4F89_7E8C_4B9A_BD9C_B3BD47D1496F

#include <QtCore/QString>
#include <QtCore/QVector>

class QGraphicsItem;

struct BlockType
{
    QString name;
    QGraphicsItem* (*factory)();
};

const inline QString blockTypeMIMEtype = QStringLiteral("application/x.block-scene-block-type");

const QVector<BlockType>& getRegisteredBlockTypes();

#endif
