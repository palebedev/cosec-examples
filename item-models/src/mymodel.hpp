#ifndef UUID_899C5745_93CB_4C41_B466_3BB211DFCC15
#define UUID_899C5745_93CB_4C41_B466_3BB211DFCC15

#include <QtCore/QAbstractTableModel>

// QAbstractTableModel is a base class for tabular data
// which implements parts of QAbstractItemModel interface
// common to all tables. (QAbstractListModel does the same
// for lists.) The general QAbsractItemModel describes
// a data structure which starts from the root item which
// can have a subtable of elements, where each element
// may have a subtable of its own and so on.
// For a normal table, only the root element has a subtable.
// To identify items in models a QModelIndex is used that
// stores row/column indices plus a pointer (not needed for
// a simple table). A default-constructed QItemModel returns
// true for isValid() and refers to the root item.
class MyModel : public QAbstractTableModel
{
public:
    MyModel(QObject* parent = nullptr);

    // Number of rows in a subtable for the item at index.
    int rowCount(const QModelIndex& index = {}) const override;
    // Number of columns in a subtable for the item at index.
    int columnCount(const QModelIndex& index = {}) const override;
    // Data for item at index with a specific role.
    QVariant data(const QModelIndex& index,int role = Qt::DisplayRole) const override;
    // To make the model write-enabled, we must return proper
    // flags for items and also implement setData.
    Qt::ItemFlags flags(const QModelIndex&) const override;
    bool setData(const QModelIndex& index,const QVariant& value,int role = Qt::EditRole) override;
private:
    // For this simple model, it itself contains the data
    // it represents (a 2x3 table stored as a single array
    // with strided access). Most real models adapt external data.
    constexpr static int rows_ = 2;
    constexpr static int columns_ = 3;
    QVector<QString> storage_;
};

#endif
