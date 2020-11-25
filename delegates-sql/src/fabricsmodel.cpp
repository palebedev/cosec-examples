#include "fabricsmodel.hpp"

#include <QtCore/QDateTime>
#include <QtGui/QColor>

FabricsModel::FabricsModel(QObject* parent)
    : QSqlTableModel{parent}
{
    setTable(QStringLiteral("fabrics"));
    setEditStrategy(OnFieldChange);
    select();
    setHeaderData(1,Qt::Horizontal,tr("Name"));
    setHeaderData(2,Qt::Horizontal,tr("Color"));
    setHeaderData(3,Qt::Horizontal,tr("Available from"));
}

Qt::ItemFlags FabricsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags ret = QSqlTableModel::flags(index);
    if(index.isValid()&&index.column()==0)
        ret &= ~Qt::ItemIsEditable;
    return ret;
}

QVariant FabricsModel::data(const QModelIndex& index,int role) const
{
    QVariant ret = QSqlTableModel::data(index,role);

    if(index.isValid()&&(role==Qt::DisplayRole||role==Qt::EditRole))
        switch(index.column()){
            case 2:
                ret = QColor::fromRgb(ret.value<qlonglong>());
                break;
            case 3:
                ret = QDateTime::fromSecsSinceEpoch(ret.value<qlonglong>());
        }
    return ret;
}

bool FabricsModel::setData(const QModelIndex& index,const QVariant& value,int role)
{
    if(index.isValid()&&role==Qt::EditRole)
        switch(index.column()){
            case 2:
                return QSqlTableModel::setData(index,value.value<QColor>().rgb(),role);
            case 3:
                return QSqlTableModel::setData(index,value.toDateTime().toSecsSinceEpoch(),role);
        }
    return QSqlTableModel::setData(index,value,role);
}
