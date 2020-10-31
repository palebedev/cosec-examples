#include "mymodel.hpp"

MyModel::MyModel(QObject* parent)
    : QAbstractTableModel{parent}
{
    for(int i=0;i<rows_;++i)
        for(int j=0;j<columns_;++j)
            storage_ += tr("Item %1-%2").arg(i+1).arg(j+1);
}

int MyModel::rowCount(const QModelIndex& index) const
{
    // In a regular table only root element has a subtable.
    return index.isValid()?0:rows_;
}

int MyModel::columnCount(const QModelIndex& index) const
{
    return index.isValid()?0:columns_;
}

QVariant MyModel::data(const QModelIndex& index,int role) const
{
    // Root element of the regular table contains no data.
    if(index.isValid())
        switch(role){
            // A role for displaying content of the item.
            case Qt::DisplayRole:
            // A role used for initializing the item's editor,
            // usually the same data as displayed.
            case Qt::EditRole:
                return storage_[index.row()*columns_+index.column()];
        }
    // Roles can return text, colors, icons, fonts and other data.
    // Everything is wrapped in a QVariant. A default-constructed one
    // is returned when no data is available.
    return {};
}

Qt::ItemFlags MyModel::flags(const QModelIndex& index) const
{
    // All table items are enabled, selectable and editable.
    return index.isValid()?Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable:Qt::ItemFlags();
}

bool MyModel::setData(const QModelIndex& index,const QVariant& value,int role)
{
    if(index.isValid())
        switch(role){
            case Qt::EditRole:
                storage_[index.row()*columns_+index.column()] = value.toString();
                // A model is responsible for notifying views of changes in its content.
                // dataChanged should be emitted for changes in a rectangular
                // area in a layer of data specified by two corners.
                emit dataChanged(index,index,{role});
                return true;
        }
    // False is returned for rejected requests to change data.
    return false;
}

// Models that can change its size must override
// insertRows/insertColumns/removeRows/removeColumns and
// use beginInsertRows/endInsertRows and similar paired
// protected functions to properly signal models of these changes.
// When the data is moved around inside the model,
// {begin,end}Mode{Rows,Columns} should be called for optimal
// view updates.
