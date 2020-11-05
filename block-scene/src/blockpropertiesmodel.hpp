#ifndef UUID_4919FBB3_5EF5_4EFF_AFF3_68306F9C2755
#define UUID_4919FBB3_5EF5_4EFF_AFF3_68306F9C2755

#include <QtCore/QAbstractTableModel>

class Block;

class BlockPropertiesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    using QAbstractTableModel::QAbstractTableModel;

    Block* block() const noexcept
    {
        return block_;
    }

    void setBlock(Block* newBlock);

    int rowCount(const QModelIndex& index = {}) const override;
    int columnCount(const QModelIndex& index = {}) const override;
    QVariant headerData(int section,Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index,int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index,const QVariant& value,int role = Qt::EditRole) override;
private:
    Block* block_ = nullptr;
};

#endif
