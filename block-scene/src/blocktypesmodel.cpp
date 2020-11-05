#include "blocktypesmodel.hpp"

#include "blocktypes.hpp"

#include <QtCore/QMimeData>

int BlockTypesModel::rowCount(const QModelIndex& index) const
{
    return !index.isValid()?getRegisteredBlockTypes().size():0;
}

Qt::ItemFlags BlockTypesModel::flags(const QModelIndex& index) const
{
    return index.isValid()?
        Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemNeverHasChildren:
        Qt::NoItemFlags;
}

QVariant BlockTypesModel::data(const QModelIndex& index,int role) const
{
    if(index.isValid()&&role==Qt::DisplayRole)
        return getRegisteredBlockTypes()[index.row()].name;
    return {};
}

QStringList BlockTypesModel::mimeTypes() const
{
    static const QStringList list = {blockTypeMIMEtype};
    return list;
}

QMimeData* BlockTypesModel::mimeData(const QModelIndexList& indexes) const
{
    auto md = new QMimeData;
    QString s;
    auto& bt = getRegisteredBlockTypes();
    for(const QModelIndex& i:indexes){
        s += bt[i.row()].name;
        s += QChar{};
    }
    md->setData(blockTypeMIMEtype,s.toUtf8());
    return md;
}
