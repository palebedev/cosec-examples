#ifndef UUID_8A0F3A45_9817_400A_97D4_4FE2B845ABB0
#define UUID_8A0F3A45_9817_400A_97D4_4FE2B845ABB0

#include <QtSql/QSqlTableModel>

class FabricsModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit FabricsModel(QObject* parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index,int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index,const QVariant& value,int role = Qt::EditRole) override;
};

#endif
