#include "blocktypes.hpp"

#include "block.hpp"

#include <QtCore/QCoreApplication>

const QVector<BlockType>& getRegisteredBlockTypes()
{
    const char* ctx = "Block types";
    static const QVector<BlockType> blockTypes = {
        {qApp->translate(ctx,"Block"),[]() -> QGraphicsItem* {
            return new Block;
        }}
    };
    return blockTypes;
}
