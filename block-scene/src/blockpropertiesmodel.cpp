#include "blockpropertiesmodel.hpp"

#include "block.hpp"

namespace
{
    constexpr int propertiesCount = 3,
                  maxPorts = 16;
}

void BlockPropertiesModel::setBlock(Block* newBlock)
{
    if(block_==newBlock)
        return;
    if(!block_){
        beginInsertRows({},0,propertiesCount-1);
        block_ = newBlock;
        endInsertRows();
    }else if(!newBlock){
        beginRemoveRows({},0,propertiesCount-1);
        block_ = newBlock;
        endRemoveRows();
    }else{
        block_ = newBlock;
        emit dataChanged(index(0,0),index(propertiesCount-1,1));
    }
}

int BlockPropertiesModel::rowCount(const QModelIndex& index) const
{
    return !index.isValid()&&block_?propertiesCount:0;
}

int BlockPropertiesModel::columnCount(const QModelIndex& index) const
{
    return !index.isValid()?2:0;
}

QVariant BlockPropertiesModel::headerData(int section,Qt::Orientation orientation,int role) const
{
    if(orientation==Qt::Horizontal&&role==Qt::DisplayRole)
        switch(section){
            case 0:
                return tr("Property");
            case 1:
                return tr("Value");
            default:
                ;
        }
    return {};
}

Qt::ItemFlags BlockPropertiesModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return {};
    Qt::ItemFlags ret = Qt::ItemIsEnabled|Qt::ItemNeverHasChildren;
    if(index.column()==1)
        ret |= Qt::ItemIsSelectable|Qt::ItemIsEditable;
    return ret;
}

QVariant BlockPropertiesModel::data(const QModelIndex& index,int role) const
{
    if(index.isValid()&&(role==Qt::DisplayRole||(index.column()==1&&role==Qt::EditRole)))
        switch(index.row()){
            case 0:
                return !index.column()?tr("Name"):block_->name();
            case 1:
                return !index.column()?QVariant{tr("Inputs")}:QVariant{block_->inputs()};
            case 2:
                return !index.column()?QVariant{tr("Outputs")}:QVariant{block_->outputs()};
            default:
                ;
        }
    return {};
}

bool BlockPropertiesModel::setData(const QModelIndex& index,const QVariant& value,int role)
{
    if(!index.isValid()||index.column()!=1||role!=Qt::EditRole)
        return {};
    switch(index.row()){
        case 0:
            block_->setName(value.toString());
            break;
        case 1:
            {
                bool ok;
                int n = value.toInt(&ok);
                if(!ok||n<0||n>maxPorts)
                    return false;
                block_->setInputs(n);
            }
            break;
        case 2:
            {
                bool ok;
                int n = value.toInt(&ok);
                if(!ok||n<0||n>maxPorts)
                    return false;
                block_->setOutputs(n);
            }
            break;
        default:
            return false;
    }
    emit dataChanged(index,index);
    return true;
}
