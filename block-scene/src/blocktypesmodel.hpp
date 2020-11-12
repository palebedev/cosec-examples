#ifndef UUID_D6BE5100_8A2F_41F4_9593_036CCFFBFD75
#define UUID_D6BE5100_8A2F_41F4_9593_036CCFFBFD75

#include <QtCore/QAbstractListModel>

class BlockTypesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using QAbstractListModel::QAbstractListModel;

    int rowCount(const QModelIndex& index = {}) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index,int role = Qt::DisplayRole) const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
};

#endif
